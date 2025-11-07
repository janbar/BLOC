/*
 *      Copyright (C) 2022 Jean-Luc Barriere
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "member_complex.h"
#include <blocc/parse_expression.h>
#include <blocc/exception_parse.h>
#include <blocc/context.h>
#include <blocc/parser.h>
#include <blocc/debug.h>
#include <blocc/plugin_manager.h>

#include <cstring>
#include <cassert>

namespace bloc
{

MemberMETHODExpression::MemberMETHODExpression(const PLUGIN_METHOD& method, unsigned type_id, Expression * e)
: MemberExpression(BTM_undefined, e), _method(&method), _method_type_id(type_id)
{
  /* the type/decl returned by complex member are defined by the static interface,
   * so it is static */
  if (_method)
  {
    _decl_method = plugin::make_decl(_method->ret.decl, _method_type_id);
    if (_decl_method.size() == 1)
      _type_method = Type(_decl_method[0].major(), _decl_method[0].minor(), _method->ret.ndim);
    else
      _type_method = _decl_method.make_type(_method->ret.ndim);
  }
  else
    _type_method = Type::NO_TYPE;
}

Value& MemberMETHODExpression::value(Context& ctx) const
{
  if (!_method)
    throw RuntimeError(EXC_RT_MEMB_FAILED_S, KEYWORDS[_builtin]);

  Value& val = _exp->value(ctx);
  if (val.isNull())
    return val;
  const PLUGGED_MODULE& plug = PluginManager::instance().plugged(_method_type_id);
  if (val.type().minor() != _method_type_id)
    throw RuntimeError(EXC_RT_BAD_COMPLEX_S, plug.interface.name);

  Value * ret = plug.instance->executeMethod(
          *val.complex(), _method->id, ctx, _args);
  if (ret == nullptr)
    throw RuntimeError(EXC_RT_MEMB_FAILED_S, _method->name);
  if (ret->lvalue())
  {
    Value& v = ctx.allocate(ret->clone());
    delete ret;
    return v;
  }
  else
  {
    Value& v = ctx.allocate(std::move(*ret));
    delete ret;
    return v;
  }
}

std::string MemberMETHODExpression::typeName(Context& ctx) const
{
  if (_type_method.major() == Type::ROWTYPE)
    return _type_method.typeName(tuple_decl(ctx).tupleName());
  return _type_method.typeName();
}

std::string MemberMETHODExpression::unparse(Context& ctx) const
{
  std::string sb(_exp->unparse(ctx).append(1, OPERATOR));
  if (_method)
    sb.append(_method->name);
  else
    sb.append(KEYWORDS[_builtin]);
  sb.append("(");
  for (int i = 0; i < _args.size(); ++i)
  {
    if (i > 0)
      sb.append(1, Parser::Chain).append(1, ' ');
    sb.append(_args[i]->unparse(ctx));
  }
  sb.append(1, ')');
  return sb;
}

MemberMETHODExpression * MemberMETHODExpression::parse(Parser& p, Context& ctx, Expression * exp)
{
  std::vector<Expression*> args;
  const Type& exp_type = exp->type(ctx);
  unsigned type_id = exp_type.minor();
  const PLUGGED_MODULE& plug = PluginManager::instance().plugged(type_id);
  TokenPtr t = p.pop();

  try
  {
    std::string m_name = t->text;
    /* parse arguments list */
    t = p.pop();
    if (t->code != '(')
      throw ParseError(EXC_PARSE_INV_EXPRESSION, t);
    if (p.front()->code == ')')
      p.pop();
    else
    {
      for (;;)
      {
        args.push_back(ParseExpression::expression(p, ctx));
        t = p.pop();
        if (t->code == Parser::Chain)
          continue;
        if (t->code != ')')
          throw ParseError(EXC_PARSE_EXPRESSION_END_S, t->text.c_str(), t);
        break;
      }
    }

    /* find method with compatible specs
     * it returns the first found with exact match, else the one with the
     * highest score will be returned
     */
    bool f_name = false; /* a method exists with this name */
    int f_sc = 0; /* the highest score */
    int f_no = 0; /* the index of the first method with the highest score */
    for (int m = 0; m < plug.interface.method_count; ++m)
    {
      const PLUGIN_METHOD& method = plug.interface.methods[m];
      if (method.name != m_name)
        continue;
      f_name = true;
      if (method.args_count == args.size())
      {
        bool found = true;
        int match = 2 * method.args_count; /* the best score: 2 pts per arg */
        int score = 0;
        /* check arguments list */
        for (int a = 0; found && a < method.args_count; ++a)
        {
          /* the expected type */
          Type m_arg_type = plugin::make_type(method.args[a].type, type_id);
          switch (method.args[a].mode)
          {
          case PLUGIN_IN:
            /* check type */
            if (!ParseExpression::typeChecking(args[a], m_arg_type, p, ctx))
              found = false;
            else
            {
              found = true;
              const Type& _type = args[a]->type(ctx);
              if (m_arg_type.level() == _type.level())
                score += 2;
              else if (_type == Type::NO_TYPE)
                score += 1;
            }
            break;
          case PLUGIN_INOUT:
          {
            /* must be symbolic variable, not read-only, and with same type */
            if (!args[a]->isVarName() || args[a]->symbol()->locked() ||
                    !ParseExpression::typeChecking(args[a], m_arg_type, p, ctx))
              found = false;
            else
            {
              found = true;
              score += 2;
            }
            break;
          }
          default:
            found = false;
            break;
          }
        }

        if (found)
        {
          /* return the method with exact match */
          if (score == match)
          {
            DBG(DBG_DEBUG, "%s: found method %s id=%d\n", __FUNCTION__, method.name, method.id);
            return new MemberMETHODExpression(method, exp_type.minor(), exp, std::move(args));
          }
          /* else keep the one with the highest score */
          if (score > f_sc)
          {
            f_sc = score;
            f_no = m;
          }
        }
      }
      /* loop next method */
    }

    /* if score exists returns method found, otherwise fails */
    if (f_sc > 0)
    {
      const PLUGIN_METHOD& method = plug.interface.methods[f_no];
      DBG(DBG_DEBUG, "%s: found method %s id=%d\n", __FUNCTION__, method.name, method.id);
      return new MemberMETHODExpression(method, exp_type.minor(), exp, std::move(args));
    }

    if (f_name)
      throw ParseError(EXC_PARSE_MEMB_ARG_TYPE_S, m_name.c_str(), t);
    throw ParseError(EXC_PARSE_MEMB_NOT_IMPL_S, m_name.c_str(), t);
  }
  catch (ParseError& pe)
  {
    DBG(DBG_DEBUG, "exception %p at %s line %d\n", &pe, __PRETTY_FUNCTION__, __LINE__);
    for (Expression * e : args)
      delete e;
    throw;
  }
  return nullptr;
}

}

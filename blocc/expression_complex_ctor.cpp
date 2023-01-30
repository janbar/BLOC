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

#include "expression_complex_ctor.h"
#include "parse_expression.h"
#include "exception_parse.h"
#include "plugin_manager.h"
#include "parser.h"
#include "debug.h"

namespace bloc
{

ComplexCTORExpression::~ComplexCTORExpression()
{
  for (Expression * e : _args)
    delete e;
  _args.clear();
}

Complex ComplexCTORExpression::complex(Context& ctx) const
{
  Complex c(_type.minor());
  int ctor_id = (_ctor == nullptr ? (-1) : _ctor->id);
  if (c.CTOR(ctor_id, ctx, _args))
    return c;
  throw RuntimeError(EXC_RT_CTOR_FAILED_S, unparse(ctx).c_str());
}

std::string ComplexCTORExpression::unparse(Context& ctx) const
{
  const PLUGGED_MODULE& plug = PluginManager::instance().plugged(_type.minor());
  std::string sb(plug.interface.name);
  sb.append("(");
  for (Expression * e : _args)
    sb.append(e->unparse(ctx)).append(1, Parser::CHAIN);
  if (sb.back() == Parser::CHAIN)
    sb.back() = ')';
  else
    sb.push_back(')');
  return sb;
}

ComplexCTORExpression * ComplexCTORExpression::parse(Parser& p, Context& ctx, unsigned type_id)
{
  if (type_id == 0)
    throw ParseError(EXC_PARSE_NOT_COMPLEX);
  std::vector<Expression*> args;
  const PLUGGED_MODULE& plug = PluginManager::instance().plugged(type_id);

  try
  {
    /* parse arguments list */
    TokenPtr t = p.pop();
    if (t->code != '(')
      throw ParseError(EXC_PARSE_INV_EXPRESSION);
    if (p.front()->code == ')')
    {
      p.pop();
      /* default constructor */
      return new ComplexCTORExpression(type_id);
    }
    else
    {
      for (;;)
      {
        args.push_back(ParseExpression::expression(p, ctx));
        t = p.pop();
        if (t->code == Parser::CHAIN)
          continue;
        if (t->code != ')')
          throw ParseError(EXC_PARSE_EXPRESSION_END_S, t->text.c_str());
        break;
      }
    }

    /* else find the right constructor */
    bool fid = false;
    for (int m = 0; m < plug.interface.ctors_count; ++m)
    {
      const PLUGIN_CTOR& ctor = plug.interface.ctors[m];
      fid = true;
      if (ctor.args_count == args.size())
      {
        bool found = true;
        /* check arguments list */
        int a = 0;
        while (found && a < ctor.args_count)
        {
          /* the expected type */
          Type m_arg_type = plugin::make_type(ctor.args[a], type_id);
          /* check type */
          found = ParseExpression::typeChecking(args[a], m_arg_type, p, ctx);
          ++a;
        }
        if (found)
        {
          DBG(DBG_DEBUG, "%s: found ctor id=%d\n", __FUNCTION__, ctor.id);
          return new ComplexCTORExpression(type_id, ctor, std::move(args));
        }
      }
    }
    if (fid)
      throw ParseError(EXC_PARSE_MEMB_ARG_TYPE_S, plug.interface.name);
    throw ParseError(EXC_PARSE_MEMB_NOT_IMPL_S, plug.interface.name);
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

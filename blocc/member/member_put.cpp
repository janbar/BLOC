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

#include "member_put.h"
#include <blocc/parse_expression.h>
#include <blocc/exception_parse.h>
#include <blocc/collection.h>
#include <blocc/tuple.h>
#include <blocc/context.h>
#include <blocc/parser.h>
#include <blocc/debug.h>

#include <cstring>
#include <cassert>

namespace bloc
{

Value& MemberPUTExpression::value(Context& ctx) const
{
  Value& val = _exp->value(ctx);
  Value& a0 = _args[0]->value(ctx);
  if (val.isNull() || a0.isNull())
    throw RuntimeError(EXC_RT_INDEX_RANGE_S, a0.toString().c_str());
  Value& a1 = _args[1]->value(ctx);

  if (val.type().level() > 0)
  {
    Collection * rv = val.collection();
    const Type& rv_type = rv->table_type();
    Integer p = *a0.integer();
    if (p < 0 || p >= rv->size())
      throw RuntimeError(EXC_RT_INDEX_RANGE_S, a0.toString().c_str());
    const Type& a1_type = a1.type();
    /* collection */
    if (a1_type.level() > 0)
    {
      if (!a1.isNull() && a1.collection()->table_type() == rv_type.levelDown())
      {
        if (a1.lvalue())
          rv->at(p).swap(a1.clone());
        else
          rv->at(p).swap(std::move(a1));
        return val;
      }
    }
    else if (a1_type == rv_type.major())
    {
      /* tuple */
      if (a1_type == Type::ROWTYPE)
      {
        if (!a1.isNull() && a1.tuple()->tuple_type() == rv_type.levelDown())
        {
          if (a1.lvalue())
            rv->at(p).swap(a1.clone());
          else
            rv->at(p).swap(std::move(a1));
          return val;
        }
      }
      /* others */
      else if (a1_type == rv_type.levelDown())
      {
        if (a1.lvalue())
          rv->at(p).swap(a1.clone());
        else
          rv->at(p).swap(std::move(a1));
        return val;
      }
    }
    else
    {
      /* type mixing */
      switch (rv_type.major())
      {
      case Type::INTEGER:
        if (a1_type == Type::NUMERIC)
        {
          rv->at(p).swap(Value(Integer(*a1.numeric())));
          return val;
        }
        break;
      case Type::NUMERIC:
        if (a1_type == Type::INTEGER)
        {
          rv->at(p).swap(Value(Numeric(*a1.integer())));
          return val;
        }
        break;
      default:
        break;
      }
    }
    if (val.type() == Type::ROWTYPE)
      throw RuntimeError(EXC_RT_TYPE_MISMATCH_S, val.type().levelDown().typeName(rv->table_decl().tupleName()).c_str());
    throw RuntimeError(EXC_RT_TYPE_MISMATCH_S, val.type().levelDown().typeName().c_str());
  }

  switch (val.type().major())
  {
    /* literal */
  case Type::LITERAL:
  {
    Literal * rv = val.literal();
    Integer p = *a0.integer();
    if (p < 0 || p >= rv->size())
      throw RuntimeError(EXC_RT_INDEX_RANGE_S, a0.toString().c_str());
    if (a1.isNull())
      throw RuntimeError(EXC_RT_TYPE_MISMATCH_S, val.typeName().c_str());
    Integer c = *a1.integer();
    if (c < 1 || c > 255)
      throw RuntimeError(EXC_RT_OUT_OF_RANGE);
    if (_exp->isConst())
    {
      Value v(new Literal(*rv));
      v.literal()->replace(p, 1, 1, (char)c);
      return ctx.allocate(std::move(v));
    }
    rv->replace(p, 1, 1, (char)c);
    return val;
  }
    /* tabchar */
  case Type::TABCHAR:
  {
    TabChar * rv = val.tabchar();
    Integer p = *a0.integer();
    if (p < 0 || p >= rv->size())
      throw RuntimeError(EXC_RT_INDEX_RANGE_S, a0.toString().c_str());
    if (a1.isNull())
      throw RuntimeError(EXC_RT_TYPE_MISMATCH_S, val.typeName().c_str());
    Integer c = *a1.integer();
    if (c < 0 || c > 255)
      throw RuntimeError(EXC_RT_OUT_OF_RANGE);
    rv->at(p) = (char)c;
    return val;
  }
  default:
    break;
  }
  throw RuntimeError(EXC_RT_MEMB_NOT_IMPL_S, KEYWORDS[BTM_PUT]);
}

MemberPUTExpression * MemberPUTExpression::parse(Parser& p, Context& ctx, Expression * exp)
{
  std::vector<Expression*> args;
  TokenPtr t = p.pop();

  if (t->code != '(')
    throw ParseError(EXC_PARSE_BAD_MEMB_CALL_S, KEYWORDS[BTM_PUT]);
  try
  {
    const Type& exp_type = exp->type(ctx);
    /* supported type: collection, literal, tabchar */
    if (exp_type.level() == 0)
    {
      switch (exp_type.major())
      {
      case Type::NO_TYPE: /* opaque */
      case Type::LITERAL:
      case Type::TABCHAR:
        break;
      default:
        throw ParseError(EXC_PARSE_MEMB_NOT_IMPL_S, KEYWORDS[BTM_PUT]);
      }
    }

    args.push_back(ParseExpression::expression(p, ctx));
    if (!ParseExpression::typeChecking(args.back(), Type::INTEGER, p, ctx))
      throw ParseError(EXC_PARSE_MEMB_ARG_TYPE_S, KEYWORDS[BTM_PUT]);
    t = p.pop();
    if (t->code != Parser::CHAIN)
      throw ParseError(EXC_PARSE_MEMB_ARG_NUM_S, KEYWORDS[BTM_PUT]);

    args.push_back(ParseExpression::expression(p, ctx));
    if (exp_type.level() == 0)
    {
      switch (exp_type.major())
      {
      case Type::NO_TYPE: /* opaque */
        break;
      case Type::LITERAL: /* PUT a char */
      case Type::TABCHAR: /* PUT a char */
        if (!ParseExpression::typeChecking(args.back(), Type::INTEGER, p, ctx))
          throw ParseError(EXC_PARSE_MEMB_ARG_TYPE_S, KEYWORDS[BTM_PUT]);
        break;
      default:
        throw ParseError(EXC_PARSE_MEMB_NOT_IMPL_S, KEYWORDS[BTM_PUT]);
      }
    }
    /* collection PUT */
    else
    {
      const Type& arg_type = args.back()->type(ctx);
      /* type opaque or tuple opaque */
      bool exp_opaque = (exp_type == Type::NO_TYPE || (exp_type == Type::ROWTYPE && exp_type.minor() == 0));
      bool arg_opaque = (arg_type == Type::NO_TYPE || (arg_type == Type::ROWTYPE && arg_type.minor() == 0));

      /* test known types are compatible */
      if (!exp_opaque && !arg_opaque)
      {
        if (!ParseExpression::typeChecking(args.back(), exp_type.levelDown(), p, ctx))
          throw ParseError(EXC_PARSE_TYPE_MISMATCH_S, exp->typeName(ctx).c_str());
      }
    }
    assertClosedMember(p, ctx, KEYWORDS[BTM_PUT]);
    return new MemberPUTExpression(exp, std::move(args));
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

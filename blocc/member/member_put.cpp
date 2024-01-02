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
#include <blocc/expression_boolean.h>
#include <blocc/expression_integer.h>
#include <blocc/expression_numeric.h>
#include <blocc/expression_literal.h>
#include <blocc/expression_complex.h>
#include <blocc/expression_tabchar.h>
#include <blocc/expression_collection.h>
#include <blocc/expression_tuple.h>
#include <blocc/context.h>
#include <blocc/parser.h>
#include <blocc/debug.h>

#include <cstring>
#include <cassert>

namespace bloc
{

std::string& MemberPUTExpression::literal(Context& ctx) const
{
  /* literal */
  std::string& rv = _exp->literal(ctx);
  int64_t p = _args[0]->integer(ctx);
  int64_t c = _args[1]->integer(ctx);
  if (p < 0 || p >= rv.size())
    throw RuntimeError(EXC_RT_INDEX_RANGE_S, std::to_string(p).c_str());
  if (c < 1 || c > 255)
    throw RuntimeError(EXC_RT_OUT_OF_RANGE);
  return rv.replace(p, 1, 1, (char)c);
}

TabChar& MemberPUTExpression::tabchar(Context& ctx) const
{
  /* tabchar */
  TabChar& rv = _exp->tabchar(ctx);
  int64_t p = _args[0]->integer(ctx);
  int64_t c = _args[1]->integer(ctx);
  if (p < 0 || p >= rv.size())
    throw RuntimeError(EXC_RT_INDEX_RANGE_S, std::to_string(p).c_str());
  if (c < 0 || c > 255)
    throw RuntimeError(EXC_RT_OUT_OF_RANGE);
  rv[p] = (char)c;
  return rv;
}

Collection& MemberPUTExpression::collection(Context& ctx) const
{
  /* collection */
  Collection& rv = _exp->collection(ctx);
  const Type& rv_type = rv.table_type();
  int64_t p = _args[0]->integer(ctx);
  if (p < 0 || p >= rv.size())
    throw RuntimeError(EXC_RT_INDEX_RANGE_S, std::to_string(p).c_str());

  const Type& arg_type = _args[1]->type(ctx);

  /* collection */
  if (arg_type.level() > 0)
  {
    Collection& a = _args[1]->collection(ctx);
    if (a.table_type() == rv_type.levelDown())
    {
      /* the existing element will be freed after completion */
      StaticExpression * old = rv[p];
      if (_args[1]->isRvalue())
        rv[p] = new CollectionExpression(std::move(a));
      else
        rv[p] = new CollectionExpression(a);
      /* free old element */
      if (old) delete old;
      return rv;
    }
    throw RuntimeError(EXC_RT_TYPE_MISMATCH_S, _exp->typeName(ctx).c_str());
  }
  else if (arg_type == rv_type.major())
  {
    /* tuple */
    if (arg_type == Type::ROWTYPE)
    {
      Tuple& t = _args[1]->tuple(ctx);
      if (t.tuple_type() == rv_type.levelDown())
      {
        /* the existing element will be freed after completion */
        StaticExpression * old = rv[p];
        if (_args[1]->isRvalue())
          rv[p] = new TupleExpression(std::move(t));
        else
          rv[p] = new TupleExpression(t);
        /* free old element */
        if (old) delete old;
        return rv;
      }
      throw RuntimeError(EXC_RT_TYPE_MISMATCH_S, _exp->typeName(ctx).c_str());
    }
    /* others */
    else if (arg_type == rv_type.levelDown())
    {
      /* the existing element will be freed after completion */
      StaticExpression * old = rv[p];
      switch (arg_type.major())
      {
      case Type::BOOLEAN:
        rv[p] = new BooleanExpression(_args[1]->boolean(ctx));
        break;
      case Type::INTEGER:
        rv[p] = new IntegerExpression(_args[1]->integer(ctx));
        break;
      case Type::NUMERIC:
        rv[p] = new NumericExpression(_args[1]->numeric(ctx));
        break;
      case Type::COMPLEX:
        if (_args[1]->isRvalue())
          rv[p] = new ComplexExpression(std::move(_args[1]->complex(ctx)));
        else
          rv[p] = new ComplexExpression(_args[1]->complex(ctx));
        break;
      case Type::LITERAL:
        if (_args[1]->isRvalue())
          rv[p] = new LiteralExpression(std::move(_args[1]->literal(ctx)));
        else
          rv[p] = new LiteralExpression(_args[1]->literal(ctx));
        break;
      case Type::TABCHAR:
        if (_args[1]->isRvalue())
          rv[p] = new TabcharExpression(std::move(_args[1]->tabchar(ctx)));
        else
          rv[p] = new TabcharExpression(_args[1]->tabchar(ctx));
        break;
      default:
        throw RuntimeError(EXC_RT_NOT_IMPLEMENTED);
      }
      /* free old element */
      if (old) delete old;
      return rv;
    }
  }
  throw RuntimeError(EXC_RT_TYPE_MISMATCH_S, _exp->typeName(ctx).c_str());
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

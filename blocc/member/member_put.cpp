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
  int64_t p = _args[0]->integer(ctx);
  if (p < 0 || p >= rv.size())
    throw RuntimeError(EXC_RT_INDEX_RANGE_S, std::to_string(p).c_str());

  /* the existing element will be freed after completion */
  StaticExpression * old = rv[p];

  /* put the new */
  const Type& arg1_type = _args[1]->type(ctx);
  if (arg1_type.level() == 0)
  {
    switch (arg1_type.major())
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
      if (_args[1]->isRvalue(ctx))
        rv[p] = new ComplexExpression(std::move(_args[1]->complex(ctx)));
      else
        rv[p] = new ComplexExpression(_args[1]->complex(ctx));
      break;
    case Type::LITERAL:
      if (_args[1]->isRvalue(ctx))
        rv[p] = new LiteralExpression(std::move(_args[1]->literal(ctx)));
      else
        rv[p] = new LiteralExpression(_args[1]->literal(ctx));
      break;
    case Type::TABCHAR:
      if (_args[1]->isRvalue(ctx))
        rv[p] = new TabcharExpression(std::move(_args[1]->tabchar(ctx)));
      else
        rv[p] = new TabcharExpression(_args[1]->tabchar(ctx));
      break;
    case Type::ROWTYPE:
      if (_args[1]->isRvalue(ctx))
        rv[p] = new TupleExpression(std::move(_args[1]->tuple(ctx)));
      else
        rv[p] = new TupleExpression(_args[1]->tuple(ctx));
      break;
    default:
      throw RuntimeError(EXC_RT_NOT_IMPLEMENTED);
    }
  }
  else
  {
    if (_args[1]->isRvalue(ctx))
      rv[p] = new CollectionExpression(std::move(_args[1]->collection(ctx)));
    else
      rv[p] = new CollectionExpression(_args[1]->collection(ctx));
  }

  /* free old element */
  if (old) delete old;
  return rv;
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
        /* PUT a char */
      case Type::LITERAL:
      case Type::TABCHAR:
        if (!ParseExpression::typeChecking(args.back(), Type::INTEGER, p, ctx))
          throw ParseError(EXC_PARSE_MEMB_ARG_TYPE_S, KEYWORDS[BTM_PUT]);
        break;
      default:
        throw ParseError(EXC_PARSE_MEMB_NOT_IMPL_S, KEYWORDS[BTM_PUT]);
      }
    }
    else
    {
      /* PUT element type */
      if (!ParseExpression::typeChecking(args.back(), exp_type.levelDown(), p, ctx))
        throw ParseError(EXC_PARSE_TYPE_MISMATCH_S, exp->typeName(ctx).c_str());
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

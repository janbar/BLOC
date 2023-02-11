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

#include "member_insert.h"
#include <blocc/parse_expression.h>
#include <blocc/exception_parse.h>
#include <blocc/expression_boolean.h>
#include <blocc/expression_integer.h>
#include <blocc/expression_numeric.h>
#include <blocc/expression_literal.h>
#include <blocc/expression_complex.h>
#include <blocc/expression_tabchar.h>
#include <blocc/expression_tuple.h>
#include <blocc/expression_collection.h>
#include <blocc/context.h>
#include <blocc/parser.h>
#include <blocc/debug.h>

#include <cstring>
#include <cassert>

namespace bloc
{

std::string& MemberINSERTExpression::literal(Context& ctx) const
{
  /* literal */
  std::string& rv = _exp->literal(ctx);
  int64_t p = _args[0]->integer(ctx);
  if (p < 0 || p > rv.size())
    throw RuntimeError(EXC_RT_INDEX_RANGE_S, std::to_string(p).c_str());
  switch (_args[0]->type(ctx).major())
  {
  case Type::LITERAL:
    rv.insert(p, _args[1]->literal(ctx));
    return rv;
  case Type::INTEGER:
  {
    int64_t c = _args[1]->integer(ctx);
    if (c < 1 || c > 255)
      throw RuntimeError(EXC_RT_OUT_OF_RANGE);
    rv.insert(p, 1, (char)c);
    return rv;
  }
  default:
    break;
  }

  throw RuntimeError(EXC_RT_MEMB_ARG_TYPE_S, KEYWORDS[_builtin]);
}

TabChar& MemberINSERTExpression::tabchar(Context& ctx) const
{
  /* tabchar */
  TabChar& rv = _exp->tabchar(ctx);
  int64_t p = _args[0]->integer(ctx);
  if (p < 0 || p > rv.size())
    throw RuntimeError(EXC_RT_INDEX_RANGE_S, std::to_string(p).c_str());
  switch (_args[0]->type(ctx).major())
  {
  case Type::TABCHAR:
  {
    TabChar& a = _args[1]->tabchar(ctx);
    if (&a == &rv)
    {
      TabChar _a(a);
      rv.insert(rv.begin() + p, _a.begin(), _a.end());
    }
    else
      rv.insert(rv.begin() + p, a.begin(), a.end());
    return rv;
  }
  case Type::LITERAL:
  {
    std::string& a = _args[1]->literal(ctx);
    rv.insert(rv.begin() + p, a.begin(), a.end());
    return rv;
  }
  case Type::INTEGER:
  {
    int64_t c = _args[1]->integer(ctx);
    if (c < 0 || c > 255)
      throw RuntimeError(EXC_RT_OUT_OF_RANGE);
    rv.insert(rv.begin() + p, (char)c);
    return rv;
  }
  default:
    break;
  }

  throw RuntimeError(EXC_RT_MEMB_ARG_TYPE_S, KEYWORDS[_builtin]);
}

Collection& MemberINSERTExpression::collection(Context& ctx) const
{
  const Type& arg_type = _args[1]->type(ctx);
  const Type& exp_type = _exp->type(ctx);
  Collection& rv = _exp->collection(ctx);
  int64_t p = _args[0]->integer(ctx);
  if (p < 0 || p > rv.size())
    throw RuntimeError(EXC_RT_INDEX_RANGE_S, std::to_string(p).c_str());
  /* same set */
  if (arg_type == exp_type)
  {
    Collection& a = _args[1]->collection(ctx);
    /* allocate once and for all */
    rv.reserve(rv.size() + a.size());
    if (&a == &rv)
    {
      /* clone and move */
      Collection _a(a);
      Collection::const_iterator it = rv.begin() + p;
      for (StaticExpression * e : _a)
        it = rv.insert(it, e->swapNew());
      _a.clear();
    }
    else if (_args[1]->isRvalue())
    {
      /* move */
      Collection::const_iterator it = rv.begin() + p;
      for (StaticExpression * e : a)
        it = rv.insert(it, e->swapNew());
      a.clear();
    }
    else
    {
      /* inline clone */
      Collection::const_iterator it = rv.begin() + p;
      for (const StaticExpression * e : a)
        it = rv.insert(it, e->cloneNew());
    }
    return rv;
  }
  /* one element */
  else if (arg_type == exp_type.levelDown())
  {
    if (arg_type.level() == 0)
    {
      switch (arg_type.major())
      {
      case Type::BOOLEAN:
        rv.insert(rv.begin() + p, new BooleanExpression(_args[1]->boolean(ctx)));
        break;
      case Type::INTEGER:
        rv.insert(rv.begin() + p, new IntegerExpression(_args[1]->integer(ctx)));
        break;
      case Type::NUMERIC:
        rv.insert(rv.begin() + p, new NumericExpression(_args[1]->numeric(ctx)));
        break;
      case Type::COMPLEX:
        if (_args[1]->isRvalue())
          rv.insert(rv.begin() + p, new ComplexExpression(std::move(_args[1]->complex(ctx))));
        else
          rv.insert(rv.begin() + p, new ComplexExpression(_args[1]->complex(ctx)));
        break;
      case Type::LITERAL:
        if (_args[1]->isRvalue())
          rv.insert(rv.begin() + p, new LiteralExpression(std::move(_args[1]->literal(ctx))));
        else
          rv.insert(rv.begin() + p, new LiteralExpression(_args[1]->literal(ctx)));
        break;
      case Type::TABCHAR:
        if (_args[1]->isRvalue())
          rv.insert(rv.begin() + p, new TabcharExpression(std::move(_args[1]->tabchar(ctx))));
        else
          rv.insert(rv.begin() + p, new TabcharExpression(_args[1]->tabchar(ctx)));
        break;
      case Type::ROWTYPE:
        if (_args[1]->isRvalue())
          rv.insert(rv.begin() + p, new TupleExpression(std::move(_args[1]->tuple(ctx))));
        else
          rv.insert(rv.begin() + p, new TupleExpression(_args[1]->tuple(ctx)));
        break;
      default:
        throw RuntimeError(EXC_RT_NOT_IMPLEMENTED);
      }
    }
    else if (_args[1]->isRvalue())
      rv.insert(rv.begin() + p, new CollectionExpression(std::move(_args[1]->collection(ctx))));
    else
      rv.insert(rv.begin() + p, new CollectionExpression(_args[1]->collection(ctx)));

    return rv;
  }

  throw RuntimeError(EXC_RT_TYPE_MISMATCH_S, _exp->typeName(ctx).c_str());
}

MemberINSERTExpression * MemberINSERTExpression::parse(Parser& p, Context& ctx, Expression * exp)
{
  std::vector<Expression*> args;
  TokenPtr t = p.pop();

  if (t->code != '(')
    throw ParseError(EXC_PARSE_BAD_MEMB_CALL_S, KEYWORDS[BTM_INSERT]);
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
      case Type::NO_TYPE: /* opaque */
        break;
      default:
        throw ParseError(EXC_PARSE_MEMB_NOT_IMPL_S, KEYWORDS[BTM_INSERT]);
      }
    }

    args.push_back(ParseExpression::expression(p, ctx));
    if (!ParseExpression::typeChecking(args.back(), Type::INTEGER, p, ctx))
      throw ParseError(EXC_PARSE_MEMB_ARG_TYPE_S, KEYWORDS[BTM_INSERT]);
    t = p.pop();
    if (t->code != Parser::CHAIN)
      throw ParseError(EXC_PARSE_MEMB_ARG_NUM_S, KEYWORDS[BTM_INSERT]);

    args.push_back(ParseExpression::expression(p, ctx));
    if (exp_type.level() == 0)
    {
      switch (exp_type.major())
      {
        /* string INSERT string or one byte */
      case Type::LITERAL:
        if (args.back()->type(ctx) != Type::LITERAL &&
                !ParseExpression::typeChecking(args.back(), Type::INTEGER, p, ctx))
          throw ParseError(EXC_PARSE_MEMB_ARG_TYPE_S, KEYWORDS[BTM_INSERT]);
        break;
        /* bytes INSERT bytes or string or one byte */
      case Type::TABCHAR:
        if (args.back()->type(ctx) != Type::TABCHAR &&
                args.back()->type(ctx) != Type::LITERAL &&
                !ParseExpression::typeChecking(args.back(), Type::INTEGER, p, ctx))
          throw ParseError(EXC_PARSE_MEMB_ARG_TYPE_S, KEYWORDS[BTM_INSERT]);
        break;
      case Type::NO_TYPE: /* opaque */
        break;
      default:
        throw ParseError(EXC_PARSE_MEMB_NOT_IMPL_S, KEYWORDS[BTM_INSERT]);
      }
    }
    /* collection INSERT */
    else if (exp_type != Type::ROWTYPE ||
            /* not opaque */
            (exp_type.minor() != 0 && args.back()->type(ctx).minor() != 0))
    {
      if (args.back()->type(ctx) != exp_type &&
              args.back()->type(ctx) != exp_type.levelDown())
        throw ParseError(EXC_PARSE_TYPE_MISMATCH_S, exp->typeName(ctx).c_str());
    }
    else
    {
      /* collection INSERT opaque */
      if (args.back()->type(ctx).level() != exp_type.level() &&
              args.back()->type(ctx).level() != exp_type.levelDown().level())
        throw ParseError(EXC_PARSE_TYPE_MISMATCH_S, exp->typeName(ctx).c_str());
    }
    assertClosedMember(p, ctx, KEYWORDS[BTM_INSERT]);
    return new MemberINSERTExpression(exp, std::move(args));
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

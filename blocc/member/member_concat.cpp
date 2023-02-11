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

#include "member_concat.h"
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

std::string& MemberCONCATExpression::literal(Context& ctx) const
{
  /* literal */
  switch (_args[0]->type(ctx).major())
  {
  case Type::LITERAL:
    return _exp->literal(ctx).append(_args[0]->literal(ctx));
  case Type::INTEGER:
  {
    int64_t c = _args[0]->integer(ctx);
    if (c < 1 || c > 255)
      throw RuntimeError(EXC_RT_OUT_OF_RANGE);
    return _exp->literal(ctx).append(1, (char)c);
  }
  default:
    break;
  }

  throw RuntimeError(EXC_RT_MEMB_ARG_TYPE_S, KEYWORDS[_builtin]);
}

TabChar& MemberCONCATExpression::tabchar(Context& ctx) const
{
  /* tabchar */
  switch (_args[0]->type(ctx).major())
  {
  case Type::TABCHAR:
  {
    TabChar& rv = _exp->tabchar(ctx);
    TabChar& a = _args[0]->tabchar(ctx);
    if (&a == &rv)
    {
      TabChar _a(a);
      rv.insert(rv.end(), _a.begin(), _a.end());
    }
    else
      rv.insert(rv.end(), a.begin(), a.end());
    return rv;
  }
  case Type::LITERAL:
  {
    TabChar& rv = _exp->tabchar(ctx);
    std::string& a = _args[0]->literal(ctx);
    rv.insert(rv.end(), a.begin(), a.end());
    return rv;
  }
  case Type::INTEGER:
  {
    TabChar& rv = _exp->tabchar(ctx);
    int64_t c = _args[0]->integer(ctx);
    if (c < 0 || c > 255)
      throw RuntimeError(EXC_RT_OUT_OF_RANGE);
    rv.push_back((char)c);
    return rv;
  }
  default:
    break;
  }

  throw RuntimeError(EXC_RT_MEMB_ARG_TYPE_S, KEYWORDS[_builtin]);
}

Collection& MemberCONCATExpression::collection(Context& ctx) const
{
  const Type& arg_type = _args[0]->type(ctx);
  const Type& exp_type = _exp->type(ctx);
  Collection& rv = _exp->collection(ctx);
  /* same set */
  if (arg_type == exp_type)
  {
    Collection& a = _args[0]->collection(ctx);
    /* allocate once and for all */
    rv.reserve(rv.size() + a.size());
    if (&a == &rv)
    {
      /* clone and move */
      Collection _a(a);
      for (StaticExpression * e : _a)
        rv.push_back(e->swapNew());
      _a.clear();
    }
    else
    {
      if (_args[0]->isRvalue())
      {
        /* move */
        for (StaticExpression * e : a)
          rv.push_back(e->swapNew());
        a.clear();
      }
      else
        /* inline clone */
        for (const StaticExpression * e : a)
          rv.push_back(e->cloneNew());
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
        rv.push_back(new BooleanExpression(_args[0]->boolean(ctx)));
        break;
      case Type::INTEGER:
        rv.push_back(new IntegerExpression(_args[0]->integer(ctx)));
        break;
      case Type::NUMERIC:
        rv.push_back(new NumericExpression(_args[0]->numeric(ctx)));
        break;
      case Type::COMPLEX:
        if (_args[0]->isRvalue())
          rv.push_back(new ComplexExpression(std::move(_args[0]->complex(ctx))));
        else
          rv.push_back(new ComplexExpression(_args[0]->complex(ctx)));
        break;
      case Type::LITERAL:
        if (_args[0]->isRvalue())
          rv.push_back(new LiteralExpression(std::move(_args[0]->literal(ctx))));
        else
          rv.push_back(new LiteralExpression(_args[0]->literal(ctx)));
        break;
      case Type::TABCHAR:
        if (_args[0]->isRvalue())
          rv.push_back(new TabcharExpression(std::move(_args[0]->tabchar(ctx))));
        else
          rv.push_back(new TabcharExpression(_args[0]->tabchar(ctx)));
        break;
      case Type::ROWTYPE:
        if (_args[0]->isRvalue())
          rv.push_back(new TupleExpression(std::move(_args[0]->tuple(ctx))));
        else
          rv.push_back(new TupleExpression(_args[0]->tuple(ctx)));
        break;
      default:
        throw RuntimeError(EXC_RT_NOT_IMPLEMENTED);
      }
    }
    else if (_args[0]->isRvalue())
      rv.push_back(new CollectionExpression(std::move(_args[0]->collection(ctx))));
    else
      rv.push_back(new CollectionExpression(_args[0]->collection(ctx)));

    return rv;
  }

  throw RuntimeError(EXC_RT_TYPE_MISMATCH_S, _exp->typeName(ctx).c_str());
}

MemberCONCATExpression * MemberCONCATExpression::parse(Parser& p, Context& ctx, Expression * exp)
{
  std::vector<Expression*> args;
  TokenPtr t = p.pop();

  if (t->code != '(')
    throw ParseError(EXC_PARSE_BAD_MEMB_CALL_S, KEYWORDS[BTM_CONCAT]);
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
        throw ParseError(EXC_PARSE_MEMB_NOT_IMPL_S, KEYWORDS[BTM_CONCAT]);
      }
    }

    args.push_back(ParseExpression::expression(p, ctx));
    if (exp_type.level() == 0)
    {
      switch (exp_type.major())
      {
        /* string CONCAT string or one byte */
      case Type::LITERAL:
        if (args.back()->type(ctx) != Type::LITERAL &&
                !ParseExpression::typeChecking(args.back(), Type::INTEGER, p, ctx))
          throw ParseError(EXC_PARSE_MEMB_ARG_TYPE_S, KEYWORDS[BTM_CONCAT]);
        break;
        /* bytes CONCAT bytes or string or one byte */
      case Type::TABCHAR:
        if (args.back()->type(ctx) != Type::TABCHAR &&
                args.back()->type(ctx) != Type::LITERAL &&
                !ParseExpression::typeChecking(args.back(), Type::INTEGER, p, ctx))
          throw ParseError(EXC_PARSE_MEMB_ARG_TYPE_S, KEYWORDS[BTM_CONCAT]);
        break;
      case Type::NO_TYPE: /* opaque */
        break;
      default:
        throw ParseError(EXC_PARSE_MEMB_NOT_IMPL_S, KEYWORDS[BTM_CONCAT]);
      }
    }
    /* collection CONCAT */
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
      /* collection CONCAT opaque */
      if (args.back()->type(ctx).level() != exp_type.level() &&
              args.back()->type(ctx).level() != exp_type.levelDown().level())
        throw ParseError(EXC_PARSE_TYPE_MISMATCH_S, exp->typeName(ctx).c_str());
    }
    assertClosedMember(p, ctx, KEYWORDS[BTM_CONCAT]);
    return new MemberCONCATExpression(exp, std::move(args));
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

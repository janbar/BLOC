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
#include <blocc/collection.h>
#include <blocc/tuple.h>
#include <blocc/context.h>
#include <blocc/parser.h>
#include <blocc/debug.h>

#include <cstring>
#include <cassert>

namespace bloc
{

Value& MemberINSERTExpression::value(Context& ctx) const
{
  Value& val = _exp->value(ctx);
  Value& a0 = _args[0]->value(ctx);
  if (a0.isNull())
    throw RuntimeError(EXC_RT_INDEX_RANGE_S, a0.toString().c_str());
  Value& a1 = _args[1]->value(ctx);

  if (val.type().level() > 0)
  {
    Collection * rv = val.collection();
    const Type& rv_type = rv->table_type();
    Integer p = *a0.integer();
    if (p < 0 || p > rv->size())
      throw RuntimeError(EXC_RT_INDEX_RANGE_S, a0.toString().c_str());
    if (a1.isNull())
      return val;
    const Type& a1_type = a1.type();
    /* collection */
    if (a1_type.level() > 0)
    {
      if (val.isNull())
        return val;
      Collection * a = a1.collection();
      if (a->table_type() == rv_type)
      {
        /* allocate once and for all */
        rv->reserve(rv->size() + a->size());
        if (a == rv)
        {
          /* clone and move */
          Collection _a(*a);
          Collection::const_iterator it = rv->begin() + p;
          for (Value& e : _a)
            it = rv->insert(it, std::move(e));
          _a.clear();
        }
        else if (a1.lvalue())
        {
          /* inline clone */
          Collection::const_iterator it = rv->begin() + p;
          for (Value& e : *a)
            it = rv->insert(it, e.clone());
        }
        else
        {
          /* move */
          Collection::const_iterator it = rv->begin() + p;
          for (Value& e : *a)
            it = rv->insert(it, std::move(e));
        }
        return val;
      }
      else if (a->table_type() == rv_type.levelDown())
      {
        if (a1.lvalue())
          rv->insert(rv->begin() + p, a1.clone());
        else
          rv->insert(rv->begin() + p, std::move(a1));
        return val;
      }
      throw RuntimeError(EXC_RT_TYPE_MISMATCH_S, val.typeName().c_str());
    }
    /* tuple */
    if (a1_type == Type::ROWTYPE)
    {
      if (a1.tuple()->tuple_type() == rv_type.levelDown())
      {
        if (a1.lvalue())
          rv->insert(rv->begin() + p, a1.clone());
        else
          rv->insert(rv->begin() + p, std::move(a1));
        return val;
      }
      throw RuntimeError(EXC_RT_TYPE_MISMATCH_S, val.typeName().c_str());
    }
    /* others */
    if (a1_type == rv_type.levelDown())
    {
      if (a1.lvalue())
        rv->insert(rv->begin() + p, a1.clone());
      else
        rv->insert(rv->begin() + p, std::move(a1));
      return val;
    }
    throw RuntimeError(EXC_RT_TYPE_MISMATCH_S, val.typeName().c_str());
  }

  switch (val.type().major())
  {
    /* literal */
  case Type::LITERAL:
  {
    Literal * rv = val.literal();
    Integer p = *a0.integer();
    if (p < 0 || p > rv->size())
      throw RuntimeError(EXC_RT_INDEX_RANGE_S, a0.toString().c_str());
    if (a1.isNull())
      return val;
    switch (a1.type().major())
    {
    case Type::LITERAL:
      if (_exp->isConst())
      {
        Value v(new Literal(*rv));
        v.literal()->insert(p, *a1.literal());
        return ctx.allocate(std::move(v));
      }
      rv->insert(p, *a1.literal());
      return val;
    case Type::INTEGER:
    {
      Integer c = *a1.integer();
      if (c < 1 || c > 255)
        throw RuntimeError(EXC_RT_OUT_OF_RANGE);
      if (_exp->isConst())
      {
        Value v(new Literal(*rv));
        v.literal()->insert(p, 1, (char)c);
        return ctx.allocate(std::move(v));
      }
      rv->insert(p, 1, (char)c);
      return val;
    }
    default:
      break;
    }
  }
    /* tabchar */
  case Type::TABCHAR:
  {
    TabChar * rv = val.tabchar();
    Integer p = *a0.integer();
    if (p < 0 || p > rv->size())
      throw RuntimeError(EXC_RT_INDEX_RANGE_S, a0.toString().c_str());
    if (a1.isNull())
      return val;
    switch (a1.type().major())
    {
    case Type::TABCHAR:
    {
      TabChar * a = a1.tabchar();
      if (a == rv)
      {
        TabChar _a(*a);
        rv->insert(rv->begin() + p, _a.begin(), _a.end());
      }
      else
        rv->insert(rv->begin() + p, a->begin(), a->end());
      return val;
    }
    case Type::LITERAL:
    {
      std::string * a = a1.literal();
      rv->insert(rv->begin() + p, a->begin(), a->end());
      return val;
    }
    case Type::INTEGER:
    {
      Integer c = *a1.integer();
      if (c < 0 || c > 255)
        throw RuntimeError(EXC_RT_OUT_OF_RANGE);
      rv->insert(rv->begin() + p, (char)c);
      return val;
    }
    default:
      break;
    }
  }
  default:
    break;
  }
  throw RuntimeError(EXC_RT_MEMB_NOT_IMPL_S, KEYWORDS[BTM_INSERT]);
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
    else
    {
      const Type& arg_type = args.back()->type(ctx);
      /* type opaque or tuple opaque */
      bool exp_opaque = (exp_type == Type::NO_TYPE || (exp_type == Type::ROWTYPE && exp_type.minor() == 0));
      bool arg_opaque = (arg_type == Type::NO_TYPE || (arg_type == Type::ROWTYPE && arg_type.minor() == 0));

      /* test levels of known type or tuple opaque */
      if ((!exp_opaque || exp_type == Type::ROWTYPE) && (!arg_opaque || arg_type == Type::ROWTYPE))
      {
        if (arg_type.level() != exp_type.level() &&
              arg_type.level() != exp_type.levelDown().level())
          throw ParseError(EXC_PARSE_TYPE_MISMATCH_S, exp->typeName(ctx).c_str());

        /* test known types are compatible */
        if (!exp_opaque && !arg_opaque &&
                arg_type != exp_type && arg_type != exp_type.levelDown())
          throw ParseError(EXC_PARSE_TYPE_MISMATCH_S, exp->typeName(ctx).c_str());
      }
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

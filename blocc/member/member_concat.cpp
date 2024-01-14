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
#include <blocc/context.h>
#include <blocc/collection.h>
#include <blocc/tuple.h>
#include <blocc/parser.h>
#include <blocc/debug.h>

#include <cstring>
#include <cassert>

namespace bloc
{

Value& MemberCONCATExpression::value(Context& ctx) const
{
  Value& val = _exp->value(ctx);
  Value& a0 = _args[0]->value(ctx);

  /* collection */
  if (val.type().level() > 0)
  {
    if (val.isNull())
    {
      if (a0.type().level() > 0) /* null + tab */
        return a0;
      Collection * rv;
      if (a0.type() == Type::ROWTYPE)
      {
        if (a0.isNull()) /* null + tuple null */
          return val;
        rv = new Collection(a0.tuple()->tuple_decl(), 1);
      }
      else
        rv = new Collection(a0.type().levelUp());
      if (a0.lvalue())
        rv->push_back(a0.clone());
      else
        rv->push_back(std::move(a0));
      if (val.lvalue())
        return ctx.allocate(Value(rv));
      val.swap(Value(rv));
      return val;
    }

    Collection * rv = val.collection();
    const Type& rv_type = rv->table_type();
    const Type& a0_type = a0.type();
    /* collection */
    if (a0_type.level() > 0)
    {
      if (a0.isNull()) /* + table null */
        return val;
      Collection * a = a0.collection();
      if (a->table_type() == rv_type)
      {
        /* allocate once and for all */
        rv->reserve(rv->size() + a->size());
        if (a == rv)
        {
          /* clone and move */
          Collection _a(*a);
          for (Value& e : _a)
            rv->push_back(std::move(e));
          _a.clear();
        }
        else
        {
          if (a0.lvalue())
          {
            /* inline clone */
            for (const Value& e : *a)
              rv->push_back(e.clone());
          }
          else
          {
            /* move */
            for (Value& e : *a)
              rv->push_back(std::move(e));
          }
        }
        return val;
      }
      else if (a->table_type() == rv_type.levelDown())
      {
        if (a0.lvalue())
          rv->push_back(a0.clone());
        else
          rv->push_back(std::move(a0));
        return val;
      }
    }
    else if (a0_type == rv_type.major())
    {
      /* tuple */
      if (a0_type == Type::ROWTYPE)
      {
        if (a0.isNull()) /* + tuple null */
          return val;
        if (a0.tuple()->tuple_type() == rv_type.levelDown())
        {
          if (a0.lvalue())
            rv->push_back(a0.clone());
          else
            rv->push_back(std::move(a0));
          return val;
        }
      }
      /* others */
      else if (a0_type == rv_type.levelDown())
      {
        if (a0.lvalue())
          rv->push_back(a0.clone());
        else
          rv->push_back(std::move(a0));
        return val;
      }
    }
    else
    {
      /* type mixing */
      switch (rv_type.major())
      {
      case Type::INTEGER:
        if (a0_type == Type::NUMERIC)
        {
          rv->push_back(Value(Integer(*a0.numeric())));
          return val;
        }
        else if (a0.type() == Type::NO_TYPE)
        {
          rv->push_back(Value(Value::type_integer));
          return val;
        }
        break;
      case Type::NUMERIC:
        if (a0_type == Type::INTEGER)
        {
          rv->push_back(Value(Numeric(*a0.integer())));
          return val;
        }
        else if (a0.type() == Type::NO_TYPE)
        {
          rv->push_back(Value(Value::type_numeric));
          return val;
        }
        break;
      case Type::ROWTYPE:
        break;
      default:
        if (a0.type() == Type::NO_TYPE)
        {
          rv->push_back(Value(rv_type.levelDown()));
          return val;
        }
        break;
      }
    }
    if (val.type() == Type::ROWTYPE)
      throw RuntimeError(EXC_RT_TYPE_MISMATCH_S, val.type().levelDown().typeName(rv->table_decl().tupleName()).c_str());
    throw RuntimeError(EXC_RT_TYPE_MISMATCH_S, val.type().levelDown().typeName().c_str());
  }

  if (a0.isNull()) /* + null */
    return val;
  switch (val.type().major())
  {
    /* literal */
  case Type::NO_TYPE:
    switch (a0.type().major())
    {
    case Type::LITERAL:
      return a0;
    case Type::INTEGER:
    {
      Integer c = *a0.integer();
      if (c < 0 || c > 255)
        break;
      if (c == 0)
      {
        if (val.lvalue())
          return ctx.allocate(Value(new TabChar(1, (char)c)));
        val.swap(Value(new TabChar(1, (char)c)));
        return val;
      }
      if (val.lvalue())
        return ctx.allocate(Value(new Literal(1, (char)c)));
      val.swap(Value(new Literal(1, (char)c)));
      return val;
    }
    default:
      break;
    }
    break;
  case Type::LITERAL:
    switch (a0.type().major())
    {
      /* literal + literal */
    case Type::LITERAL:
      if (val.isNull())
        return a0;
      if (_exp->isConst())
      {
        Value v(new Literal(*val.literal()));
        v.literal()->append(*a0.literal());
        return ctx.allocate(std::move(v));
      }
      val.literal()->append(*a0.literal());
      return val;
      /* literal + char */
    case Type::INTEGER:
    {
      Integer c = *a0.integer();
      if (c < 1 || c > 255)
        throw RuntimeError(EXC_RT_OUT_OF_RANGE);
      if (val.isNull())
      {
        if (val.lvalue())
          return ctx.allocate(Value(new Literal(1, (char)c)));
        val.swap(Value(new Literal(1, (char)c)));
        return val;
      }
      if (_exp->isConst())
      {
        Value v(new Literal(*val.literal()));
        v.literal()->append(1, (char)c);
        return ctx.allocate(std::move(v));
      }
      val.literal()->append(1, (char)c);
      return val;
    }
    default:
      break;
    }

    /* tabchar */
  case Type::TABCHAR:
    switch (a0.type().major())
    {
      /* tabchar + tabchar */
    case Type::TABCHAR:
    {
      if (val.isNull())
        return a0;
      TabChar * rv = val.tabchar();
      TabChar * a = a0.tabchar();
      if (a == rv)
      {
        TabChar _a(*a);
        rv->insert(rv->end(), _a.begin(), _a.end());
      }
      else
        rv->insert(rv->end(), a->begin(), a->end());
      return val;
    }
      /* tabchar + literal */
    case Type::LITERAL:
    {
      Literal * a = a0.literal();
      if (val.isNull())
      {
        TabChar * rv = new TabChar();
        rv->insert(rv->end(), a->begin(), a->end());
        if (val.lvalue())
          return ctx.allocate(Value(rv));
        val.swap(Value(rv));
        return val;
      }
      TabChar * rv = val.tabchar();
      rv->insert(rv->end(), a->begin(), a->end());
      return val;
    }
      /* tabchar + char */
    case Type::INTEGER:
    {
      Integer c = *a0.integer();
      if (c < 0 || c > 255)
        throw RuntimeError(EXC_RT_OUT_OF_RANGE);
      if (val.isNull())
      {
        if (val.lvalue())
          return ctx.allocate(Value(new TabChar(1, (char)c)));
        val.swap(Value(new TabChar(1, (char)c)));
        return val;
      }
      TabChar * rv = val.tabchar();
      rv->push_back((char)c);
      return val;
    }
    default:
      break;
    }
  default:
    break;
  }
  throw RuntimeError(EXC_RT_MEMB_ARG_TYPE_S, KEYWORDS[_builtin]);
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

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

#include "builtin_mod.h"
#include <blocc/parse_expression.h>
#include <blocc/exception_parse.h>
#include <blocc/context.h>
#include <blocc/parser.h>
#include <blocc/debug.h>

#include <cmath>

namespace bloc
{

const Type& MODExpression::type(Context &ctx) const
{
  const Type& t0 = _args[0]->type(ctx);
  const Type& t1 = _args[1]->type(ctx);
  if (t0 == Type::INTEGER && t1 == Type::INTEGER)
    return Value::type_integer;
  return Value::type_numeric;
}

Value& MODExpression::value(Context & ctx) const
{
  Value& a0 = _args[0]->value(ctx);
  Value& a1 = _args[1]->value(ctx);
  Value v(Value::type_numeric);

  switch (a0.type().major())
  {
  case Type::NO_TYPE:
    break;
  case Type::INTEGER:
    switch (a1.type().major())
    {
    case Type::NO_TYPE:
      v = Value(Value::type_integer);
      break;
    case Type::INTEGER:
      if (*a1.integer() == 0)
        throw RuntimeError(EXC_RT_DIVIDE_BY_ZERO);
      v = Value(Integer(*a0.integer() % *a1.integer()));
      break;
    case Type::NUMERIC:
      if (*a1.numeric() == 0.0)
        throw RuntimeError(EXC_RT_DIVIDE_BY_ZERO);
      v = Value(Numeric(std::fmod((double)*a0.integer(), *a1.numeric())));
      break;
    default:
      throw RuntimeError(EXC_RT_FUNC_ARG_TYPE_S, KEYWORDS[oper]);
    }
    break;
  case Type::NUMERIC:
    switch (a1.type().major())
    {
    case Type::NO_TYPE:
      v = Value(Value::type_numeric);
      break;
    case Type::INTEGER:
      if (*a1.integer() == 0)
        throw RuntimeError(EXC_RT_DIVIDE_BY_ZERO);
      v = Value(Numeric(std::fmod(*a0.numeric(), (double)*a1.integer())));
      break;
    case Type::NUMERIC:
      if (*a1.numeric() == 0.0)
        throw RuntimeError(EXC_RT_DIVIDE_BY_ZERO);
      v = Value(Numeric(std::fmod(*a0.numeric(), *a1.numeric())));
      break;
    default:
      throw RuntimeError(EXC_RT_FUNC_ARG_TYPE_S, KEYWORDS[oper]);
    }
    break;
  default:
    throw RuntimeError(EXC_RT_FUNC_ARG_TYPE_S, KEYWORDS[oper]);
  }
  if (!a0.lvalue())
  {
    a0.swap(Value(std::move(v)));
    return a0;
  }
  if (!a1.lvalue())
  {
    a1.swap(Value(std::move(v)));
    return a1;
  }
  return ctx.allocate(std::move(v));
}

MODExpression * MODExpression::parse(Parser& p, Context& ctx)
{
  std::vector<Expression*> args;

  try
  {
    TokenPtr t = p.pop();
    if (t->code != '(')
      throw ParseError(EXC_PARSE_FUNC_ARG_NUM_S, KEYWORDS[FUNC_MOD]);
    args.push_back(ParseExpression::expression(p, ctx));
    if (!ParseExpression::typeChecking(args.back(), Type::NUMERIC, p, ctx))
      throw ParseError(EXC_PARSE_FUNC_ARG_TYPE_S, KEYWORDS[FUNC_MOD]);
    t = p.pop();
    if (t->code != Parser::Chain)
      throw ParseError(EXC_PARSE_FUNC_ARG_NUM_S, KEYWORDS[FUNC_MOD]);
    args.push_back(ParseExpression::expression(p, ctx));
    if (!ParseExpression::typeChecking(args.back(), Type::NUMERIC, p, ctx))
      throw ParseError(EXC_PARSE_FUNC_ARG_TYPE_S, KEYWORDS[FUNC_MOD]);
    assertClosedFunction(p, ctx, FUNC_MOD);
    return new MODExpression(std::move(args));
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

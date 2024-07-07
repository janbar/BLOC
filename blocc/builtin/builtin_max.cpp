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

#include "builtin_max.h"
#include <blocc/parse_expression.h>
#include <blocc/exception_parse.h>
#include <blocc/context.h>
#include <blocc/parser.h>
#include <blocc/debug.h>

#include <cmath>

namespace bloc
{

const Type& MAXExpression::type (Context &ctx) const
{
  if (_args[0]->type(ctx) == Type::INTEGER && _args[1]->type(ctx) == Type::INTEGER)
    return Value::type_integer;
  return Value::type_numeric;
}

Value& MAXExpression::value(Context & ctx) const
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
      v = Value(Integer(std::max<int64_t>(*a0.integer(), *a1.integer())));
      break;
    case Type::NUMERIC:
      v = Value(Numeric(std::max<double>((double)*a0.integer(), *a1.numeric())));
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
      v = Value(Numeric(std::max<double>(*a0.numeric(), (double)*a1.integer())));
      break;
    case Type::NUMERIC:
      v = Value(Numeric(std::max<double>(*a0.numeric(), *a1.numeric())));
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

MAXExpression * MAXExpression::parse(Parser& p, Context& ctx)
{
  std::vector<Expression*> args;

  try
  {
    TokenPtr t = p.pop();
    if (t->code != '(')
      throw ParseError(EXC_PARSE_FUNC_ARG_NUM_S, KEYWORDS[FUNC_MAX]);
    args.push_back(ParseExpression::expression(p, ctx));
    if (!ParseExpression::typeChecking(args.back(), Type::NUMERIC, p, ctx))
      throw ParseError(EXC_PARSE_FUNC_ARG_TYPE_S, KEYWORDS[FUNC_MAX]);
    t = p.pop();
    if (t->code != Parser::Chain)
      throw ParseError(EXC_PARSE_FUNC_ARG_NUM_S, KEYWORDS[FUNC_MAX]);
    args.push_back(ParseExpression::expression(p, ctx));
    if (!ParseExpression::typeChecking(args.back(), Type::NUMERIC, p, ctx))
      throw ParseError(EXC_PARSE_FUNC_ARG_TYPE_S, KEYWORDS[FUNC_MAX]);
    assertClosedFunction(p, ctx, FUNC_MAX);
    return new MAXExpression(std::move(args));
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

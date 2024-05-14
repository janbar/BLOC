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

#include "builtin_pow.h"
#include <blocc/parse_expression.h>
#include <blocc/exception_parse.h>
#include <blocc/context.h>
#include <blocc/parser.h>
#include <blocc/debug.h>

#include <cmath>

namespace bloc
{

const Type& POWExpression::type (Context &ctx) const
{
  if (_args[0]->type(ctx) == Type::IMAGINARY || _args[1]->type(ctx) == Type::IMAGINARY)
    return Value::type_imaginary;
  if (_args[0]->type(ctx) == Type::INTEGER && _args[1]->type(ctx) == Type::INTEGER)
    return Value::type_integer;
  return Value::type_numeric;
}

#define LVAL2(V,A,B) (\
 !A.lvalue() ? (A = std::move(V)) : \
 !B.lvalue() ? (B = std::move(V)) : ctx.allocate(std::move(V)) \
)

Value& POWExpression::value(Context & ctx) const
{
  /* see operator OP_EXP */
  Value& a1 = _args[0]->value(ctx);
  Value& a2 = _args[1]->value(ctx);

  switch (a1.type().major())
  {
  case Type::NO_TYPE:
    switch (a2.type().major())
    {
    case Type::NO_TYPE:
      return LVAL2(Value(Value::type_numeric), a1, a2);
    case Type::INTEGER:
    case Type::NUMERIC:
      return LVAL2(Value(a2.type()), a1, a2);
    default:
      break;
    }
    break;
  case Type::INTEGER:
    switch (a2.type().major())
    {
    case Type::NO_TYPE:
      return LVAL2(Value(Value::type_integer), a1, a2);
    case Type::NUMERIC:
    {
      if (a2.isNull() || a1.isNull())
        return LVAL2(Value(Value::type_numeric), a1, a2);
      Value val(Numeric(std::pow(*a1.integer(), *a2.numeric())));
      return LVAL2(val, a1, a2);
    }
    case Type::INTEGER:
    {
      if (a2.isNull() || a1.isNull())
        return LVAL2(Value(Value::type_integer), a1, a2);
      Value val(Integer(std::pow(*a1.integer(), *a2.integer())));
      return LVAL2(val, a1, a2);
    }
    case Type::IMAGINARY:
      throw RuntimeError(EXC_RT_NOT_IMPLEMENTED, KEYWORDS[oper]);
    default:
      break;
    }
    break;
  case Type::NUMERIC:
    switch (a2.type().major())
    {
    case Type::NO_TYPE:
      return LVAL2(Value(Value::type_numeric), a1, a2);
    case Type::INTEGER:
    {
      if (a2.isNull() || a1.isNull())
        return LVAL2(Value(Value::type_numeric), a1, a2);
      Value val(Numeric(std::pow(*a1.numeric(), *a2.integer())));
      return LVAL2(val, a1, a2);
    }
    case Type::NUMERIC:
    {
      if (a2.isNull() || a1.isNull())
        return LVAL2(Value(Value::type_numeric), a1, a2);
      Value val(Numeric(std::pow(*a1.numeric(), *a2.numeric())));
      return LVAL2(val, a1, a2);
    }
    case Type::IMAGINARY:
      throw RuntimeError(EXC_RT_NOT_IMPLEMENTED, KEYWORDS[oper]);
    default:
      break;
    }
    break;
  case Type::IMAGINARY:
    switch (a2.type().major())
    {
    case Type::NO_TYPE:
      return LVAL2(Value(Value::type_imaginary), a1, a2);
    case Type::INTEGER:
    {
      if (a2.isNull() || a1.isNull())
        return LVAL2(Value(Value::type_imaginary), a1, a2);
      double rp = std::pow(std::sqrt(std::pow(a1.imaginary()->a, 2) + std::pow(a1.imaginary()->b, 2)), Numeric(*a2.integer()));
      double tp = Numeric(*a2.integer()) * std::atan2(a1.imaginary()->b, a1.imaginary()->a);
      Value val(new Imaginary{rp * std::cos(tp), rp * std::sin(tp)});
      return LVAL2(val, a1, a2);
    }
    case Type::NUMERIC:
    {
      if (a2.isNull() || a1.isNull())
        return LVAL2(Value(Value::type_imaginary), a1, a2);
      double rp = std::pow(std::sqrt(std::pow(a1.imaginary()->a, 2) + std::pow(a1.imaginary()->b, 2)), *a2.numeric());
      double tp = *a2.numeric() * std::atan2(a1.imaginary()->b, a1.imaginary()->a);
      Value val(new Imaginary{rp * std::cos(tp), rp * std::sin(tp)});
      return LVAL2(val, a1, a2);
    }
    case Type::IMAGINARY:
      throw RuntimeError(EXC_RT_NOT_IMPLEMENTED, KEYWORDS[oper]);
    default:
      break;
    }
    break;
  default:
    break;
  }
  throw RuntimeError(EXC_RT_FUNC_ARG_TYPE_S, KEYWORDS[oper]);
}

POWExpression * POWExpression::parse(Parser& p, Context& ctx)
{
  std::vector<Expression*> args;

  try
  {
    TokenPtr t = p.pop();
    if (t->code != '(')
      throw ParseError(EXC_PARSE_FUNC_ARG_NUM_S, KEYWORDS[FUNC_POW]);
    args.push_back(ParseExpression::expression(p, ctx));
    if (!ParseExpression::typeChecking(args.back(), Type::NUMERIC, p, ctx))
      throw ParseError(EXC_PARSE_FUNC_ARG_TYPE_S, KEYWORDS[FUNC_POW]);
    t = p.pop();
    if (t->code != Parser::CHAIN)
      throw ParseError(EXC_PARSE_FUNC_ARG_NUM_S, KEYWORDS[FUNC_POW]);
    args.push_back(ParseExpression::expression(p, ctx));
    if (!ParseExpression::typeChecking(args.back(), Type::NUMERIC, p, ctx))
      throw ParseError(EXC_PARSE_FUNC_ARG_TYPE_S, KEYWORDS[FUNC_POW]);
    assertClosedFunction(p, ctx, FUNC_POW);
    return new POWExpression(std::move(args));
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

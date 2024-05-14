/*
 *      Copyright (C) 2023 Jean-Luc Barriere
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

#include "op_exp.h"
#include <blocc/exception_parse.h>
#include <blocc/complex.h>
#include <blocc/collection.h>
#include <blocc/tuple.h>
#include <blocc/context.h>
#include <blocc/debug.h>

#include <cmath>
#include <climits>

namespace bloc
{

OpEXPExpression::~OpEXPExpression()
{
  if (arg2)
    delete arg2;
  if (arg1)
    delete arg1;
}

const Type& OpEXPExpression::type(Context& ctx) const
{
  if (arg1->type(ctx) == Type::IMAGINARY || arg2->type(ctx) == Type::IMAGINARY)
    return Value::type_imaginary;
  if (arg1->type(ctx) == Type::INTEGER && arg2->type(ctx) == Type::INTEGER)
    return Value::type_integer;
  return Value::type_numeric;
}

#define LVAL1(V,A) (\
 !A.lvalue() ? (A = std::move(V)) : ctx.allocate(std::move(V)) \
)

#define LVAL2(V,A,B) (\
 !A.lvalue() ? (A = std::move(V)) : \
 !B.lvalue() ? (B = std::move(V)) : ctx.allocate(std::move(V)) \
)

Value& OpEXPExpression::value(Context& ctx) const
{
  Value& a1 = arg1->value(ctx);
  Value& a2 = arg2->value(ctx);

  if (a1.type().level() == 0 && a2.type().level() == 0)
  {
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
        throw RuntimeError(EXC_RT_NOT_IMPLEMENTED, Operator::OPVALS[Operator::OP_EXP]);
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
        throw RuntimeError(EXC_RT_NOT_IMPLEMENTED, Operator::OPVALS[Operator::OP_EXP]);
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
        throw RuntimeError(EXC_RT_NOT_IMPLEMENTED, Operator::OPVALS[Operator::OP_EXP]);
      default:
        break;
      }
      break;
    default:
      break;
    }
  }
  throw RuntimeError(EXC_RT_INV_EXPRESSION);
}

std::string OpEXPExpression::unparse(Context&ctx) const
{
  std::string str;
  if (arg2 == nullptr)
    str.append(Operator::OPVALS[Operator::OP_EXP]).append(arg1->unparse(ctx));
  else
    str.append("(").append(arg1->unparse(ctx)).append(" ")
            .append(Operator::OPVALS[Operator::OP_EXP]).append(" ")
            .append(arg2->unparse(ctx)).append(")");
  return str;
}

}

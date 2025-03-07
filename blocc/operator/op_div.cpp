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

#include "op_div.h"
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

OpDIVExpression::~OpDIVExpression()
{
  if (arg2)
    delete arg2;
  if (arg1)
    delete arg1;
}

const Type& OpDIVExpression::type(Context& ctx) const
{
  const Type& t1 = arg1->type(ctx);
  const Type& t2 = arg2->type(ctx);
  if (t1 == Type::IMAGINARY || t2 == Type::IMAGINARY)
    return Value::type_imaginary;
  if (t1 == Type::INTEGER && t2 == Type::INTEGER)
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

Value& OpDIVExpression::value(Context& ctx) const
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
      case Type::IMAGINARY:
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
        Numeric d = *a2.numeric();
        if (d == 0.0)
          throw RuntimeError(EXC_RT_DIVIDE_BY_ZERO);
        Value val(Numeric(*a1.integer() / d));
        return LVAL2(val, a1, a2);
      }
      case Type::INTEGER:
      {
        if (a2.isNull() || a1.isNull())
          return LVAL2(Value(Value::type_integer), a1, a2);
        Integer l = *a2.integer();
        if (l == 0)
          throw RuntimeError(EXC_RT_DIVIDE_BY_ZERO);
        Value val(Integer(*a1.integer() / l));
        return LVAL2(val, a1, a2);
      }
      case Type::IMAGINARY:
      {
        if (a2.isNull() || a1.isNull())
          return LVAL2(Value(Value::type_imaginary), a1, a2);

        double sc = std::pow(a2.imaginary()->a, 2) + std::pow(a2.imaginary()->b, 2);
        double a = a2.imaginary()->a / sc;
        double b = (-a2.imaginary()->b) / sc;
        Value val(new Imaginary{Numeric(*a1.integer()) * a, Numeric(*a1.integer()) * b});

        return LVAL2(val, a1, a2);
      }
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
        Integer l = *a2.integer();
        if (l == 0)
          throw RuntimeError(EXC_RT_DIVIDE_BY_ZERO);
        Value val(Numeric(*a1.numeric() / l));
        return LVAL2(val, a1, a2);
      }
      case Type::NUMERIC:
      {
        if (a2.isNull() || a1.isNull())
          return LVAL2(Value(Value::type_numeric), a1, a2);
        Numeric d = *a2.numeric();
        if (d == 0.0)
          throw RuntimeError(EXC_RT_DIVIDE_BY_ZERO);
        Value val(Numeric(*a1.numeric() / d));
        return LVAL2(val, a1, a2);
      }
      case Type::IMAGINARY:
      {
        if (a2.isNull() || a1.isNull())
          return LVAL2(Value(Value::type_imaginary), a1, a2);

        double sc = std::pow(a2.imaginary()->a, 2) + std::pow(a2.imaginary()->b, 2);
        double a = a2.imaginary()->a / sc;
        double b = (-a2.imaginary()->b) / sc;
        Value val(new Imaginary{*a1.numeric() * a, *a1.numeric() * b});

        return LVAL2(val, a1, a2);
      }
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
        Value val(new Imaginary{a1.imaginary()->a / Numeric(*a2.integer()), a1.imaginary()->b / Numeric(*a2.integer())});
        return LVAL2(val, a1, a2);
      }
      case Type::NUMERIC:
      {
        if (a2.isNull() || a1.isNull())
          return LVAL2(Value(Value::type_imaginary), a1, a2);
        Value val(new Imaginary{a1.imaginary()->a / *a2.numeric(), a1.imaginary()->b / *a2.numeric()});
        return LVAL2(val, a1, a2);
      }
      case Type::IMAGINARY:
      {
        if (a2.isNull() || a1.isNull())
          return LVAL2(Value(Value::type_imaginary), a1, a2);

        double sc = std::pow(a2.imaginary()->a, 2) + std::pow(a2.imaginary()->b, 2);
        double a = a2.imaginary()->a / sc;
        double b = (-a2.imaginary()->b) / sc;
        Value val(new Imaginary{
                  a1.imaginary()->a * a - a1.imaginary()->b * b,
                  a1.imaginary()->a * b + a1.imaginary()->b * a
        });
        return LVAL2(val, a1, a2);
      }
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

std::string OpDIVExpression::unparse(Context&ctx) const
{
  std::string str;
  if (arg2 == nullptr)
    str.append(Operator::OPVALS[Operator::OP_DIV]).append(arg1->unparse(ctx));
  else
    str.append("(").append(arg1->unparse(ctx)).append(" ")
            .append(Operator::OPVALS[Operator::OP_DIV]).append(" ")
            .append(arg2->unparse(ctx)).append(")");
  return str;
}

}

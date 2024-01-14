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

#include "op_sub.h"
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

OpSUBExpression::~OpSUBExpression()
{
  if (arg2)
    delete arg2;
  if (arg1)
    delete arg1;
}

const Type& OpSUBExpression::type(Context& ctx) const
{
  if (arg1->type(ctx) == Type::INTEGER && arg2->type(ctx) == Type::INTEGER)
    return Value::type_integer;
  return Value::type_numeric;
}

#define LVAL1(V,A) (\
 !A.lvalue() ? (A = std::move(V)) : ctx.allocate(V.clone()) \
)

#define LVAL2(V,A,B) (\
 !A.lvalue() ? (A = std::move(V)) : \
 !B.lvalue() ? (B = std::move(V)) : ctx.allocate(V.clone()) \
)

Value& OpSUBExpression::value(Context& ctx) const
{
  Value& a1 = arg1->value(ctx);
  if (a1.isNull())
    return a1;
  Value& a2 = arg2->value(ctx);
  if (a2.isNull())
    return a2;

  switch (a1.type().major())
  {
  case Type::INTEGER:
  {
    if (a2.type() == Type::NUMERIC)
    {
      Value val(Numeric(*a1.integer() - *a2.numeric()));
      return LVAL2(val, a1, a2);
    }
    Value val(Integer(*a1.integer() - *a2.integer()));
    return LVAL2(val, a1, a2);
  }
  case Type::NUMERIC:
  {
    if (a2.type() == Type::INTEGER)
    {
      Value val(Numeric(*a1.numeric() - *a2.integer()));
      return LVAL2(val, a1, a2);
    }
    Value val(Numeric(*a1.numeric() - *a2.numeric()));
    return LVAL2(val, a1, a2);
  }
  default:
    throw RuntimeError(EXC_RT_INV_EXPRESSION);
  }
}

std::string OpSUBExpression::unparse(Context&ctx) const
{
  std::string str;
  if (arg2 == nullptr)
    str.append(Operator::OPVALS[Operator::OP_SUB]).append(arg1->unparse(ctx));
  else
    str.append("(").append(arg1->unparse(ctx)).append(" ")
            .append(Operator::OPVALS[Operator::OP_SUB]).append(" ")
            .append(arg2->unparse(ctx)).append(")");
  return str;
}

}
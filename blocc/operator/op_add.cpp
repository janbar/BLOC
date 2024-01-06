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

#include "op_add.h"
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

OpADDExpression::~OpADDExpression()
{
  if (arg2)
    delete arg2;
  if (arg1)
    delete arg1;
}

const Type& OpADDExpression::type(Context& ctx) const
{
  if (arg1->type(ctx) == Type::LITERAL)
    return Value::type_literal;
  if (arg1->type(ctx) == Type::INTEGER && arg2->type(ctx) == Type::INTEGER)
    return Value::type_integer;
  if (arg1->type(ctx) == Type::NUMERIC || arg2->type(ctx) == Type::NUMERIC)
    return Value::type_numeric;
  return Value::type_no_type;
}

#define LVAL1(V,A) (\
 !A.lvalue() ? (A = std::move(V)) : ctx.allocate(V.clone()) \
)

#define LVAL2(V,A,B) (\
 !A.lvalue() ? (A = std::move(V)) : \
 !B.lvalue() ? (B = std::move(V)) : ctx.allocate(V.clone()) \
)

Value& OpADDExpression::value(Context& ctx) const
{
  Value& a1 = arg1->value(ctx);

  switch (a1.type().major())
  {
  case Type::NO_TYPE:
  {
    Value& a2 = arg2->value(ctx);
    /* concat literal */
    if (a2.type() == Type::LITERAL)
      return a2;
    return a1;
  }
  case Type::INTEGER:
  {
    if (a1.isNull())
      return a1;
    Value& a2 = arg2->value(ctx);
    if (a2.isNull())
      return a2;
    if (a2.type() == Type::NUMERIC)
    {
      Value val(Numeric(*a1.integer() + *a2.numeric()));
      return LVAL2(val, a1, a2);
    }
    Value val(Integer(*a1.integer() + *a2.integer()));
    return LVAL2(val, a1, a2);
  }
  case Type::NUMERIC:
  {
    if (a1.isNull())
      return a1;
    Value& a2 = arg2->value(ctx);
    if (a2.isNull())
      return a2;
    if (a2.type() == Type::INTEGER)
    {
      Value val(Numeric(*a1.numeric() + *a2.integer()));
      return LVAL2(val, a1, a2);
    }
    Value val(Numeric(*a1.numeric() + *a2.numeric()));
    return LVAL2(val, a1, a2);
  }
  case Type::LITERAL:
  {
    Value& a2 = arg2->value(ctx);
    if (a1.isNull())
    {
      if (a2.type() == Type::LITERAL)
        return a2;
      break; /* throw */
    }
    if (a1.lvalue())
    {
      Value& val = ctx.allocate(a1.clone());
      if (!a2.isNull())
        val.literal()->append(*a2.literal());
      return val;
    }
    if (!a2.isNull())
      a1.literal()->append(*a2.literal());
    return a1;
  }
  default:
    break;
  }
  throw RuntimeError(EXC_RT_INV_EXPRESSION);
}

std::string OpADDExpression::unparse(Context&ctx) const
{
  std::string str;
  if (arg2 == nullptr)
    str.append(Operator::OPVALS[Operator::OP_ADD]).append(arg1->unparse(ctx));
  else
    str.append("(").append(arg1->unparse(ctx)).append(" ")
            .append(Operator::OPVALS[Operator::OP_ADD]).append(" ")
            .append(arg2->unparse(ctx)).append(")");
  return str;
}

}

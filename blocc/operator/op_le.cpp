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

#include "op_le.h"
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

OpLEExpression::~OpLEExpression()
{
  if (arg2)
    delete arg2;
  if (arg1)
    delete arg1;
}

#define LVAL1(V,A) (\
 !A.lvalue() ? (A = std::move(V)) : ctx.allocate(std::move(V)) \
)

#define LVAL2(V,A,B) (\
 !A.lvalue() ? (A = std::move(V)) : \
 !B.lvalue() ? (B = std::move(V)) : ctx.allocate(std::move(V)) \
)

Value& OpLEExpression::value(Context& ctx) const
{
  Value& a1 = arg1->value(ctx);
  Value& a2 = arg2->value(ctx);
  if (a1.isNull() || a2.isNull())
    return LVAL2(Value(Value::type_boolean), a1, a2);

  switch (a1.type().major())
  {
  case Type::INTEGER:
  {
    if (a2.type() == Type::NUMERIC)
    {
      Value val(Bool(*a1.integer() <= *a2.numeric()));
      return LVAL2(val, a1, a2);
    }
    Value val(Bool(*a1.integer() <= *a2.integer()));
    return LVAL2(val, a1, a2);
  }
  case Type::NUMERIC:
  {
    if (a2.type() == Type::INTEGER)
    {
      Value val(Bool(*a1.numeric() <= *a2.integer()));
      return LVAL2(val, a1, a2);
    }
    Value val(Bool(*a1.numeric() <= *a2.numeric()));
    return LVAL2(val, a1, a2);
  }
  case Type::LITERAL:
  {
    Value val(Bool(a1.literal()->compare(*a2.literal()) <= 0));
    return LVAL2(val, a1, a2);
  }
  default:
    break;
  }
  Value val(Bool(false));
  return LVAL2(val, a1, a2);
}

std::string OpLEExpression::unparse(Context&ctx) const
{
  std::string str;
  if (enc)
    str.push_back('(');
  if (arg2 == nullptr)
    str.append(Operator::OPVALS[Operator::OP_LE]).append(arg1->unparse(ctx));
  else
    str.append(arg1->unparse(ctx)).append(" ")
            .append(Operator::OPVALS[Operator::OP_LE]).append(" ")
            .append(arg2->unparse(ctx));
  if (enc)
    str.push_back(')');
  return str;
}

}

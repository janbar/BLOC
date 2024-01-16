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

#include "op_bxor.h"
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

OpBXORExpression::~OpBXORExpression()
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

Value& OpBXORExpression::value(Context& ctx) const
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
      case Type::BOOLEAN:
        return LVAL2(Value(Value::type_boolean), a1, a2);     /* null */
      default:
        break;
      }
      break;
    case Type::BOOLEAN:
      switch (a2.type().major())
      {
      case Type::NO_TYPE:
        return LVAL2(Value(Value::type_boolean), a1, a2);     /* null */
      case Type::BOOLEAN:
      {
        if (a2.isNull())
          return a2;
        if (a1.isNull())
          return a1;
        return LVAL2(Value(Bool(*a1.boolean() ^ *a2.boolean())), a1, a2);
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

std::string OpBXORExpression::unparse(Context&ctx) const
{
  std::string str;
  if (arg2 == nullptr)
    str.append(Operator::OPVALS[Operator::OP_BXOR]).append(arg1->unparse(ctx));
  else
    str.append("(").append(arg1->unparse(ctx)).append(" ")
            .append(Operator::OPVALS[Operator::OP_BXOR]).append(" ")
            .append(arg2->unparse(ctx)).append(")");
  return str;
}

}

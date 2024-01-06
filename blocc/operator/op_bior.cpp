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

#include "op_bior.h"
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

OpBIORExpression::~OpBIORExpression()
{
  if (arg2)
    delete arg2;
  if (arg1)
    delete arg1;
}

const Type& OpBIORExpression::type(Context& ctx) const
{
  return Value::type_boolean;
}

#define LVAL1(V,A) (\
 !A.lvalue() ? (A = std::move(V)) : ctx.allocate(V.clone()) \
)

#define LVAL2(V,A,B) (\
 !A.lvalue() ? (A = std::move(V)) : \
 !B.lvalue() ? (B = std::move(V)) : ctx.allocate(V.clone()) \
)

Value& OpBIORExpression::value(Context& ctx) const
{
  Value& a1 = arg1->value(ctx);
  if (!a1.isNull() && *a1.boolean())
  {
    Value val(Bool(true));
    return LVAL1(val, a1);
  }
  Value& a2 = arg2->value(ctx);
  if (!a2.isNull() && *a2.boolean())
  {
    Value val(Bool(true));
    return LVAL2(val, a1, a2);
  }
  Value val(Bool(false));
  return LVAL2(val, a1, a2);
}

std::string OpBIORExpression::unparse(Context&ctx) const
{
  std::string str;
  if (arg2 == nullptr)
    str.append(Operator::OPVALS[Operator::OP_BIOR]).append(arg1->unparse(ctx));
  else
    str.append("(").append(arg1->unparse(ctx)).append(" ")
            .append(Operator::OPVALS[Operator::OP_BIOR]).append(" ")
            .append(arg2->unparse(ctx)).append(")");
  return str;
}

}

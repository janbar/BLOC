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

#include "op_bnot.h"
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

OpBNOTExpression::~OpBNOTExpression()
{
  if (arg1)
    delete arg1;
}

const Type& OpBNOTExpression::type(Context& ctx) const
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

Value& OpBNOTExpression::value(Context& ctx) const
{
  Value& a1 = arg1->value(ctx);
  Value val(Bool(a1.isNull() || *a1.boolean() == false));
  return LVAL1(val, a1);
}

std::string OpBNOTExpression::unparse(Context&ctx) const
{
  std::string str;
  str.append(Operator::OPVALS[Operator::OP_BNOT]).append(arg1->unparse(ctx));
  return str;
}

}

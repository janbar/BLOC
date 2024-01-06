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

#include "op_pos.h"
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

OpPOSExpression::~OpPOSExpression()
{
  if (arg1)
    delete arg1;
}

const Type& OpPOSExpression::type(Context& ctx) const
{
  return arg1->type(ctx);
}

#define LVAL1(V,A) (\
 !A.lvalue() ? (A = std::move(V)) : ctx.allocate(V.clone()) \
)

#define LVAL2(V,A,B) (\
 !A.lvalue() ? (A = std::move(V)) : \
 !B.lvalue() ? (B = std::move(V)) : ctx.allocate(V.clone()) \
)

Value& OpPOSExpression::value(Context& ctx) const
{
  Value& a1 = arg1->value(ctx);
  if (a1.isNull())
    return a1;

  if (a1.type() == Type::INTEGER)
  {
    Value val(Integer(*a1.integer()));
    return LVAL1(val, a1);
  }
  Value val(Numeric(*a1.numeric()));
  return LVAL1(val, a1);
}

std::string OpPOSExpression::unparse(Context&ctx) const
{
  std::string str;
  str.append(Operator::OPVALS[Operator::OP_POS]).append(arg1->unparse(ctx));
  return str;
}

}

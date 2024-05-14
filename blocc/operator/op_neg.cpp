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

#include "op_neg.h"
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

OpNEGExpression::~OpNEGExpression()
{
  if (arg1)
    delete arg1;
}

const Type& OpNEGExpression::type(Context& ctx) const
{
  return arg1->type(ctx);
}

#define LVAL1(V,A) (\
 !A.lvalue() ? (A = std::move(V)) : ctx.allocate(std::move(V)) \
)

#define LVAL2(V,A,B) (\
 !A.lvalue() ? (A = std::move(V)) : \
 !B.lvalue() ? (B = std::move(V)) : ctx.allocate(std::move(V)) \
)

Value& OpNEGExpression::value(Context& ctx) const
{
  Value& a1 = arg1->value(ctx);

  if (a1.type().level() == 0)
  {
    switch (a1.type().major())
    {
    case Type::NO_TYPE:
      return a1;
    case Type::INTEGER:
      if (a1.isNull())
        return a1;
      return LVAL1(Value(Integer(0 - *a1.integer())), a1);
    case Type::NUMERIC:
      if (a1.isNull())
        return a1;
      return LVAL1(Value(Numeric(0.0 - *a1.numeric())), a1);
    case Type::IMAGINARY:
      if (a1.isNull())
        return a1;
      return LVAL1(Value(new Imaginary{0.0 - a1.imaginary()->a, 0.0 - a1.imaginary()->b }), a1);
    default:
      break;
    }
  }
  throw RuntimeError(EXC_RT_INV_EXPRESSION);
}

std::string OpNEGExpression::unparse(Context&ctx) const
{
  std::string str;
  str.append(Operator::OPVALS[Operator::OP_NEG]).append(arg1->unparse(ctx));
  return str;
}

}

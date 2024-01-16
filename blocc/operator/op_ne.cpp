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

#include "op_ne.h"
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

OpNEExpression::~OpNEExpression()
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

Value& OpNEExpression::value(Context& ctx) const
{
  Value& a1 = arg1->value(ctx);
  if (a1.isNull())
    return LVAL1(Value(Bool(false)), a1);
  Value& a2 = arg2->value(ctx);
  if (a2.isNull())
    return LVAL2(Value(Bool(false)), a1, a2);

  const Type& t1 = a1.type();
  const Type& t2 = a2.type();
  if (t1.level() > 0)
  {
    if (t2.level() > 0)
    {
      Value val(Bool(a1.collection() != a2.collection()));
      return LVAL2(val, a1, a2);
    }
    Value val(Bool(true));
    return LVAL2(val, a1, a2);
  }
  if (t2.level() > 0)
  {
    Value val(Bool(true));
    return LVAL2(val, a1, a2);
  }
  switch (t1.major())
  {
  case Type::BOOLEAN:
  {
    if (t2 == Type::BOOLEAN)
    {
      Value val(Bool(*a1.boolean() != *a2.boolean()));
      return LVAL2(val, a1, a2);
    }
    Value val(Bool(true));
    return LVAL2(val, a1, a2);
  }
  case Type::INTEGER:
  {
    if (t2 == Type::NUMERIC)
    {
      Value val(Bool(*a1.integer() != *a2.numeric()));
      return LVAL2(val, a1, a2);
    }
    else if (t2 == Type::INTEGER)
    {
      Value val(Bool(*a1.integer() != *a2.integer()));
      return LVAL2(val, a1, a2);
    }
    Value val(Bool(true));
    return LVAL2(val, a1, a2);
  }
  case Type::NUMERIC:
  {
    if (t2 == Type::NUMERIC)
    {
      Value val(Bool(*a1.numeric() != *a2.numeric()));
      return LVAL2(val, a1, a2);
    }
    if (t2 == Type::INTEGER)
    {
      Value val(Bool(*a1.numeric() != *a2.integer()));
      return LVAL2(val, a1, a2);
    }
    Value val(Bool(true));
    return LVAL2(val, a1, a2);
  }
  case Type::LITERAL:
  {
    if (t2 == Type::LITERAL)
    {
      Value val(Bool(a1.literal()->compare(*a2.literal()) != 0));
      return LVAL2(val, a1, a2);
    }
    Value val(Bool(true));
    return LVAL2(val, a1, a2);
  }
  case Type::COMPLEX:
  {
    if (t2 == Type::COMPLEX)
    {
      Value val(Bool(*a1.complex() != *a2.complex()));
      return LVAL2(val, a1, a2);
    }
    Value val(Bool(true));
    return LVAL2(val, a1, a2);
  }
  case Type::TABCHAR:
  {
    if (t2 == Type::TABCHAR)
    {
      Value val(Bool(*a1.tabchar() != *a2.tabchar()));
      return LVAL2(val, a1, a2);
    }
    Value val(Bool(true));
    return LVAL2(val, a1, a2);
  }
  case Type::ROWTYPE:
  {
    if (t2 == Type::ROWTYPE)
    {
      Value val(Bool(a1.tuple() != a2.tuple()));
      return LVAL2(val, a1, a2);
    }
    Value val(Bool(true));
    return LVAL2(val, a1, a2);
  }
  default:
    break;
  }
  Value val(Bool(true));
  return LVAL2(val, a1, a2);
}

std::string OpNEExpression::unparse(Context&ctx) const
{
  std::string str;
  if (arg2 == nullptr)
    str.append(Operator::OPVALS[Operator::OP_NE]).append(arg1->unparse(ctx));
  else
    str.append("(").append(arg1->unparse(ctx)).append(" ")
            .append(Operator::OPVALS[Operator::OP_NE]).append(" ")
            .append(arg2->unparse(ctx)).append(")");
  return str;
}

}

/*
 *      Copyright (C) 2022 Jean-Luc Barriere
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

#include "builtin_error.h"
#include <blocc/parse_expression.h>
#include <blocc/exception_parse.h>
#include <blocc/context.h>
#include <blocc/parser.h>
#include <blocc/debug.h>

namespace bloc
{

Tuple::container_t ERRORExpression::empty_error()
{
  Tuple::container_t items;
  items.push_back(Value(Value::type_literal)); /* name */
  items.push_back(Value(Value::type_literal)); /* message */
  items.push_back(Value(Value::type_integer)); /* internal id */
  return items;
}

Value& ERRORExpression::value(Context & ctx) const
{
  /* load error from context */
  const RuntimeError& rt = ctx.error();
  Value& tmp = ctx.allocate(Value(new Tuple(empty_error())));
  if (rt.no == EXC_RT_USER_S)
    tmp.tuple()->at(0) = Value(new Literal(rt.what()));
  else
    tmp.tuple()->at(0) = Value(new Literal(RuntimeError::THROWABLES[RuntimeError::throwable(rt.no)].keyword));
  tmp.tuple()->at(1) = Value(new Literal(rt.what()));
  tmp.tuple()->at(2) = Value(Integer(rt.no));
  return tmp;
}

}

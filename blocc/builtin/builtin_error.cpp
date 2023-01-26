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
#include <blocc/expression_integer.h>
#include <blocc/expression_literal.h>
#include <blocc/debug.h>

namespace bloc
{

Tuple::container_t ERRORExpression::empty_error()
{
  Tuple::container_t items;
  items.push_back(new LiteralExpression(std::string())); /* name */
  items.push_back(new LiteralExpression(std::string())); /* message */
  items.push_back(new IntegerExpression(0)); /* internal id */
  return items;
}

Tuple& ERRORExpression::tuple(Context & ctx) const
{
  /* load error from context */
  const RuntimeError& rt = ctx.error();
  if (rt.no == EXC_RT_USER_S)
    v.at(0)->refLiteral().assign(rt.what());
  else
    v.at(0)->refLiteral().assign(rt.THROWABLES[RuntimeError::throwable(rt.no)].keyword);
  v.at(1)->refLiteral().assign(rt.what());
  v.at(2)->refInteger() = rt.no;
  return v;
}

}

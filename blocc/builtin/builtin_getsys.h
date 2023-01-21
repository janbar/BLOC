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

#ifndef BUILTIN_GETCONTEXT_H_
#define BUILTIN_GETCONTEXT_H_

#include <blocc/expression_builtin.h>
#include <blocc/expression_boolean.h>
#include <blocc/expression_integer.h>
#include <blocc/expression_numeric.h>
#include <blocc/expression_literal.h>
#include <blocc/expression_tabchar.h>
#include <blocc/expression_complex.h>

namespace bloc
{

class Context;
class Parser;

class GETSYSExpression : public BuiltinExpression {

public:

  virtual ~GETSYSExpression() { }

  explicit GETSYSExpression(std::vector<Expression*>&& args) : BuiltinExpression(FUNC_GETSYS, std::move(args)) { }

  const Type& type(Context& ctx) const override { return LiteralExpression::type_static; }

  std::string& literal(Context& ctx) const override;

  static GETSYSExpression * parse(Parser& p, Context& ctx);
};

}

#endif /* BUILTIN_GETCONTEXT_H_ */

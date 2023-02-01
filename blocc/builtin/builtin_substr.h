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

#ifndef BUILTIN_SUBSTR_H_
#define BUILTIN_SUBSTR_H_

#include <blocc/expression_builtin.h>
#include <blocc/expression_literal.h>

namespace bloc
{

class Context;
class Parser;

class SUBSTRExpression : public BuiltinExpression {

public:

  virtual ~SUBSTRExpression() { }

  explicit SUBSTRExpression(std::vector<Expression*>&& args) : BuiltinExpression(FUNC_SUBSTR, std::move(args)) { }

  const Type& type(Context& ctx) const override { return LiteralExpression::type_static; }

  std::string& literal(Context& ctx) const override;

  static SUBSTRExpression * parse(Parser& p, Context& ctx);
};

}

#endif /* BUILTIN_SUBSTR_H_ */

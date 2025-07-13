/*
 *      Copyright (C) 2025 Jean-Luc Barriere
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

#ifndef BUILTIN_B64ENC_H_
#define BUILTIN_B64ENC_H_

#include <blocc/expression_builtin.h>

namespace bloc
{

class Context;
class Parser;

class B64ENCExpression : public BuiltinExpression {

public:

  virtual ~B64ENCExpression() { }

  explicit B64ENCExpression(std::vector<Expression*>&& args) : BuiltinExpression(FUNC_B64ENC, std::move(args)) { }

  const Type& type(Context& ctx) const override { return Value::type_literal; }

  Value& value(Context& ctx) const override;

  static B64ENCExpression * parse(Parser& p, Context& ctx);
};

}

#endif /* BUILTIN_B64ENC_H_ */

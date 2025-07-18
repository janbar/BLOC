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

#ifndef BUILTIN_HEX_H_
#define BUILTIN_HEX_H_

#include <blocc/expression_builtin.h>

namespace bloc
{

class Context;
class Parser;

class HEXExpression : public BuiltinExpression {

public:

  virtual ~HEXExpression() { }

  explicit HEXExpression(std::vector<Expression*>&& args) : BuiltinExpression(FUNC_HEX, std::move(args)) { }

  const Type& type(Context& ctx) const override { return Value::type_literal; }

  Value& value(Context& ctx) const override;

  static HEXExpression * parse(Parser& p, Context& ctx);

private:
  static std::string hex(Integer val, Integer n);
};

}

#endif /* BUILTIN_HEX_H_ */

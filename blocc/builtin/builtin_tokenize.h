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

#ifndef BUILTIN_TOKENIZE_H_
#define BUILTIN_TOKENIZE_H_

#include <blocc/expression_builtin.h>

namespace bloc
{

class Context;
class Parser;

class TOKENIZEExpression : public BuiltinExpression {

  Type _type;

public:

  virtual ~TOKENIZEExpression() { }

  explicit TOKENIZEExpression(std::vector<Expression*>&& args)
  : BuiltinExpression(FUNC_TKNIZE, std::move(args))
  , _type(Value::type_literal.levelUp())
  { }

  const Type& type(Context& ctx) const override { return _type; }

  Value& value(Context& ctx) const override;

  static TOKENIZEExpression * parse(Parser& p, Context& ctx);

private:
  Collection * tokenize(const std::string& str, const std::string& sep, bool trimnull) const;
};

}

#endif /* BUILTIN_TOKENIZE_H_ */

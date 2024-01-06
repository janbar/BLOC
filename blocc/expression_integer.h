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

#ifndef EXPRESSION_INTEGER_H
#define EXPRESSION_INTEGER_H

#include "expression.h"

namespace bloc
{

/**
 * This class implements the simplest possible expression, a integer value.
 */
class IntegerExpression : public Expression
{
private:

  mutable Value v;

public:

  virtual ~IntegerExpression() { }

  explicit IntegerExpression(Integer a) : v(a) { v.to_lvalue(true); }
  explicit IntegerExpression(Value&& _v) : v(std::move(_v)) { v.to_lvalue(true); }

  const Type& type(Context& ctx) const override
  {
    return v.type();
  }

  Value& value(Context& ctx) const override
  {
    return v;
  }

  bool isConst() const override { return true; }

  std::string unparse(Context& ctx) const override;

  std::string toString(Context& ctx) const override
  {
    return v.toString();
  }

  static IntegerExpression * parse(const std::string& text);

};

}

#endif /* EXPRESSION_INTEGER_H */

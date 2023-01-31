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

#include "expression_static.h"
#include "declspec.h"

namespace bloc
{

/**
 * This class implements the simplest possible expression, a integer value.
 */
class IntegerExpression : public SwappableExpression<IntegerExpression>
{
private:

  int64_t v;

public:
  LIBBLOC_API static const Type& type_static;

  virtual ~IntegerExpression() { }

  explicit IntegerExpression(int64_t a) : v(a) { }

  const Type& type(Context& ctx) const override
  {
    return type_static;
  }

  int64_t integer(Context& ctx) const override
  {
    return v;
  }

  /* to support type mixing */
  double numeric(Context& ctx) const override
  {
    return (double) v;
  }

  std::string unparse(Context& ctx) const override
  {
    return readableInteger(v);
  }

  std::string toString(Context& ctx) const override
  {
    return std::string(typeName(ctx))
            .append(1, ' ')
            .append(readableInteger(v));
  }

  static IntegerExpression * parse(const std::string& text, int base = 10)
  {
    return new IntegerExpression((int64_t)std::stoll(text, 0, base));
  }

  static std::string readableInteger(int64_t l)
  {
    return std::to_string(l);
  }

  const Type& refType() const override { return type_static; }

  int64_t& refInteger() override { return v; }

  void swap(IntegerExpression& e) noexcept override
  {
    int64_t tmp = e.v;
    e.v = v;
    v = tmp;
  }

  IntegerExpression * swapNew() override
  {
    return new IntegerExpression(this->v);
  }

  IntegerExpression * cloneNew() const override
  {
    return new IntegerExpression(this->v);
  }
};

}

#endif /* EXPRESSION_INTEGER_H */

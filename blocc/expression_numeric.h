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

#ifndef EXPRESSION_NUMERIC_H
#define EXPRESSION_NUMERIC_H

#include "expression_static.h"
#include "declspec.h"

namespace bloc
{

/**
 * This class implements the simplest possible expression, a numeric value.
 */
class NumericExpression : public SwappableExpression<NumericExpression>
{
private:

  double v;

public:
  LIBBLOC_API static const Type& type_static;
  LIBBLOC_API static const double& null;

  virtual ~NumericExpression() { }

  explicit NumericExpression(double a) : v(a) { }

  const Type& refType() const override
  {
    return type_static;
  }

  /* to support type mixing */
  int64_t integer(Context& ctx) const override
  {
    return (int64_t) v;
  }

  double numeric(Context& ctx) const override
  {
    return v;
  }

  std::string unparse(Context& ctx) const override
  {
    std::string str = readableNumeric(v);
    if (str.find('.') == std::string::npos)
      str.append(".0");
    return str;
  }

  std::string toString(Context& ctx) const override
  {
    return std::string(typeName(ctx))
            .append(1, ' ')
            .append(readableNumeric(v));
  }

  static NumericExpression * parse(const std::string& text)
  {
    return new NumericExpression(std::stod(text));
  }

  static std::string readableNumeric(double d)
  {
    char buf[32];
    snprintf(buf, sizeof(buf), "%.16g", d);
    return buf;
  }

  double& refNumeric() override { return v; }

  void swap(NumericExpression& e) noexcept override
  {
    double tmp = e.v;
    e.v = v;
    v = tmp;
  }

  NumericExpression * swapNew() override
  {
    return new NumericExpression(this->v);
  }

  NumericExpression * cloneNew() const override
  {
    return new NumericExpression(this->v);
  }
};

}

#endif /* EXPRESSION_NUMERIC_H */

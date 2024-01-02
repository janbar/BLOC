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

#ifndef EXPRESSION_COMPLEX_H
#define EXPRESSION_COMPLEX_H

#include "expression_static.h"
#include "declspec.h"

namespace bloc
{

/**
 * This class implements the simplest possible expression, a complex value.
 */
class ComplexExpression : public SwappableExpression<ComplexExpression>
{
private:

  mutable Complex v;

public:
  LIBBLOC_API static const Type& null;

  virtual ~ComplexExpression() { }

  explicit ComplexExpression(const Complex& c) : v(c) { }
  explicit ComplexExpression(Complex&& c) : v(std::move(c)) { }

  const Type& refType() const override
  {
    return v.complex_type();
  }

  Complex& complex(Context& ctx) const override
  {
    return v;
  }

  std::string unparse(Context& ctx) const override
  {
    return ""; /* not printable */
  }

  std::string toString(Context& ctx) const override
  {
    return readableComplex(v);
  }

  static std::string readableComplex(const Complex& c);

  Complex& refComplex() override { return v; }

  void swap(ComplexExpression& e) noexcept override
  {
    v.swap(e.v);
  }

  ComplexExpression * swapNew() override
  {
    return new ComplexExpression(std::move(v));
  }

  ComplexExpression * cloneNew() const override
  {
    return new ComplexExpression(v);
  }

  std::string typeName(Context& ctx) const override;
};

}

#endif /* EXPRESSION_COMPLEX_H */

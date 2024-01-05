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
#include "expression_boolean.h"
#include "expression_integer.h"
#include "expression_numeric.h"
#include "expression_literal.h"
#include "expression_tabchar.h"
#include "expression_collection.h"
#include "expression_tuple.h"
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
  mutable StaticExpression * v_null = nullptr;

public:
  LIBBLOC_API static const Type& null;

  virtual ~ComplexExpression();

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

  bool boolean(Context& ctx) const override
  {
    if (v.typeId())
      throw RuntimeError(EXC_RT_NOT_BOOLEAN);
    return false; /* boolean cannot be null */
  }

  int64_t integer(Context& ctx) const override
  {
    if (v.typeId())
      throw RuntimeError(EXC_RT_NOT_INTEGER);
    return 0; /* integer cannot be null */
  }

  double numeric(Context& ctx) const override
  {
    if (v.typeId())
      throw RuntimeError(EXC_RT_NOT_NUMERIC);
    return NumericExpression::null;
  }

  std::string& literal(Context& ctx) const override
  {
    if (v.typeId())
      throw RuntimeError(EXC_RT_NOT_LITERAL);
    if (v_null)
      delete v_null;
    v_null = new LiteralExpression(LiteralExpression::null);
    return v_null->refLiteral();
  }

  TabChar& tabchar(Context& ctx) const override
  {
    if (v.typeId())
      throw RuntimeError(EXC_RT_NOT_TABCHAR);
    if (v_null)
      delete v_null;
    v_null = new TabcharExpression(TabcharExpression::null);
    return v_null->refTabchar();
  }

  Collection& collection(Context& ctx) const override
  {
    if (v.typeId())
      throw RuntimeError(EXC_RT_NOT_COLLECT);
    if (v_null)
      delete v_null;
    v_null = new CollectionExpression(CollectionExpression::null);
    return v_null->refCollection();
  }

  Tuple& tuple(Context& ctx) const override
  {
    if (v.typeId())
      throw RuntimeError(EXC_RT_NOT_ROWTYPE);
    if (v_null)
      delete v_null;
    v_null = new TupleExpression(TupleExpression::null);
    return v_null->refTuple();
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

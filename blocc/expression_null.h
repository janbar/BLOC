/*
 *      Copyright (C) 2023 Jean-Luc Barriere
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

#ifndef EXPRESSION_NULL_H
#define EXPRESSION_NULL_H

#include "expression_static.h"
#include "expression_boolean.h"
#include "expression_integer.h"
#include "expression_numeric.h"
#include "expression_literal.h"
#include "expression_complex.h"
#include "expression_tabchar.h"
#include "expression_collection.h"
#include "expression_tuple.h"
#include "declspec.h"

namespace bloc
{

/**
 * This class implements the simplest possible expression, a null value.
 */
class NullExpression : public SwappableExpression<NullExpression>
{
private:

  mutable StaticExpression * v = nullptr;

public:
  LIBBLOC_API static const Type& null;
  LIBBLOC_API static const Complex& null_complex;

  LIBBLOC_API static const double& null_numeric;
  LIBBLOC_API static const std::string& null_literal;
  LIBBLOC_API static const TabChar& null_tabchar;
  LIBBLOC_API static const Collection& null_collection;
  LIBBLOC_API static const Tuple& null_tuple;

  virtual ~NullExpression();

  NullExpression() : v(new ComplexExpression(null_complex)) { }

  const Type& refType() const override
  {
    return null;
  }

  bool boolean(Context& ctx) const override
  {
    if (v)
      delete v;
    v = new BooleanExpression(false);
    return v->refBoolean();
  }

  int64_t integer(Context& ctx) const override
  {
    if (v)
      delete v;
    v = new IntegerExpression(0);
    return v->refInteger();
  }

  double numeric(Context& ctx) const override
  {
    if (v)
      delete v;
    v = new NumericExpression(null_numeric);
    return v->refNumeric();
  }

  std::string& literal(Context& ctx) const override
  {
    if (v)
      delete v;
    v = new LiteralExpression(null_literal);
    return v->refLiteral();
  }

  TabChar& tabchar(Context& ctx) const override
  {
    if (v)
      delete v;
    v = new TabcharExpression(null_tabchar);
    return v->refTabchar();
  }

  Complex& complex(Context& ctx) const override
  {
    if (v)
      delete v;
    v = new ComplexExpression(null_complex);
    return v->refComplex();
  }

  Collection& collection(Context& ctx) const override
  {
    if (v)
      delete v;
    v = new CollectionExpression(null_collection);
    return v->refCollection();
  }

  Tuple& tuple(Context& ctx) const override
  {
    if (v)
      delete v;
    v = new TupleExpression(null_tuple);
    return v->refTuple();
  }

  std::string unparse(Context& ctx) const override
  {
    return ""; /* not printable */
  }

  std::string toString(Context& ctx) const override
  {
    return "null";
  }

  bool& refBoolean() override
  {
    return v->refBoolean();
  }

  int64_t& refInteger() override
  {
    return v->refInteger();
  }

  double& refNumeric() override
  {
    return v->refNumeric();
  }

  std::string& refLiteral() override
  {
    return v->refLiteral();
  }

  TabChar& refTabchar() override
  {
    return v->refTabchar();
  }

  Complex& refComplex() override
  {
    return v->refComplex();
  }

  Collection& refCollection() override
  {
    return v->refCollection();
  }

  Tuple& refTuple() override
  {
    return v->refTuple();
  }

  void swap(NullExpression& e) noexcept override
  {
    StaticExpression * tmp = v;
    v = e.v;
    e.v = tmp;
  }

  NullExpression * swapNew() override
  {
    NullExpression * e = new NullExpression();
    e->v = v;
    v = nullptr;
    return e;
  }

  NullExpression * cloneNew() const override
  {
    NullExpression * e = new NullExpression();
    e->v = v->cloneNew();
    return e;
  }

  static bool isNull(const Type& type)
  {
    return type == null;
  }

  static bool isNull(double d)
  {
    return d != d; /* check NaN */
  }

  static bool isNull(const std::string& s)
  {
    return s.size() == 0;
  }

  static bool isNull(const TabChar& x)
  {
    return x.size() == 0;
  }

  static bool isNull(const Complex& o)
  {
    return o.typeId() == null_complex.typeId();
  }

  static bool isNull(const Collection& c)
  {
    return c.size() == 0 && c.table_type() == null_collection.table_type();
  }

  static bool isNull(const Tuple& t)
  {
    return t.tuple_type() == null_tuple.tuple_type();
  }
};

}

#endif /* EXPRESSION_NULL_H */

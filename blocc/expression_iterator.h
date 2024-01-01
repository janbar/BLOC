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

#ifndef EXPRESSION_ITERATOR_H
#define EXPRESSION_ITERATOR_H

#include "expression_static.h"

namespace bloc
{

/**
 * This class implements the simplest possible expression, an iterator.
 */
class IteratorExpression : public SwappableExpression<IteratorExpression>
{
private:

  Collection& v;
  size_t _index;

public:

  virtual ~IteratorExpression() { }

  IteratorExpression(Collection& c, size_t i) : v(c), _index(i) { }
  explicit IteratorExpression(Collection& c) : v(c), _index(0) { }
  size_t& index() { return _index; }

  const Type& type(Context& ctx) const override { return v.at(_index)->type(ctx); }
  const Type& refType() const override { return v.at(_index)->refType(); }

  bool boolean(Context& ctx) const override { return v.at(_index)->refBoolean(); }
  int64_t integer(Context& ctx) const override { return v.at(_index)->refInteger(); }
  double numeric(Context& ctx) const override { return v.at(_index)->refNumeric(); }
  std::string& literal(Context& ctx) const override { return v.at(_index)->refLiteral(); }
  TabChar& tabchar(Context& ctx) const override { return v.at(_index)->refTabchar(); }
  Collection& collection(Context& ctx) const override { return v.at(_index)->refCollection(); }
  Tuple& tuple(Context& ctx) const override { return v.at(_index)->refTuple(); }
  Complex& complex(Context& ctx) const override { return v.at(_index)->refComplex(); }

  bool& refBoolean() override { return v.at(_index)->refBoolean(); }

  int64_t& refInteger() override { return v.at(_index)->refInteger(); }

  double& refNumeric() override { return v.at(_index)->refNumeric(); }

  std::string& refLiteral() override { return v.at(_index)->refLiteral(); }

  TabChar& refTabchar() override { return v.at(_index)->refTabchar(); }

  Collection& refCollection() override { return v.at(_index)->refCollection(); }

  Tuple& refTuple() override { return v.at(_index)->refTuple(); }

  const TupleDecl::Decl& tuple_decl(Context& ctx) const override { return v.at(_index)->tuple_decl(ctx); }

  Complex& refComplex() override { return v.at(_index)->refComplex(); }

  /* opaque can be typed */
  bool isStored() const override { return true; }

  std::string unparse(Context& ctx) const override
  {
    return ""; /* not printable */
  }

  std::string toString(Context& ctx) const override
  {
    return std::string(typeName(ctx));
  }

  void swap(IteratorExpression& e) noexcept override
  {
    v.swap(e.v);
    size_t p = _index;
    _index = e._index;
    e._index = p;
  }

  IteratorExpression * swapNew() override
  {
    return new IteratorExpression(this->v, this->_index);
  }

  IteratorExpression * cloneNew() const override
  {
    return new IteratorExpression(this->v, this->_index);
  }

  std::string typeName(Context& ctx) const override;
};

}

#endif /* EXPRESSION_ITERATOR_H */

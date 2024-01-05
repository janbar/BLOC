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

#ifndef EXPRESSION_TUPLE_H
#define EXPRESSION_TUPLE_H

#include "expression_static.h"
#include "tuple.h"

namespace bloc
{

/**
 * This class implements the simplest possible expression, a tuple of value.
 */
class TupleExpression : public SwappableExpression<TupleExpression>
{
private:

  mutable Tuple v;

public:
  LIBBLOC_API static const Tuple& null;

  virtual ~TupleExpression() { }

  explicit TupleExpression(const Tuple& a) : v(a) { }
  explicit TupleExpression(Tuple &&a) : v(std::move(a)) { }

  const Type& refType() const override
  {
    return v.tuple_type();
  }

  Tuple& tuple(Context& ctx) const override
  {
    return v;
  }

  const Tuple::Decl& tuple_decl(Context& ctx) const override
  {
    return v.tuple_decl();
  }

  std::string unparse(Context& ctx) const override
  {
    return ""; /* not printable */
  }

  std::string toString(Context& ctx) const override
  {
    return typeName(ctx);
  }

  static std::string readableTuple(const Tuple& c, Context& ctx);

  Tuple& refTuple() override { return v; }

  void swap(TupleExpression& e) noexcept override
  {
    v.swap(e.v);
  }

  TupleExpression * swapNew() override
  {
    return new TupleExpression(std::move(this->v));
  }

  TupleExpression * cloneNew() const override
  {
    return new TupleExpression(this->v);
  }

  std::string typeName(Context& ctx) const override
  {
    return tuple_decl(ctx).tupleName();
  }
};

}

#endif /* EXPRESSION_TUPLE_H */

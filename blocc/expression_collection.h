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

#ifndef EXPRESSION_COLLECTION_H
#define EXPRESSION_COLLECTION_H

#include "expression_static.h"

namespace bloc
{

/**
 * This class implements the simplest possible expression, a array of value.
 */
class CollectionExpression : public StaticExpression
{
private:

  mutable Collection v;

public:

  virtual ~CollectionExpression() { }

  explicit CollectionExpression(const Collection& a) : StaticExpression(), v(a) { }
  explicit CollectionExpression(Collection &&a) : StaticExpression(), v(std::move(a)) { }

  const Type& type(Context& ctx) const override { return v.table_type(); }

  const Tuple::Decl& tuple_decl(Context& ctx) const override { return v.table_decl(); }

  Collection& collection(Context& ctx) const override  { return v; }

  std::string unparse(Context& ctx) const override
  {
    return ""; /* not printable */
  }

  std::string toString(Context& ctx) const override
  {
    return std::string(typeName(ctx))
            .append(1, '[')
            .append(std::to_string(v.size()))
            .append(1, ']');
  }

  const Type& refType() const override { return v.table_type(); }

  Collection& refCollection() override { return v; }

  void swap(StaticExpression& e) override
  {
    v.swap(e.refCollection());
  }

  CollectionExpression * swapNew() override
  {
    return new CollectionExpression(std::move(this->v));
  }

  CollectionExpression * cloneNew() const override
  {
    return new CollectionExpression(this->v);
  }

  std::string typeName(Context& ctx) const override;
};

}

#endif /* EXPRESSION_COLLECTION_H */

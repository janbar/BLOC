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

#ifndef TUPLE_H_
#define TUPLE_H_

#include "intrinsic_type.h"
#include "tuple_decl.h"

#include <vector>

namespace bloc
{

class StaticExpression;

class Tuple : public TupleDecl
{
public:
  typedef std::vector<StaticExpression*> container_t;
  typedef container_t::iterator iterator;
  typedef container_t::const_iterator const_iterator;
  typedef StaticExpression*& reference;
  typedef const StaticExpression*& const_reference;

  virtual ~Tuple();
  explicit Tuple(container_t&& items);

  Tuple(const Tuple& t);
  Tuple(Tuple&& t) noexcept : v(std::move(t.v)), _decl(std::move(t._decl)), _type(t._type) { }

  const Type& tuple_type() const { return _type; }
  const Decl& tuple_decl() const override { return _decl; }

  void swap(Tuple& t) noexcept;

  reference operator[](unsigned pos) { return v[pos]; }
  reference at(unsigned pos) { return v.at(pos); }
  const_reference operator[](unsigned pos) const { return const_cast<const_reference>(v[pos]); }
  const_reference at(unsigned pos) const { return const_cast<const_reference>(v.at(pos)); }
  size_t size() const { return v.size(); }
  const_iterator begin() const { return v.begin(); }
  const_iterator end() const { return v.end(); }

private:
  container_t v;
  Decl _decl;
  Type _type;
};

}

#endif /* TUPLE_H_ */

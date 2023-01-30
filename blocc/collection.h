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

#ifndef COLLECTION_H_
#define COLLECTION_H_

#include "intrinsic_type.h"
#include "tuple.h"

#include <vector>

namespace bloc
{

class StaticExpression;

class Collection
{
public:
  typedef std::vector<StaticExpression*> container_t;
  typedef container_t::iterator iterator;
  typedef container_t::const_iterator const_iterator;
  typedef StaticExpression*& reference;
  typedef const StaticExpression*& const_reference;

  virtual ~Collection() { this->clear(); }
  explicit Collection(const Type& type) : v(), _type(type) { }
  explicit Collection(const Tuple::Decl& decl, Type::TypeLevel level)
  : v(), _decl(decl) { _type = _decl.make_type(level); }

  Collection(const Collection& t);
  Collection(Collection&& t) noexcept
  : v(std::move(t.v)), _decl(std::move(t._decl)), _type(t._type) { }

  const Type& table_type() const { return _type; }
  const Tuple::Decl& table_decl() const { return _decl; }

  void swap(Collection& t) noexcept;
  void clear();
  void reserve(unsigned n) { v.reserve(n) ; }

  reference operator[](unsigned pos) { return v[pos]; }
  reference at(unsigned pos) { return v.at(pos); }
  const_reference operator[](unsigned pos) const { return const_cast<const_reference>(v[pos]); }
  const_reference at(unsigned pos) const { return const_cast<const_reference>(v.at(pos)); }
  size_t size() const { return v.size(); }
  const_iterator begin() const { return v.begin(); }
  const_iterator end() const { return v.end(); }
  void push_back(StaticExpression* const& e) { v.push_back(e); }
  void push_back(StaticExpression*&& e) { v.push_back(std::move(e)); }
  iterator insert(const_iterator pos, StaticExpression* const& e) { return v.insert(pos, e); }
  iterator erase(const_iterator pos);
  iterator erase(const_iterator first, const_iterator last);

private:
  container_t v;
  Tuple::Decl _decl;
  Type _type;
};

}

#endif /* COLLECTION_H_ */

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

#include "tuple.h"
#include "expression_static.h"
#include "expression_boolean.h"
#include "expression_integer.h"
#include "expression_numeric.h"
#include "expression_literal.h"
#include "expression_complex.h"
#include "expression_tabchar.h"

#include <cstring>
#include <cassert>

namespace bloc
{

Tuple::~Tuple()
{
  for (StaticExpression * e : v)
    delete e;
}

Tuple::Tuple(container_t&& items)
{
  _decl.reserve(items.size());
  for (const StaticExpression * e : items)
    _decl.push_back(e->refType());
  _type = _decl.make_type(0);
  v.swap(items);
}

Tuple::Tuple(const Tuple& t) : v(), _decl(t._decl), _type(t._type)
{
  /* clone elements */
  v.reserve(t.size());
  for (const StaticExpression * e : t.v)
    v.push_back(e->cloneNew());
}

void Tuple::swap(Tuple& t) noexcept
{
  assert(_type == t._type);
  v.swap(t.v);
}

}

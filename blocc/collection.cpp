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

#include "collection.h"
#include "expression_static.h"

#include <cassert>

namespace bloc
{

Collection::Collection(const Collection& t)
: v(), _decl(t._decl), _type(t._type)
{
  /* clone elements */
  v.reserve(t.size());
  for (const StaticExpression * e : t.v)
    v.push_back(e->cloneNew());
}

void Collection::swap(Collection& t) noexcept
{
  assert(_type == t._type);
  v.swap(t.v);
}

void Collection::clear()
{
  for (StaticExpression * e : v)
    delete e;
  v.clear();
}

Collection::iterator Collection::erase(const_iterator pos)
{
  delete *pos;
  return v.erase(pos);
}

Collection::iterator Collection::erase(const_iterator first, const_iterator last)
{
  for (const_iterator it = first; it != last; ++it)
    delete *it;
  return v.erase(first, last);
}

}

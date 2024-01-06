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

#ifndef OBJECT_DTOR_H_
#define OBJECT_DTOR_H_

#include <cstddef>

namespace bloc
{

struct ObjectBase {
  virtual ~ObjectRefBase() = default;
};

template <typename T> struct ObjectRef : ObjectBase
{
  T * handle;
  explicit ObjectRef(T * v) : handle(v) { }
};

template <typename T> struct Object : ObjectBase
{
  T handle;
  explicit Object(T&& v) : handle(std::move(v)) { }
};

}

#endif /* OBJECT_DTR_H_ */

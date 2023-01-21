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

#ifndef TEMPORARY_H_
#define TEMPORARY_H_

#include <cstddef>

namespace bloc
{

struct TemporaryBase {
  virtual ~TemporaryBase() = default;
  virtual size_t size() const = 0;
};

template <typename T> struct Temporary : TemporaryBase
{
  T handle;
  explicit Temporary(T&& hdl) : handle(std::move(hdl)) { }
  size_t size() const override { return handle.size(); }
};

}

#endif /* TEMPORARY_H_ */

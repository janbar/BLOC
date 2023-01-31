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

#ifndef COMPLEX_H_
#define COMPLEX_H_

#include "intrinsic_type.h"

#include <string>
#include <vector>

namespace bloc
{

class Expression;
class Context;

class Complex
{
  int * _refcount = nullptr;
  void * _instance = nullptr;
  Type _type;

public:

  bool operator==(const Complex& c) const { return (this->_instance == c._instance); }
  bool operator!=(const Complex& c) const { return !(*this == c); }

  Complex() { _refcount = new int(1); }

  explicit Complex(Type::TypeMinor type_id) : _type(Type::COMPLEX, type_id) { _refcount = new int(1); }

  ~Complex();

  Complex(const Complex& c);

  Complex(Complex&& c) noexcept;

  Complex& operator=(const Complex& c);

  bool CTOR(int ctor_id, Context& ctx, const std::vector<Expression*>& args);

  void swap(Complex& c) noexcept;

  void swap(Complex&& c) noexcept;

  Type::TypeMinor typeId() const { return _type.minor(); }

  void * instance() const { return _instance; }

  const Type& complex_type() const { return _type; }
};

}

#endif /* COMPLEX_H_ */

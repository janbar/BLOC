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

#include "complex.h"
#include "context.h"
#include "expression.h"
#include "import_manager.h"

#include <cassert>

namespace bloc
{

Complex::~Complex()
{
  if (_refcount)
  {
    if ((*_refcount -= 1) == 0)
    {
      delete _refcount;
      _refcount = nullptr;
      if (_instance)
      {
        ImportManager::instance().module(typeId()).instance->destroyObject(_instance);
        _instance = nullptr;
      }
    }
  }
}

Complex::Complex(const Complex& c)
: _refcount(c._refcount)
, _type(c._type)
, _instance(c._instance)
{
  *_refcount += 1;
}

Complex::Complex(Complex&& c) noexcept
: _refcount(c._refcount)
, _type(c._type)
, _instance(c._instance)
{
  c._refcount = nullptr;
  c._type = Type(Type::COMPLEX);
  c._instance = nullptr;
}

Complex& Complex::operator=(const Complex& c)
{
  if (this == &c)
    return *this;
  /* reset */
  if ((*_refcount -= 1) == 0)
  {
    delete _refcount;
    if (_instance)
      ImportManager::instance().module(typeId()).instance->destroyObject(_instance);
  }
  /* and set */
  _refcount = c._refcount;
  _instance = c._instance;
  _type = c._type;
  if ((*_refcount += 1) < 2)
  {
    _refcount = nullptr;
    _instance = nullptr;
  }
  return *this;
}

bool Complex::CTOR(int ctor_id, Context& ctx, const std::vector<Expression*>& args)
{
  assert(_type.minor());
  assert(_instance == nullptr);
  _instance = ImportManager::instance().module(typeId()).instance->createObject(ctor_id, ctx, args);
  return (_instance ? true : false);
}

void Complex::swap(Complex& c) noexcept
{
  int * tmp_ref = _refcount;
  void * tmp_ptr = _instance;
  Type tmp_typ = _type;
  _refcount = c._refcount;
  _instance = c._instance;
  _type = c._type;
  c._refcount = tmp_ref;
  c._instance = tmp_ptr;
  c._type = tmp_typ;
}

void Complex::swap(Complex&& c) noexcept
{
  _refcount = c._refcount;
  _instance = c._instance;
  _type = c._type;
  c._refcount = nullptr;
  c._instance = nullptr;
  c._type = Type(Type::COMPLEX);
}

}

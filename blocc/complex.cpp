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
#include "plugin_manager.h"
#include "debug.h"

#include <string>
#include <cassert>

//#define DEBUG_COMPLEX

namespace bloc
{

Complex::Complex(Type::TypeMinor type_id, void * handle)
: _type(Type::COMPLEX, type_id)
, _instance(handle)
{
#ifdef DEBUG_COMPLEX
  DBG(DBG_DEBUG, "%s line %d\n", __PRETTY_FUNCTION__, __LINE__);
#endif
  assert(_instance);
  _refcount = new int(1);
}

Complex * Complex::newInstance(
        Type::TypeMinor type_id,
        int ctor_id,
        Context& ctx,
        const std::vector<Expression*>& args)
{
#ifdef DEBUG_COMPLEX
  DBG(DBG_DEBUG, "%s line %d\n", __PRETTY_FUNCTION__, __LINE__);
#endif
  const PLUGGED_MODULE& module = PluginManager::instance().plugged(type_id);
  if (module.instance)
  {
    void * handle = module.instance->createObject(ctor_id, ctx, args);
    if (handle)
      return new Complex(type_id, handle);
  }
  return nullptr;
}

Complex::~Complex()
{
#ifdef DEBUG_COMPLEX
  DBG(DBG_DEBUG, "%s line %d\n", __PRETTY_FUNCTION__, __LINE__);
#endif
  if ((*_refcount -= 1) == 0)
  {
    delete _refcount;
    _refcount = nullptr;
    PluginManager::instance().plugged(typeId()).instance->destroyObject(_instance);
  }
}

Complex::Complex(const Complex& c)
: _refcount(c._refcount)
, _instance(c._instance)
, _type(c._type)
{
#ifdef DEBUG_COMPLEX
  DBG(DBG_DEBUG, "%s line %d\n", __PRETTY_FUNCTION__, __LINE__);
#endif
  *_refcount += 1;
}

Complex::Complex(Complex&& c) noexcept
: _refcount(c._refcount)
, _instance(c._instance)
, _type(c._type)
{
#ifdef DEBUG_COMPLEX
  DBG(DBG_DEBUG, "%s line %d\n", __PRETTY_FUNCTION__, __LINE__);
#endif
  c._refcount = nullptr;
  c._instance = nullptr;
}

Complex& Complex::operator=(const Complex& c)
{
#ifdef DEBUG_COMPLEX
  DBG(DBG_DEBUG, "%s line %d\n", __PRETTY_FUNCTION__, __LINE__);
#endif
  if (this == &c)
    return *this;
  /* reset */
  if ((*_refcount -= 1) == 0)
  {
    delete _refcount;
    _refcount = nullptr;
    PluginManager::instance().plugged(typeId()).instance->destroyObject(_instance);
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

void Complex::swap(Complex& c) noexcept
{
#ifdef DEBUG_COMPLEX
  DBG(DBG_DEBUG, "%s line %d\n", __PRETTY_FUNCTION__, __LINE__);
#endif
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
#ifdef DEBUG_COMPLEX
  DBG(DBG_DEBUG, "%s line %d\n", __PRETTY_FUNCTION__, __LINE__);
#endif
  /* release handle */
  if ((*_refcount -= 1) == 0)
  {
    delete _refcount;
    _refcount = nullptr;
    PluginManager::instance().plugged(typeId()).instance->destroyObject(_instance);
  }
  /*  move handle */
  _refcount = c._refcount;
  _instance = c._instance;
  _type = c._type;
  c._refcount = nullptr;
  c._instance = nullptr;
}

const char * Complex::typeIdName() const
{
  return PluginManager::instance().plugged(typeId()).interface.name;
}

}

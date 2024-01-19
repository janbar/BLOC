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

#include "symbol.h"
#include "debug.h"

//#define DEBUG_SYMBOL

namespace bloc
{

void Symbol::upgrade(const Type& to_type)
{
#ifdef DEBUG_SYMBOL
  DBG(DBG_DEBUG, "%s line %d\n", __PRETTY_FUNCTION__, __LINE__);
#endif
  _minor = to_type.minor();
  _level = to_type.level();
  _major = to_type.major();
  _decl.clear();
}

void Symbol::upgrade(const Decl& to_decl, Type::TypeLevel level)
{
#ifdef DEBUG_SYMBOL
  DBG(DBG_DEBUG, "%s line %d\n", __PRETTY_FUNCTION__, __LINE__);
#endif
  Type to_type = to_decl.make_type(level);
  _minor = to_type.minor();
  _level = to_type.level();
  _major = to_type.major();
  _decl = to_decl;
}

Symbol::SafetyCheck Symbol::check_safety(const Type& type) const noexcept
{
#ifdef DEBUG_SYMBOL
  DBG(DBG_DEBUG, "%s line %d : S={%d,%d,%d} T={%d,%d,%d}\n", __PRETTY_FUNCTION__, __LINE__,
          major(), minor(), level(), type.major(), type.minor(), type.level());
#endif
  switch (major())
  {

  /* symbol is table null or undefined */
  case Type::NO_TYPE:
     /* undefined / undefinded */
    if (type == Type::NO_TYPE)
    {
      /* undefined / undefinded */
      if (level() == type.level())
        return SAFE_EQU;
      if (type.level() > 0)
        return SAFE_UPG;
      return SAFE_FEA;
    }
    /* table null / table qualified */
    if (level() > 0 && type.level() > 0)
      return SAFE_UPG;
    /* undefined / qualified */
    if (level() == type.level())
      return SAFE_UPG;

    break; /* KO */

  /* symbol is qualified or tuple opaque */
  default:
    if (level() > 0)
    {
      /* qualified table / undefined */
      if (type == Type::NO_TYPE)
        return SAFE_FEA;
      /* qualified table / qualified table */
      if (type == *this)
        return SAFE_EQU;
    }
    /* not table */
    else if (type.level() == 0)
    {
      /* any / undefined */
      if (type == Type::NO_TYPE)
        return SAFE_FEA;
      /* qualified / qualified */
      if (type == *this)
        return SAFE_EQU;
      if (major() == type.major())
      {
        if (minor() == 0)
          return SAFE_UPG;
        if (type.minor() == 0)
          return SAFE_FEA;
      }
    }

    break; /* KO */
  }
  return SAFE_KO;
}

}

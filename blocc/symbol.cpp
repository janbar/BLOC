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
  if (type == *this)
    return SAFE_EQU;
  else if (level() > 0)
  {
    /* qualified or undefined table */
    if (type.level() > 0)
      return SAFE_UPG;
  }
  else if (type.level() == 0)
  {
    /* qualified type */
    if (major() == type.major())
      return SAFE_UPG;
  }
  return SAFE_KO;
}

}

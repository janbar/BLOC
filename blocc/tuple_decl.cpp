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

#include "tuple_decl.h"
#include "plugin_manager.h"

#include <cstdint>

namespace bloc
{

const TupleDecl::Decl TupleDecl::no_decl = TupleDecl::Decl();

std::string TupleDecl::Decl::tupleName() const
{
  std::string sb;
  sb.append(1, '{');
  for (int i = 0; i < this->size(); ++i)
  {
    if (i > 0)
      sb.append(1, ',');
    sb.append(1, ' ');
    if (this->at(i).major() == Type::COMPLEX)
      sb.append(PluginManager::instance().plugged(this->at(i).minor()).interface.name);
    else
      sb.append(Type::typeName(this->at(i).major()));
  }
  sb.append(" }");
  return sb;
}

Type TupleDecl::Decl::make_type(Type::TypeLevel level) const
{
  unsigned len = this->size();
  /* tuple */
  if (len > 0)
  {
    /*
     * DJB Hash Function
     */
    uint_fast32_t h = 5381;
    for (auto& m : *this)
    {
      h = ((h << 5) + h) + ((m.minor() << (8 * sizeof(Type::TypeMajor))) + m.major());
    }
    return Type(Type::ROWTYPE, (Type::TypeMinor) (h % TYPE_MINOR_MAX), level);
  }
  /* empty */
  return Type(Type::NO_TYPE, 0, level);
}

}

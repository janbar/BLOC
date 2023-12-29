/*
 *      Copyright (C) 2023 Jean-Luc Barriere
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

#include "expression_iterator.h"
#include "plugin_manager.h"

namespace bloc
{

std::string IteratorExpression::typeName(Context& ctx) const
{
  switch (type(ctx).major())
  {
  case Type::COMPLEX:
    return type(ctx).typeName(PluginManager::instance().plugged(type(ctx).minor()).interface.name);
  case Type::ROWTYPE:
    return type(ctx).typeName(tuple_decl(ctx).tupleName().c_str());
  default:
    return type(ctx).typeName();
  }
}

}

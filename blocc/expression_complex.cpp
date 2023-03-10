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

#include "expression_complex.h"
#include "plugin_manager.h"
#include "parser.h"

namespace bloc
{

const Type& ComplexExpression::null = Type(Type::COMPLEX);

std::string ComplexExpression::readableComplex(const Complex& c)
{
  std::string sb(PluginManager::instance().plugged(c.typeId()).interface.name);
  if (c.typeId())
  {
    char buf[24];
    snprintf(buf, sizeof(buf), "%p", c.instance());
    sb.append(1, '(');
    sb.append(buf);
    sb.append(1, ')');
  }
  return sb;
}

std::string ComplexExpression::typeName(Context& ctx) const
{
  const PLUGGED_MODULE& plugin = PluginManager::instance().plugged(v.typeId());
  return plugin.interface.name;
}

}

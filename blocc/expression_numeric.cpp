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

#include "expression_numeric.h"

#include <string>

namespace bloc
{

std::string NumericExpression::unparse(Context& ctx) const
{
  std::string str = Value::readableNumeric(*v.numeric());
  if (str.find('e') == std::string::npos && str.find('.') == std::string::npos)
    str.append(".0");
  return str;
}

NumericExpression * NumericExpression::parse(const std::string& text)
{
  return new NumericExpression(std::stod(text));
}

}

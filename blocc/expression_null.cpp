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

#include "expression_null.h"

#include <cmath>

namespace bloc
{

const Type& NullExpression::null = Type(Type::COMPLEX);
const Complex& NullExpression::null_complex = Complex(0);

const double& NullExpression::null_numeric = NAN;
const std::string& NullExpression::null_literal = std::string("");
const TabChar& NullExpression::null_tabchar = TabChar();
const Collection& NullExpression::null_collection = Collection(Type::NO_TYPE);
const Tuple& NullExpression::null_tuple = Tuple(Tuple::container_t());

NullExpression::~NullExpression()
{
  if (v)
    delete v;
}

}

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

#include "expression_tuple.h"
#include "expression_boolean.h"
#include "expression_integer.h"
#include "expression_numeric.h"
#include "expression_literal.h"
#include "expression_tabchar.h"
#include "expression_complex.h"

namespace bloc
{

std::string TupleExpression::readableTuple(const Tuple &c, Context& ctx)
{
  std::string sb;
  for (int i = 0; i < c.size(); ++i)
  {
    if (i > 0)
      sb.append(", ");
    switch (c[i]->type(ctx).major())
    {
    case Type::BOOLEAN:
      sb.append(BooleanExpression::readableBoolean(c[i]->boolean(ctx)));
      break;
    case Type::INTEGER:
      sb.append(IntegerExpression::readableInteger(c[i]->integer(ctx)));
      break;
    case Type::NUMERIC:
      sb.append(NumericExpression::readableNumeric(c[i]->numeric(ctx)));
      break;
    case Type::LITERAL:
      sb.append(LiteralExpression::readableLiteral(c[i]->literal(ctx)));
      break;
    case Type::COMPLEX:
      sb.append(c[i]->toString(ctx));
      break;
    case Type::TABCHAR:
      sb.append(c[i]->toString(ctx));
      break;
    default:
      break;
    }
  }
  return sb;
}

}

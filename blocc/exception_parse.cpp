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

#include "exception_parse.h"

namespace bloc
{

const char * ParseError::PARSE_ERROR[] =
{
  "EOF", /* EXC_PARSE_EOF */
  "Illegal operator in expression.",
  "%s is undefined.",
  "Expected numeric expression.",
  "Expected integer expression.",
  "Expected boolean expression.",
  "Expected complex expression.",
  "Expected literal expression.",
  "Expected bytes expression.",
  "Expected table expression.",
  "Invalid or non tuple expression.",
  "Type mismatch in expression: %s was expected.",
  "Limit reached of N-dimensional array.",
  "Bad item number: %s.",
  "Out of range.",
  "Bad type of argument calling function '%s'.",
  "Bad number of argument calling function '%s'.",
  "Method '%s' not applicable.",
  "Bad type of argument calling method '%s'.",
  "Bad number of argument calling method '%s'.",
  "Invalid syntax calling method '%s'.",
  "Unrecognized statement.",
  "Unrecognized function.",
  "Unexpected symbol '%s' in expression.",
  "Mismatched parenthesis.",
  "Extra input '%s' beyond statement end.",
  "Extra input '%s' beyond expression end.",
  "Missing keyword %s.",
  "Keyword '%s' cannot be used as variable name.",
  "Import module '%s' failed.",
  "The OPAQUE expression must be stored in a variable before calling a method or accessing an item.",
  "%s", /* EXC_PARSE_MESSAGE_S */
};

}

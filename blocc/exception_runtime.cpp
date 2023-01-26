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

#include "exception_runtime.h"

namespace bloc
{

const char * RuntimeError::RUNTIME_ERROR[] =
{
  "%s", /* user defined */
  "Internal error (%s).",
  "Not implemented yet.",
  "Illegal operation.",
  "%s is undefined.",
  "Invalid or non numeric expression.",
  "Invalid or non integer expression.",
  "Invalid or non boolean expression.",
  "Invalid or non complex expression.",
  "Invalid or non literal expression.",
  "Invalid or non bytes expression.",
  "Invalid or non table expression.",
  "Invalid or non tuple expression.",
  "Failed to initialize object '%s'.",
  "Failed to execute method '%s'.",
  "Method called from wrong type, expected type is '%s'.",
  "Limit reached of N-dimensional array.",
  "Out of range.",
  "Index out of range: %s.",
  "Divide by zero.",
  "Failed to convert to numeric value.",
  "Type mismatch in expression: %s was expected.",
  "Bad type of argument calling function '%s'.",
  "Bad type of argument calling method '%s'.",
};

RuntimeError::THROWABLE RuntimeError::THROWABLES[] =
{
  { EXC_RT_OUT_OF_RANGE,    "OUT_OF_RANGE" },
  { EXC_RT_DIVIDE_BY_ZERO,  "DIVIDE_BY_ZERO" },
};

EXC_RT RuntimeError::findThrowable(const std::string& keyword)
{
  for (int i = 0; i < sizeof(THROWABLES); ++i)
  {
    if (keyword == THROWABLES[i].keyword)
      return THROWABLES[i].no;
  }
  return EXC_RT_USER_S;
}

bool RuntimeError::throwable(EXC_RT no)
{
  for (int i = 0; i < sizeof(THROWABLES); ++i)
  {
    if (no == THROWABLES[i].no)
      return true;
  }
  return false;
}

}

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

#include "operator.h"

namespace bloc
{

const char * Operator::OPVALS[] = {
  "",         "+",        "-",        "*",        "/",
  "power",    "-",        "+",        "%",        "",
  "&",        "|",        "^",        "~",        "",
  "<<",       ">>",       ".",        "",         "",
  "==",       "!=",       "<",        "<=",       ">",
  ">=",       "",         "",         "",         "",
  "and",      "or",       "xor",      "not",      "",
  "matches",
};

std::set<Operator::OP> Operator::operatorSet()
{
  std::set<OP> lst;
  for (int i = 0; i < (sizeof (OPVALS) / sizeof (char*)); ++i)
    if (OPVALS[i][0] != '\0')
      lst.insert((OP)i);
  return lst;
}

}

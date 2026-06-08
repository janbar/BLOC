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

#include "string_reader.h"

#include <cstring>

namespace bloc
{

int StringReader::read(Parser *, char * buf, int max_size)
{
  int c = 0;
  std::string::iterator p = _text.begin() + _pos;
  while (p != _text.end() && c < max_size)
  {
    ++_pos;
    // discard cr to fix source formated msdos
    if (*p != '\r')
      buf[c++] = *p;
    if (*p == '\n')
      break;
    ++p;
  }
  return c;
}

}

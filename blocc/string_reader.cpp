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

int StringReader::read(char * buf, int max_size)
{
  size_t r = (max_size  > _text.size() - _pos ? _text.size() - _pos : max_size);
  ::memcpy(buf, &_text[_pos], r);
  _pos += r;
  return (int) r;
}

void StringReader::token_read(void * string_reader, char * buf, int * len, int max_size)
{
  StringReader * sr = static_cast<StringReader*>(string_reader);
  *len = sr->read(buf, max_size);
}

}

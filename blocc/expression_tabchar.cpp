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

#include "expression_tabchar.h"

namespace bloc
{

const Type& TabcharExpression::type_static = Type(Type::TABCHAR);

void TabcharExpression::outputTabchar(const TabChar& v, FILE * _stdout, unsigned max_lines /*= (-1)*/)
{
  unsigned idx = 0, lno = 0;
  size_t sz = v.size();
  while (idx < sz && lno++ < max_lines)
  {
    fprintf(_stdout, "%08X:  ", idx);
    char str[24];
    int i;
    for (i = 0; i < 16 && idx < sz; ++i, ++idx)
    {
      fprintf(_stdout, "%02x ", (unsigned char) v[idx]);
      str[i] = (v[idx] > 32 && v[idx] < 127 ? v[idx] : '.');
    }
    str[i] = '\0';
    while (i++ < 16) fputs("   ", _stdout);
    fputc(' ', _stdout);
    fputs(str, _stdout);
    fputc('\n', _stdout);
  }
}

}

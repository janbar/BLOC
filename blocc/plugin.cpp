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

#include "plugin.h"

#include <cstring>

namespace bloc
{
namespace plugin
{

#define NA Type::NO_TYPE
static const Type::TypeMajor __match_type_code__[] = {
  NA, NA, NA, NA, NA, NA, NA, NA, NA, NA, NA, NA, NA, NA, NA, NA,
  NA, NA, NA, NA, NA, NA, NA, NA, NA, NA, NA, NA, NA, NA, NA, NA,
  NA, NA, NA, NA, NA, NA, NA, NA, NA, NA, NA, NA, NA, NA, NA, NA,
  NA, NA, NA, NA, NA, NA, NA, NA, NA, NA, NA, NA, NA, NA, NA, NA,

  NA, NA, Type::BOOLEAN, Type::COMPLEX, NA, NA, NA, NA,
  NA, Type::INTEGER, NA, NA, Type::LITERAL, NA, Type::NUMERIC, NA,

  NA, NA, NA, NA, NA, NA, NA, NA,
  Type::TABCHAR, NA, NA, NA, NA, NA, NA, NA,

  NA, NA, Type::BOOLEAN, Type::COMPLEX, NA, NA, NA, NA,
  NA, Type::INTEGER, NA, NA, Type::LITERAL, NA, Type::NUMERIC, NA,

  NA, NA, NA, NA, NA, NA, NA, NA,
  Type::TABCHAR, NA, NA, NA, NA, NA, NA, NA,

  NA, NA, NA, NA, NA, NA, NA, NA, NA, NA, NA, NA, NA, NA, NA, NA,
  NA, NA, NA, NA, NA, NA, NA, NA, NA, NA, NA, NA, NA, NA, NA, NA,
  NA, NA, NA, NA, NA, NA, NA, NA, NA, NA, NA, NA, NA, NA, NA, NA,
  NA, NA, NA, NA, NA, NA, NA, NA, NA, NA, NA, NA, NA, NA, NA, NA,
  NA, NA, NA, NA, NA, NA, NA, NA, NA, NA, NA, NA, NA, NA, NA, NA,
  NA, NA, NA, NA, NA, NA, NA, NA, NA, NA, NA, NA, NA, NA, NA, NA,
  NA, NA, NA, NA, NA, NA, NA, NA, NA, NA, NA, NA, NA, NA, NA, NA,
  NA, NA, NA, NA, NA, NA, NA, NA, NA, NA, NA, NA, NA, NA, NA, NA,
};
#undef NA

TupleDecl::Decl
make_decl(PLUGIN_DECL decl_def, Type::TypeMinor type_id)
{
  Tuple::Decl decl;
  unsigned len = ::strlen(decl_def);
  unsigned cnt = 1;
  for (int i = 0; i < len && cnt < TUPLE_MAX_ITEMS; ++i)
  {
    Type::TypeMajor m = __match_type_code__[(unsigned char) decl_def[i]];
    if (m == Type::NO_TYPE)
      continue;
    cnt++;
    decl.push_back(Type(m, (m == Type::COMPLEX ? type_id : 0), 0));
  }
  return decl;
}

Type
make_type(PLUGIN_TYPE type_def, Type::TypeMinor type_id)
{
  return make_decl(type_def.decl, type_id).make_type(type_def.ndim);
}

} /* namespace import */
} /* namespace bloc */

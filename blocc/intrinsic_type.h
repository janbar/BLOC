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

#ifndef INTRINSIC_TYPE_H_
#define INTRINSIC_TYPE_H_

#include <cstdint>
#include <string>
#include <vector>

#define TYPE_MINOR_MAX      0xffff
#define TYPE_LEVEL_MAX      0xff

namespace bloc
{

class Type
{
public:

  typedef uint16_t TypeMinor;

  typedef uint8_t TypeLevel;

  enum TypeMajor : uint8_t
  {
    NO_TYPE   = 0,
    BOOLEAN,
    INTEGER,
    NUMERIC,
    LITERAL,
    COMPLEX,
    TABCHAR,
    ROWTYPE,
  };

  virtual ~Type() { }

  Type() : _major(NO_TYPE) { }

  Type(TypeMajor type) : _major(type) { } /* converting ctor */

  Type(TypeMajor major, TypeMinor minor, TypeLevel level = 0)
          : _minor(minor), _level(level), _major(major) { }

  bool operator==(TypeMajor t) const { return t == _major; }
  bool operator!=(TypeMajor t) const { return !(*this == t); }

  bool operator==(const Type& t) const
  { return (t._major == _major && t._level == _level && t._minor == _minor); }
  bool operator!=(const Type& t) const { return !(*this == t); }

  TypeMajor major() const { return _major; }
  TypeLevel level() const { return _level; }
  TypeMinor minor() const { return _minor; }

  Type levelUp() const { return Type(_major, _minor, _level + 1); }
  Type levelDown() const { return Type(_major, _minor, _level - 1); }

  std::string typeName() const { return typeName(typeName(_major)); }

  std::string typeName(const char * nickname) const
  {
    std::string fn;
    if (_level > 0)
      fn.append(_level, '[').append(nickname).append(_level, ']');
    else
      fn.assign(typeName(_major));
    return fn;
  }

  static const char * typeName(TypeMajor type)
  {
    switch (type)
    {
    case NO_TYPE:
      return "void";
    case BOOLEAN:
      return "boolean";
    case INTEGER:
      return "integer";
    case NUMERIC:
      return "decimal";
    case LITERAL:
      return "string";
    case COMPLEX:
      return "object";
    case TABCHAR:
      return "bytes";
    case ROWTYPE:
      return "tuple";
    }
    return "?";
  }

protected:
  TypeMinor _minor = 0; ///< id of the extended type of COMPLEX or ROWTYPE
  TypeLevel _level = 0; ///< nb of dimension, zero for the base type
  TypeMajor _major;     ///< the base type
};

}

#endif /* INTRINSIC_TYPE_H_ */


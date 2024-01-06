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
    BOOLEAN   = 1,
    INTEGER,
    NUMERIC,
    LITERAL,
    COMPLEX,
    TABCHAR,
    ROWTYPE,
    POINTER,
  };

  virtual ~Type() { }

  Type() : _major(NO_TYPE) { }

  Type(const Type& t) = default;
  Type& operator=(const Type& t) = default;

  /* converting ctor */
  Type(TypeMajor type)
          : _major(type) { }

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
      fn.assign(nickname);
    return fn;
  }

  constexpr static const char * STR_NO_TYPE = "undefined";
  constexpr static const char * STR_BOOLEAN = "boolean";
  constexpr static const char * STR_INTEGER = "integer";
  constexpr static const char * STR_NUMERIC = "decimal";
  constexpr static const char * STR_LITERAL = "string";
  constexpr static const char * STR_COMPLEX = "object";
  constexpr static const char * STR_TABCHAR = "bytes";
  constexpr static const char * STR_ROWTYPE = "tuple";
  constexpr static const char * STR_POINTER = "pointer";

  static const char * typeName(TypeMajor type)
  {
    switch (type)
    {
    case NO_TYPE:
      return STR_NO_TYPE;
    case BOOLEAN:
      return STR_BOOLEAN;
    case INTEGER:
      return STR_INTEGER;
    case NUMERIC:
      return STR_NUMERIC;
    case LITERAL:
      return STR_LITERAL;
    case COMPLEX:
      return STR_COMPLEX;
    case TABCHAR:
      return STR_TABCHAR;
    case ROWTYPE:
      return STR_ROWTYPE;
    case POINTER:
      return STR_POINTER;
    }
    return "?";
  }

  static Type nameType(const std::string& text)
  {
    if (text == STR_BOOLEAN)
      return Type::BOOLEAN;
    if (text == STR_INTEGER)
      return Type::INTEGER;
    if (text == STR_NUMERIC)
      return Type::NUMERIC;
    if (text == STR_LITERAL)
      return Type::LITERAL;
    if (text == STR_COMPLEX)
      return Type::COMPLEX;
    if (text == STR_TABCHAR)
      return Type::TABCHAR;
    if (text == STR_ROWTYPE)
      return Type::ROWTYPE;
    if (text == STR_POINTER)
      return Type::POINTER;
    return Type::NO_TYPE;
  }

protected:
  TypeMinor _minor = 0; ///< id of the extended type of COMPLEX or ROWTYPE
  TypeLevel _level = 0; ///< nb of dimension, zero for the base type
  TypeMajor _major;     ///< the base type
};

}

#endif /* INTRINSIC_TYPE_H_ */


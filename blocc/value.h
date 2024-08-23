/*
 *      Copyright (C) 2023 Jean-Luc Barriere
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

#ifndef VALUE_H_
#define VALUE_H_

#include "intrinsic_type.h"
#include "declspec.h"
#include "exception_runtime.h"

#include <string>
#include <cinttypes>
#include <cstdio>

namespace bloc
{

class Complex;
class Tuple;
class Collection;

typedef bool Bool;
typedef int64_t Integer;
typedef double Numeric;
typedef struct { Numeric a; Numeric b; } Imaginary;
typedef std::string Literal;
typedef std::vector<char> TabChar;

class Value final
{
  /* value payload */
  typedef union { bool b; int64_t i; double d; void * p; } payload;
  mutable payload _value;

  /* value type */
  Type _type;

  /* value flags */
  enum flag { NOTNULL = 0x1, LVALUE = 0x2 };
  int _flags = 0;

  void _clear() noexcept;

public:
  LIBBLOC_API static const char * STR_TRUE;
  LIBBLOC_API static const char * STR_FALSE;
  LIBBLOC_API static const char * STR_NIL;

  /* undefined */
  LIBBLOC_API static const Type& type_no_type;

  /* the types */
  LIBBLOC_API static const Type& type_boolean;
  LIBBLOC_API static const Type& type_integer;
  LIBBLOC_API static const Type& type_numeric;
  LIBBLOC_API static const Type& type_imaginary;
  LIBBLOC_API static const Type& type_literal;
  LIBBLOC_API static const Type& type_complex;
  LIBBLOC_API static const Type& type_tabchar;
  LIBBLOC_API static const Type& type_rowtype;
  LIBBLOC_API static const Type& type_pointer;

  bool operator==(const Value& v) const { return (this->_value.p == v._value.p); }
  bool operator!=(const Value& v) const { return !(*this == v); }

  Value() : _type(Type::NO_TYPE) { }
  explicit Value(const Type& type) : _type(type) { }
  explicit Value(Bool v)    : _type(Type::BOOLEAN), _flags(NOTNULL) { _value.b = v; }
  explicit Value(Integer v) : _type(Type::INTEGER), _flags(NOTNULL) { _value.i = v; }
  explicit Value(Numeric v) : _type(Type::NUMERIC), _flags(NOTNULL) { _value.d = v; }
  explicit Value(Imaginary * v);
  explicit Value(Literal * v);
  explicit Value(TabChar * v);
  explicit Value(Complex * v);
  explicit Value(Tuple * v);
  explicit Value(Collection * v);
  explicit Value(Value * v);

  ~Value() { if (!isNull()) _clear(); }

  Value(const Value& v) = delete;
  Value& operator=(const Value& v) = delete;

  /* move ctor */
  Value(Value&& v) noexcept;

  /* assign */
  Value& operator=(Value&& v) noexcept;

  /* swap */
  void swap(Value& v) noexcept;

  /* move */
  void swap(Value&& v) noexcept;

  /* clone */
  Value clone() const noexcept;

  payload get() { return _value; }

  bool isNull() const { return (_flags & NOTNULL) == 0; }
  bool lvalue() const { return (_flags & LVALUE) != 0; }

#define _bloc_vcast_1(T)    (static_cast<T*>(_value.p))
#define _bloc_vcast_2(T, I) (static_cast<T*>(I._value.p))
#define _bloc_vcast_null(T) (isNull() ? nullptr : static_cast<T*>(_value.p))

  Value& to_lvalue(bool b)
  {
    if (b)
      _flags |= LVALUE;
    else
      _flags &= ~LVALUE;
    return *this;
  }

  const Type& type() const
  {
    return _type;
  }

  Bool * boolean()
  {
    if (_type != Type::BOOLEAN || _type.level())
      throw RuntimeError(EXC_RT_NOT_BOOLEAN);
    return (isNull() ? nullptr : &_value.b);
  }

  Integer * integer()
  {
    if (_type != Type::INTEGER || _type.level())
      throw RuntimeError(EXC_RT_NOT_INTEGER);
    return (isNull() ? nullptr : &_value.i);
  }

  Numeric * numeric()
  {
    if (_type != Type::NUMERIC || _type.level())
      throw RuntimeError(EXC_RT_NOT_NUMERIC);
    return (isNull() ? nullptr : &_value.d);
  }

  Imaginary * imaginary()
  {
    if (_type != Type::IMAGINARY || _type.level())
      throw RuntimeError(EXC_RT_NOT_IMAGINARY);
    return _bloc_vcast_null(Imaginary);
  }

  Literal * literal()
  {
    if (_type != Type::LITERAL || _type.level())
      throw RuntimeError(EXC_RT_NOT_LITERAL);
    return _bloc_vcast_null(Literal);
  }

  TabChar * tabchar()
  {
    if (_type != Type::TABCHAR || _type.level())
      throw RuntimeError(EXC_RT_NOT_TABCHAR);
    return _bloc_vcast_null(TabChar);
  }

  Collection * collection()
  {
    if (_type.level() == 0)
      throw RuntimeError(EXC_RT_NOT_COLLECT);
    return _bloc_vcast_null(Collection);
  }

  Tuple * tuple()
  {
    if (_type != Type::ROWTYPE || _type.level())
      throw RuntimeError(EXC_RT_NOT_ROWTYPE);
    return _bloc_vcast_null(Tuple);
  }

  Complex * complex()
  {
    if (_type != Type::COMPLEX || _type.level())
      throw RuntimeError(EXC_RT_NOT_COMPLEX);
    return _bloc_vcast_null(Complex);
  }

  Value * value()
  {
    if (_type != Type::POINTER || _type.level())
      throw RuntimeError(EXC_RT_NOT_POINTER);
    return _bloc_vcast_null(Value);
  }

  Value& deref_value() noexcept
  {
    Value * v = this;
    while (!v->isNull() && v->_type == Type::POINTER)
      v = static_cast<Value*>(v->_value.p);
    return *v;
  }

  const Value& deref_value() const noexcept
  {
    const Value * v = this;
    while (!v->isNull() && v->_type == Type::POINTER)
      v = static_cast<Value*>(v->_value.p);
    return *v;
  }

  std::string toString() const;

  std::string typeName() const;

  /* readables */

  static std::string readableBoolean(Bool& b);
  static std::string readableInteger(Integer& l);
  static std::string readableNumeric(Numeric& d);
  static std::string readableImaginary(Imaginary& i);
  static std::string readableLiteral(Literal& s);
  static std::string readableTabchar(TabChar& t);
  static std::string readableComplex(Complex& c);
  static std::string readableTuple(Tuple& c);

  static void outputTabchar(const TabChar& v, FILE * _stdout, unsigned max_lines = (-1));

  static Value parseLiteral(const std::string& text);
  static Value parseInteger(const std::string& text, int base = 10);
  static Value parseNumeric(const std::string& text);
};

}

#endif /* VALUE_H_ */

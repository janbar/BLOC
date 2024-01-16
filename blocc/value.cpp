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

#include "value.h"
#include "complex.h"
#include "tuple.h"
#include "collection.h"
#include "plugin_manager.h"
#include "blocc/collection.h"
#include "debug.h"

#include <cassert>
#include <cinttypes>
#include <string>

//#define DEBUG_VALUE

namespace bloc
{

const char * Value::STR_TRUE = "TRUE";
const char * Value::STR_FALSE = "FALSE";
const char * Value::STR_NIL = "null";

const Type& Value::type_no_type = Type(Type::NO_TYPE);

const Type& Value::type_boolean = Type(Type::BOOLEAN);
const Type& Value::type_integer = Type(Type::INTEGER);
const Type& Value::type_numeric = Type(Type::NUMERIC);
const Type& Value::type_literal = Type(Type::LITERAL);
const Type& Value::type_complex = Type(Type::COMPLEX);
const Type& Value::type_tabchar = Type(Type::TABCHAR);
const Type& Value::type_rowtype = Type(Type::ROWTYPE);
const Type& Value::type_pointer = Type(Type::POINTER);

void Value::_clear() noexcept
{
#ifdef DEBUG_VALUE
  DBG(DBG_DEBUG, "%s line %d\n", __PRETTY_FUNCTION__, __LINE__);
#endif
  _flags &= ~NOTNULL;
  if (_type.level() > 0)
    delete cast(Collection);
  else
    switch (_type.major())
    {
    case Type::LITERAL:
      delete cast(Literal);
      break;
    case Type::COMPLEX:
      delete cast(Complex);
      break;
    case Type::TABCHAR:
      delete cast(TabChar);
      break;
    case Type::ROWTYPE:
      delete cast(Tuple);
      break;
    default:
      break;
    }
}

Value::Value(Literal * v) : _type(Type::LITERAL)
{
#ifdef DEBUG_VALUE
  DBG(DBG_DEBUG, "%s line %d\n", __PRETTY_FUNCTION__, __LINE__);
#endif
  if (v)
  {
    _value.p = v;
    _flags = NOTNULL;
  }
}

Value::Value(TabChar * v) : _type(Type::TABCHAR)
{
#ifdef DEBUG_VALUE
  DBG(DBG_DEBUG, "%s line %d\n", __PRETTY_FUNCTION__, __LINE__);
#endif
  if (v)
  {
    _value.p = v;
    _flags = NOTNULL;
  }
}

Value::Value(Complex * v) : _type(Type::COMPLEX)
{
#ifdef DEBUG_VALUE
  DBG(DBG_DEBUG, "%s line %d\n", __PRETTY_FUNCTION__, __LINE__);
#endif
  if (v)
  {
    _value.p = v;
    _type = v->complex_type();
    _flags = NOTNULL;
  }
}

Value::Value(Tuple * v) : _type(Type::ROWTYPE)
{
#ifdef DEBUG_VALUE
  DBG(DBG_DEBUG, "%s line %d\n", __PRETTY_FUNCTION__, __LINE__);
#endif
  if (v)
  {
    _value.p = v;
    _type = v->tuple_type();
    _flags = NOTNULL;
  }
}

Value::Value(Collection * v) : _type(type_no_type.levelUp())
{
#ifdef DEBUG_VALUE
  DBG(DBG_DEBUG, "%s line %d\n", __PRETTY_FUNCTION__, __LINE__);
#endif
  if (v)
  {
    _value.p = v;
    _type = v->table_type();
    _flags = NOTNULL;
  }
}

Value::Value(Value * v) : _type(Type::POINTER)
{
#ifdef DEBUG_VALUE
  DBG(DBG_DEBUG, "%s line %d\n", __PRETTY_FUNCTION__, __LINE__);
#endif
  if (v)
  {
    _value.p = v;
    _flags = NOTNULL;
  }
}

Value::Value(Value&& v) noexcept
: _value(v._value), _type(v._type), _flags(v._flags)
{
#ifdef DEBUG_VALUE
  DBG(DBG_DEBUG, "%s line %d\n", __PRETTY_FUNCTION__, __LINE__);
#endif
  /* move */
  v._flags = 0;
}

Value& Value::operator=(Value&& v) noexcept
{
#ifdef DEBUG_VALUE
  DBG(DBG_DEBUG, "%s line %d\n", __PRETTY_FUNCTION__, __LINE__);
#endif
  if (this == &v)
    return *this;
  if (!isNull())
    _clear();
  /*  move */
  _value = v._value;
  _type = v._type;
  _flags = v._flags;
  v._flags = 0;
  return *this;
}

void Value::swap(Value& v) noexcept
{
#ifdef DEBUG_VALUE
  DBG(DBG_DEBUG, "%s line %d\n", __PRETTY_FUNCTION__, __LINE__);
#endif
  if (this == &v)
    return;
  payload tmp_p = _value;
  Type tmp_t = _type;
  int tmp_f = _flags;
  _value = v._value;
  _type = v._type;
  _flags = v._flags;
  v._value = tmp_p;
  v._type = tmp_t;
  v._flags = tmp_f;
}

void Value::swap(Value&& v) noexcept
{
#ifdef DEBUG_VALUE
  DBG(DBG_DEBUG, "%s line %d\n", __PRETTY_FUNCTION__, __LINE__);
#endif
  if (this == &v)
    return;
  if (!isNull())
    _clear();
  /*  move */
  _value = v._value;
  _type = v._type;
  _flags = v._flags;
  v._flags = 0;
}

Value Value::clone() const noexcept
{
#ifdef DEBUG_VALUE
  DBG(DBG_DEBUG, "%s line %d\n", __PRETTY_FUNCTION__, __LINE__);
#endif
  Value c(_type);
  if (!isNull())
  {
    c._flags = NOTNULL;
    if (_type.level() > 0)
      c._value.p = new Collection(*cast(Collection));
    else
      switch (_type.major())
      {
      case Type::NO_TYPE:
        break;
      case Type::BOOLEAN:
        c._value.b = _value.b;
        break;
      case Type::INTEGER:
        c._value.i = _value.i;
        break;
      case Type::NUMERIC:
        c._value.d = _value.d;
        break;
      case Type::LITERAL:
        c._value.p = new Literal(*cast(Literal));
        break;
      case Type::COMPLEX:
        c._value.p = new Complex(*cast(Complex));
        break;
      case Type::TABCHAR:
        c._value.p = new TabChar(*cast(TabChar));
        break;
      case Type::ROWTYPE:
        c._value.p = new Tuple(*cast(Tuple));
        break;
      case Type::POINTER:
        c._value.p = _value.p;
        break;
      }
  }
  return c;
}

std::string Value::toString() const
{
#ifdef DEBUG_VALUE
  DBG(DBG_DEBUG, "%s line %d\n", __PRETTY_FUNCTION__, __LINE__);
#endif
  if (isNull())
  {
    if (_type == type_no_type)
      return std::string(Type::STR_NO_TYPE);
    return typeName().append(1, ' ').append(STR_NIL);
  }

  if (_type.level() > 0)
    return typeName().append(1, '[')
            .append(std::to_string(cast(Collection)->size()))
            .append(1, ']');

  switch (_type.major())
  {
  case Type::BOOLEAN:
    return typeName().append(1, ' ')
            .append(readableBoolean(_value.b));
  case Type::INTEGER:
    return typeName().append(1, ' ')
            .append(readableInteger(_value.i));
  case Type::NUMERIC:
    return typeName().append(1, ' ')
            .append(readableNumeric(_value.d));
  case Type::LITERAL:
    return typeName().append(1, '[')
            .append(std::to_string(cast(Literal)->size()))
            .append(1, ']');
  case Type::COMPLEX:
    return readableComplex(*cast(Complex));
  case Type::TABCHAR:
    return typeName().append(1, '[')
            .append(std::to_string(cast(TabChar)->size()))
            .append(1, ']');

  default:
    break;
  }
  return typeName();
}

std::string Value::typeName() const
{
#ifdef DEBUG_VALUE
  DBG(DBG_DEBUG, "%s line %d\n", __PRETTY_FUNCTION__, __LINE__);
#endif
  if (isNull())
    return _type.typeName();

  switch (_type.major())
  {
  case Type::COMPLEX:
    return _type.typeName(PluginManager::instance().plugged(_type.minor()).interface.name);
  case Type::ROWTYPE:
    if (_type.level() > 0)
      return _type.typeName(cast(Collection)->table_decl().tupleName());
    return _type.typeName(cast(Tuple)->tuple_decl().tupleName());
  default:
    return _type.typeName();
  }
}

/* static */

std::string Value::readableBoolean(Bool& b)
{
  return (b ? STR_TRUE : STR_FALSE);
}

std::string Value::readableInteger(Integer& l)
{
  return std::to_string(l);
}

std::string Value::readableNumeric(Numeric& d)
{
  char buf[32];
  snprintf(buf, sizeof(buf), "%.16g", d);
  return buf;
}

std::string Value::readableLiteral(Literal& s)
{
  std::string buf;
  buf.reserve(s.size() + 2);
  buf.append("\"");
  for (auto c : s)
  {
    switch(c)
    {
    case '\a':
      buf.append("\\a");
      break;
    case '\b':
      buf.append("\\b");
      break;
    case '\f':
      buf.append("\\f");
      break;
    case '\n':
      buf.append("\\n");
      break;
    case '\r':
      buf.append("\\r");
      break;
    case '\t':
      buf.append("\\t");
      break;
    case '\\':
      buf.append("\\\\");
      break;
    case '"':
      buf.append("\"\"");
      break;
    default:
      buf.push_back(c);
    }
  }
  buf.append("\"");
  return buf;
}

std::string Value::readableTabchar(TabChar& t)
{
  return std::string(Type::typeName(Type::TABCHAR)).append(1, '[')
          .append(std::to_string(t.size()))
          .append(1, ']');
}

std::string Value::readableComplex(Complex& c)
{
  std::string sb(PluginManager::instance().plugged(c.typeId()).interface.name);
  if (c.typeId())
  {
    char buf[24];
    snprintf(buf, sizeof(buf), "%p", c.instance());
    sb.append(1, '(');
    sb.append(buf);
    sb.append(1, ')');
  }
  return sb;
}

std::string Value::readableTuple(Tuple &c)
{
  std::string sb;
  for (int i = 0; i < c.size(); ++i)
  {
    if (i > 0)
      sb.append(", ");
    if (c[i].isNull())
      sb.append(Value::STR_NIL);
    else
      switch (c[i].type().major())
      {
      case Type::NO_TYPE:
        sb.append(STR_NIL);
        break;
      case Type::BOOLEAN:
        sb.append(Value::readableBoolean(*(c[i].boolean())));
        break;
      case Type::INTEGER:
        sb.append(Value::readableInteger(*(c[i].integer())));
        break;
      case Type::NUMERIC:
        sb.append(Value::readableNumeric(*(c[i].numeric())));
        break;
      case Type::LITERAL:
        sb.append(Value::readableLiteral(*(c[i].literal())));
        break;
      case Type::COMPLEX:
        sb.append(Value::readableComplex(*(c[i].complex())));
        break;
      case Type::TABCHAR:
        sb.append(Value::readableTabchar(*(c[i].tabchar())));
        break;
      case Type::POINTER:
        assert(0==1);
      default:
        break;
      }
  }
  return sb;
}

void Value::outputTabchar(const TabChar& v, FILE * _stdout, unsigned max_lines)
{
  unsigned idx = 0, lno = 0;
  size_t sz = v.size();
  while (idx < sz && lno < max_lines)
  {
    ++lno;
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

Value Value::parseLiteral(const std::string& text)
{
  Value val(new Literal());
  Literal * v = val.literal();
  v->reserve(text.size());
  /* start on enclosure, then finally drop last enclosure */
  char pc = 0;
  bool bs = false;
  for (auto c : text)
  {
    if (!bs)
    {
      bs = (c == '"');
    }
    else if (pc == '\\')
    {
      pc = 0;
      switch (c)
      {
      case 'a':
        v->push_back('\a');
        break;
      case 'b':
        v->push_back('\b');
        break;
      case 'f':
        v->push_back('\f');
        break;
      case 'n':
        v->push_back('\n');
        break;
      case 'r':
        v->push_back('\r');
        break;
      case 't':
        v->push_back('\t');
        break;
      case '\\':
        v->push_back('\\');
        break;
      default:
        pc = c;
      }
    }
    else if (pc == '"' && pc == c)
    {
      v->push_back(c);
      pc = 0;
    }
    else if (pc != 0)
    {
      v->push_back(pc);
      pc = c;
    }
    else
      pc = c;
  }
  return val;
}

Value Value::parseInteger(const std::string& text, int base)
{
  return Value(Integer(std::stoull(text, nullptr, base)));
}

Value Value::parseNumeric(const std::string& text)
{
  return Value(Numeric(std::stod(text)));
}

}

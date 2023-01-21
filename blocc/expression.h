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

#ifndef EXPRESSION_H_
#define EXPRESSION_H_

#include "intrinsic_type.h"
#include "complex.h"
#include "collection.h"
#include "tuple.h"
#include "exception_runtime.h"

#include <cinttypes>
#include <string>
#include <vector>

namespace bloc
{

/**
 * This is the base class for the simple expressions.
 *
 * Expressions are parsed by the class <b>ParseExpression</b> which creates
 * a parse tree using objects of type expression. The subclasses
 * <b>NumericExpression</b>,<b>LiteralExpression</b>,<b>VariableExpression</b>,
 * <b>BooleanExpression</b>,<b>OperatorExpression</b> and more hold specific
 * types of indivisable elements.
 *
 * See the class ParseExpression for the grammar and precidence rules.
 */

class Context;

typedef std::vector<char> TabChar;

class Expression
{

public:

  virtual ~Expression() { }

  Expression() { }

  virtual std::string unparse(Context& ctx) const = 0;

  virtual const Type& type(Context& ctx) const = 0;

  virtual bool boolean(Context& ctx) const
  {
    throw RuntimeError(EXC_RT_NOT_BOOLEAN);
  }

  virtual int64_t integer(Context& ctx) const
  {
    throw RuntimeError(EXC_RT_NOT_INTEGER);
  }

  virtual double numeric(Context& ctx) const
  {
    throw RuntimeError(EXC_RT_NOT_NUMERIC);
  }

  /**
   * Proves that the this expression provides an "rvalue".
   * The "rvalue" is temporary static expression, and its content can be
   * modified and forwarded throughout the duration of the running statement,
   * thus avoiding copying or new memory allocation.
   */
  virtual bool isRvalue(Context& ctx) const
  {
    return false;
  }

  virtual std::string& literal(Context& ctx) const
  {
    throw RuntimeError(EXC_RT_NOT_LITERAL);
  }

  virtual TabChar& tabchar(Context& ctx) const
  {
    throw RuntimeError(EXC_RT_NOT_TABCHAR);
  }

  virtual Collection& collection(Context& ctx) const
  {
    throw RuntimeError(EXC_RT_NOT_COLLECT);
  }

  virtual Tuple& tuple(Context& ctx) const
  {
    throw RuntimeError(EXC_RT_NOT_ROWTYPE);
  }

  virtual const Tuple::Decl& tuple_decl(Context& ctx) const
  {
    return Tuple::no_decl;
  }

  virtual Complex complex(Context& ctx) const
  {
    throw RuntimeError(EXC_RT_NOT_COMPLEX);
  }

  virtual std::string toString(Context& ctx) const
  {
    return "undefined";
  }

  virtual std::string typeName(Context& ctx) const
  {
    return type(ctx).typeName();
  }
};

}

#endif /* EXPRESSION_H_ */

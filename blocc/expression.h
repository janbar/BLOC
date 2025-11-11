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
#include "tuple_decl.h"
#include "value.h"
#include "exception_runtime.h"

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
class Value;
class Symbol;

class Expression
{

public:

  virtual ~Expression() { }

  Expression() { }

  virtual std::string unparse(Context& ctx) const = 0;
  virtual bool enclosed() const { return true; }

  /**
   * Returns the reference to the expression type
   */
  virtual const Type& type(Context& ctx) const = 0;


  virtual Value& value(Context& ctx) const = 0;

  /**
   * Returns true if the expression returns an immutable value, i.e constant.
   */
  virtual bool isConst() const { return false; }

  /**
   * Returns true if the expression is a symbolic variable or its accessor.
   */
  virtual bool isVarName() const { return false; }

  /**
   * Returns the source symbol allowing access to the value pointed to by
   * an expression. Defaut none.
   */
  virtual const Symbol * symbol() const { return nullptr; }

  virtual const TupleDecl::Decl& tuple_decl(Context& ctx) const
  {
    return TupleDecl::no_decl;
  }

  virtual std::string toString(Context& ctx) const { return "undefined"; }

  virtual std::string typeName(Context& ctx) const
  {
    return type(ctx).typeName();
  }
};

}

#endif /* EXPRESSION_H_ */

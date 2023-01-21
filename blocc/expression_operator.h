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

#ifndef EXPRESSION_OPERATOR_H
#define EXPRESSION_OPERATOR_H

#include "expression.h"
#include "declspec.h"

#include <string>
#include <set>

namespace bloc
{

/**
 * This class implements the logical, relational and mathematical operation.
 * The tokenizer will scan the input for one of the strings in OPVALS[] and
 * if it finds it, it returns a token of OP with its value being that of the
 * index of the string into the array. (which are convientiently mapped into
 * named constants).
 *
 * Parsing of the arguments to the function are left up to the parse method
 * in this class.
 *
 * Runtime errors:
 *     Divide by zero.
 *     Out of range.
 */

class OperatorExpression : public Expression {

private:
  int oper = OP_NIL;
  Expression * arg1 = nullptr;
  Expression * arg2 = nullptr;

public:

  /**
   * These are the valid operator types.
   */

  LIBBLOC_API static const char * OPVALS[];
  LIBBLOC_API static const char * HELPS[];

  enum OP {
    OP_NIL  = 0,
    OP_ADD  = 1,   // Addition '+'
    OP_SUB  = 2,   // Subtraction '-'
    OP_MUL  = 3,   // Multiplication '*'
    OP_DIV  = 4,   // Division '/'
    OP_EXP  = 5,   // Factor 'POWER','**'
    OP_NEG  = 6,   // Unary minus
    OP_POS  = 7,   // Unary positive
    OP_MOD  = 8,   // Modulo 'MOD','%'

    OP_AND  = 10,  // Bitwise AND '&'
    OP_IOR  = 11,  // Bitwise inclusive OR '|'
    OP_XOR  = 12,  // Bitwise exclusive OR '^'
    OP_NOT  = 13,  // Unary bitwise not '~'

    OP_POP  = 15,  // '<<'
    OP_PUS  = 16,  // '>>'
    OP_SEL  = 17,  // Member selection '.'

    OP_EQ   = 20,  // Equality '=='
    OP_NE   = 21,  // Inequality '!=','<>'
    OP_LT   = 22,  // Less than '<'
    OP_LE   = 23,  // Less than or equal '<='
    OP_GT   = 24,  // Greater than '>'
    OP_GE   = 25,  // Greater than or equal '>='

    OP_BAND = 30,  // Boolean AND 'AND','&&'
    OP_BIOR = 31,  // Boolean inclusive or 'OR','||'
    OP_BXOR = 32,  // Boolean exclusive or 'XOR'
    OP_BNOT = 33,  // Boolean negation 'NOT','!'

    OP_MATCH = 35, // String matches regular expression
  };

  virtual ~OperatorExpression();

  OperatorExpression() : Expression() { }

  /**
   * Create a unary expression.
   */
  OperatorExpression(int op, Expression * a) : Expression(), oper(op), arg2(a) { }

  /**
   * Create a binary expression.
   */
  OperatorExpression(int op, Expression * a, Expression * b) : Expression(), oper(op), arg1(a), arg2(b) { }

  std::string unparse(Context& ctx) const override;

  const Type& type(Context& ctx) const override;

  bool boolean(Context& ctx) const override;

  int64_t integer(Context& ctx) const override;

  /**
   * This method evaluates the expression in the context of the
   * passed in program. It throws runtime errors for things like
   * divide by zero or out of range.
   */
  double numeric(Context& ctx) const override;

  bool isRvalue(Context& ctx) const override { return true; }

  std::string& literal(Context& ctx) const override;

  std::string toString(Context& ctx) const override
  {
    return OPVALS[oper];
  }

  static std::set<OP> operatorSet();
};

}

#endif /* EXPRESSION_OPERATOR_H */

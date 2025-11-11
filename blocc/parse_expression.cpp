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

#include "parse_expression.h"
#include "exception_parse.h"
#include "parser.h"
#include "context.h"
#include "debug.h"
#include "expression_numeric.h"
#include "expression_integer.h"
#include "expression_literal.h"
#include "expression_boolean.h"
#include "expression_variable.h"
#include "expression_complex_ctor.h"
#include "expression_item.h"
#include "expression_builtin.h"
#include "expression_member.h"
#include "expression_functor.h"
#include "plugin_manager.h"
#include "functor_manager.h"

#include "operator/op_add.h"
#include "operator/op_and.h"
#include "operator/op_band.h"
#include "operator/op_bior.h"
#include "operator/op_bnot.h"
#include "operator/op_bxor.h"
#include "operator/op_div.h"
#include "operator/op_eq.h"
#include "operator/op_exp.h"
#include "operator/op_ge.h"
#include "operator/op_gt.h"
#include "operator/op_ior.h"
#include "operator/op_le.h"
#include "operator/op_lt.h"
#include "operator/op_match.h"
#include "operator/op_mod.h"
#include "operator/op_mul.h"
#include "operator/op_neg.h"
#include "operator/op_ne.h"
#include "operator/op_not.h"
#include "operator/op_pop.h"
#include "operator/op_pos.h"
#include "operator/op_pus.h"
#include "operator/op_sub.h"
#include "operator/op_xor.h"

#include <string>
#include <cassert>

namespace bloc
{

Expression * ParseExpression::expression(Parser& p, Context& ctx)
{
  ParseExpression pe(p, ctx, false);
  /* begin the parse by the precedence with the lowest priority */
  return pe.logic();
}

bool ParseExpression::typeChecking(Expression * exp, const Type& type, Parser& p, Context& ctx)
{
  const Type& exp_type = exp->type(ctx);
  return exp_type == type ||
          /* opaque */
          type == Type::NO_TYPE || exp_type == Type::NO_TYPE ||
          (exp_type.level() == type.level() && (
            /* type mixing */
            (exp_type == Type::NUMERIC && (type == Type::INTEGER || type == Type::IMAGINARY)) ||
            (exp_type == Type::INTEGER && (type == Type::NUMERIC || type == Type::IMAGINARY)) ||
            (exp_type == Type::IMAGINARY && (type == Type::NUMERIC || type == Type::INTEGER)) ||
            /* tuple opaque */
            (exp_type == Type::ROWTYPE && type == Type::ROWTYPE && (exp_type.minor() == 0 || type.minor() == 0))
          ));
}

Expression * ParseExpression::assertType(Expression * exp, const Type& type, Parser& p, Context& ctx, bool deleteOnFailure /*= true*/)
{
  if (typeChecking(exp, type, p, ctx))
    return exp;
  DBG(DBG_DEBUG, "%s : %s != %s\n", exp->unparse(ctx).c_str(), exp->typeName(ctx).c_str(), type.typeName().c_str());
  if (deleteOnFailure) delete exp;
  throw ParseError(EXC_PARSE_TYPE_MISMATCH_S, type.typeName().c_str(), p.front());
  return nullptr;
}

Expression * ParseExpression::assertTypeUniform(Expression * exp, const Type& type, Parser& p, Context& ctx, bool deleteOnFailure /*= true*/)
{
  const Type& exp_type = exp->type(ctx);
  if (exp_type == type ||
      /* opaque */
      type == Type::NO_TYPE || exp_type == Type::NO_TYPE)
    return exp;
  DBG(DBG_DEBUG, "%s : %s != %s\n", exp->unparse(ctx).c_str(), exp->typeName(ctx).c_str(), type.typeName().c_str());
  if (deleteOnFailure) delete exp;
  throw ParseError(EXC_PARSE_TYPE_MISMATCH_S, type.typeName().c_str(), p.front());
}

/**
 * Member operator for an element: is the precedence with the highgest priority
 * The member operator could be recursive.
 */
Expression * ParseExpression::member(Expression * exp)
{
  try
  {
    bool done = false;
    while (!done)
    {
      switch (p.front()->code)
      {
      case MemberExpression::OPERATOR:
        p.pop();
        exp = MemberExpression::parse(p, ctx, exp);
        break;
      case ItemExpression::OPERATOR:
        p.pop();
        exp = ItemExpression::parse(p, ctx, exp);
        break;
      default:
        done = true;
        break;
      }
    }
  }
  catch (ParseError& pe)
  {
    DBG(DBG_DEBUG, "exception %p at %s line %d\n", &pe, __PRETTY_FUNCTION__, __LINE__);
    throw;
  }
  return exp;
}

/**
 * Element: The precedence (1)
 * Not an operator, the element could be a constant, a variable, a function or
 * a new enclosed expression.
 */
Expression * ParseExpression::element()
{
  Expression * result = nullptr;

  try
  {
    TokenPtr t;
    t = p.pop();
    switch (t->code)
    {
    case TOKEN_INTEGER:
      try { return new IntegerExpression(Value::parseInteger(t->text)); }
      catch (std::out_of_range& e) { throw ParseError(EXC_PARSE_OUT_OF_RANGE, t); }
      break;
    case TOKEN_HEXANUM:
      try { return new IntegerExpression(Value::parseInteger(t->text, 16)); }
      catch (std::out_of_range& e) { throw ParseError(EXC_PARSE_OUT_OF_RANGE, t); }
      break;
    case TOKEN_DOUBLE:
    case TOKEN_FLOAT:
      try { return new NumericExpression(Value::parseNumeric(t->text)); }
      catch (std::out_of_range& e) { throw ParseError(EXC_PARSE_OUT_OF_RANGE, t); }
      break;
    case TOKEN_LITERALSTR:
      result = new LiteralExpression(Value::parseLiteral(t->text));
      return member(result);
    case TOKEN_KEYWORD:
      if (BuiltinExpression::findKeyword(t->text) != BuiltinExpression::unknown)
      {
        /* found a builtin function */
        p.push(t);
        result = BuiltinExpression::parse(p, ctx);
        return member(result);
      }
      else
      {
        unsigned type_id = PluginManager::instance().findModuleTypeId(t->text);
        if (type_id > 0)
        {
          /* found a CTOR of complex */
          result = ComplexCTORExpression::parse(p, ctx, type_id);
          return member(result);
        }
        /* finally it should be a symbol */
        if (p.front()->code == '(')
          result = FunctorExpression::parse(p, ctx, t);
        else
          result = VariableExpression::parse(p, ctx, t);
        return member(result);
      }
    case '(':
    {
      ParseExpression pe(p, ctx, true);
      result = pe.logic();
      t = p.pop();
      if (t->code != ')')
        throw ParseError(EXC_PARSE_MM_PARENTHESIS, t);
      return member(result);
    }
    default:
      throw ParseError(EXC_PARSE_UNEXPECTED_LEX_S, t->text.c_str(), t);
    }
    p.push(t);
  }
  catch (ParseError& pe)
  {
    DBG(DBG_DEBUG, "exception %p at %s line %d\n", &pe, __PRETTY_FUNCTION__, __LINE__);
    if (result) delete result;
    throw;
  }
  return result;
}

/**
 * Factor operator: The precedence (2)
 * The factor operator could be recursive, and it has the biggest priority over
 * all binary operators and itself.
 * The associativity is right->left
 */
Expression * ParseExpression::factor()
{
  Expression * result = nullptr;
  TokenPtr t;

  try
  {
    result = element();
    t = p.pop();
    switch (t->code)
    {
    case TOKEN_KEYWORD:
      if (t->text == Operator::OPVALS[Operator::OP_EXP])
        return new OpEXPExpression(assertType(result, Type::NUMERIC, p, ctx, false), assertType(factor(), Type::NUMERIC, p, ctx), enclosed);
      break;
    case TOKEN_POWER:
      return new OpEXPExpression(assertType(result, Type::NUMERIC, p, ctx, false), assertType(factor(), Type::NUMERIC, p, ctx), enclosed);
    default:
      break;
    }
    p.push(t);
  }
  catch (ParseError& pe)
  {
    DBG(DBG_DEBUG, "exception %p at %s line %d\n", &pe, __PRETTY_FUNCTION__, __LINE__);
    if (result) delete result;
    throw;
  }
  return result;
}

/**
 * Primary operator: The precedence (3)
 * The primary operator could be recursive. It starts an unary expression.
 * The associativity is right->left.
 */
Expression * ParseExpression::primary()
{
  TokenPtr t;

  try
  {
    t = p.pop();
    switch (t->code)
    {
    case TOKEN_KEYWORD:
      if (t->text == "not")
        return new OpBNOTExpression(assertType(factor(), Type::BOOLEAN, p, ctx), enclosed);
      break;
    case '!':
      return new OpBNOTExpression(assertType(factor(), Type::BOOLEAN, p, ctx), enclosed);
    case '~':
      return new OpNOTExpression(assertType(factor(), Type::NUMERIC, p, ctx), enclosed);
    case '-':
      return new OpNEGExpression(assertType(factor(), Type::NUMERIC, p, ctx), enclosed);
    case '+':
      return new OpPOSExpression(assertType(factor(), Type::NUMERIC, p, ctx), enclosed);
    default:
      break;
    }
    p.push(t);
  }
  catch (ParseError& pe)
  {
    DBG(DBG_DEBUG, "exception %p at %s line %d\n", &pe, __PRETTY_FUNCTION__, __LINE__);
    throw;
  }
  return factor();
}

/**
 * Term operator: The precedence (4)
 * The term operator could be recursive.
 * The associativity is left->right
 */
Expression * ParseExpression::term()
{
  Expression * result = nullptr;
  TokenPtr t;

  try
  {
    result = primary();
    bool done = false;
    while (!done)
    {
      t = p.pop();
      switch (t->code)
      {
      case '*':
        result = new OpMULExpression(assertType(result, Type::NUMERIC, p, ctx, false), assertType(primary(), Type::NUMERIC, p, ctx), enclosed);
        break;
      case '/':
        result = new OpDIVExpression(assertType(result, Type::NUMERIC, p, ctx, false), assertType(primary(), Type::NUMERIC, p, ctx), enclosed);
        break;
      case '%':
        result = new OpMODExpression(assertType(result, Type::NUMERIC, p, ctx, false), assertType(primary(), Type::NUMERIC, p, ctx), enclosed);
        break;
      default:
        done = true;
        break;
      }
    }
    p.push(t);
  }
  catch (ParseError& pe)
  {
    DBG(DBG_DEBUG, "exception %p at %s line %d\n", &pe, __PRETTY_FUNCTION__, __LINE__);
    if (result) delete result;
    throw;
  }
  return result;
}

/**
 * Sum operator: The precedence (5)
 * The sum operator could be recursive.
 * The associativity is left->right
 */
Expression * ParseExpression::sum()
{
  Expression * result = nullptr;
  TokenPtr t;

  try
  {
    result = term();
    bool done = false;
    while (!done)
    {
      t = p.pop();
      switch (t->code)
      {
      case '+':
        result = new OpADDExpression(result, assertType(term(), result->type(ctx), p, ctx), enclosed);
        break;
      case '-':
        result = new OpSUBExpression(assertType(result, Type::NUMERIC, p, ctx, false), assertType(term(), Type::NUMERIC, p, ctx), enclosed);
        break;
      default:
        done = true;
        break;
      }
    }
    p.push(t);
  }
  catch (ParseError& pe)
  {
    DBG(DBG_DEBUG, "exception %p at %s line %d\n", &pe, __PRETTY_FUNCTION__, __LINE__);
    if (result) delete result;
    throw;
  }
  return result;
}

/**
 * Bitwise left shift and right shift operator: The precedence (6)
 * The operator could be recursive.
 * The associativity is left->right
 */
Expression * ParseExpression::bitshift()
{
  Expression * result = nullptr;
  TokenPtr t;

  try
  {
    result = sum();
    bool done = false;
    while (!done)
    {
      t = p.pop();
      switch (t->code)
      {
      case TOKEN_POPLEFT:
        result =  new OpPOPExpression(assertTypeUniform(result, Type::INTEGER, p, ctx, false), assertTypeUniform(sum(), Type::INTEGER, p, ctx), enclosed);
        break;
      case TOKEN_PUSHRIGHT:
        result = new OpPUSExpression(assertTypeUniform(result, Type::INTEGER, p, ctx, false), assertTypeUniform(sum(), Type::INTEGER, p, ctx), enclosed);
        break;
      default:
        done = true;
        break;
      }
    }
    p.push(t);
  }
  catch (ParseError& pe)
  {
    DBG(DBG_DEBUG, "exception %p at %s line %d\n", &pe, __PRETTY_FUNCTION__, __LINE__);
    if (result) delete result;
    throw;
  }
  return result;
}

/**
 * Bitwise logical operator: The precedence (7)
 * The operator could be recursive.
 * The associativity is left->right
 */
Expression * ParseExpression::bitlogic()
{
  Expression * result = nullptr;
  TokenPtr t;

  try
  {
    result = bitshift();
    bool done = false;
    while (!done)
    {
      t = p.pop();
      switch (t->code)
      {
      case '&':
        result = new OpANDExpression(assertTypeUniform(result, Type::INTEGER, p, ctx, false), assertTypeUniform(bitshift(), Type::INTEGER, p, ctx), enclosed);
        break;
      case '|':
        result = new OpIORExpression(assertTypeUniform(result, Type::INTEGER, p, ctx, false), assertTypeUniform(bitshift(), Type::INTEGER, p, ctx), enclosed);
        break;
      case '^':
        result = new OpXORExpression(assertTypeUniform(result, Type::INTEGER, p, ctx, false), assertTypeUniform(bitshift(), Type::INTEGER, p, ctx), enclosed);
        break;
      default:
        done = true;
        break;
      }
    }
    p.push(t);
  }
  catch (ParseError& pe)
  {
    DBG(DBG_DEBUG, "exception %p at %s line %d\n", &pe, __PRETTY_FUNCTION__, __LINE__);
    if (result) delete result;
    throw;
  }
  return result;
}

/**
 * Relational operator: The precedence (8)
 * The operator CANNOT be recursive: (1==1==TRUE) is not allowed
 */
Expression * ParseExpression::relation()
{
  Expression * result = nullptr;
  TokenPtr t;

  try
  {
    result = bitlogic();
    t = p.pop();
    switch (t->code)
    {
    case TOKEN_ISNOTEQ:
      return new OpNEExpression(result, bitlogic(), enclosed);
    case TOKEN_ISEQUAL:
      return new OpEQExpression(result, bitlogic(), enclosed);
    case TOKEN_ISEQLESS:
      return new OpLEExpression(result, assertType(bitlogic(), result->type(ctx), p, ctx), enclosed);
    case '<':
      return new OpLTExpression(result, assertType(bitlogic(), result->type(ctx), p, ctx), enclosed);
    case TOKEN_ISEQMORE:
      return new OpGEExpression(result, assertType(bitlogic(), result->type(ctx), p, ctx), enclosed);
    case '>':
      return new OpGTExpression(result, assertType(bitlogic(), result->type(ctx), p, ctx), enclosed);
    case TOKEN_KEYWORD:
      if (t->text == Operator::OPVALS[Operator::OP_MATCH])
        return new OpMATCHExpression(assertType(result, Type::LITERAL, p, ctx, false), assertType(bitlogic(), Type::LITERAL, p, ctx), enclosed);
      break;
    default:
      break;
    }
    p.push(t);
  }
  catch (ParseError& pe)
  {
    DBG(DBG_DEBUG, "exception %p at %s line %d\n", &pe, __PRETTY_FUNCTION__, __LINE__);
    if (result) delete result;
    throw;
  }
  return result;
}

/**
 * Logical operator: The precedence (9)
 * The operator could be recursive.
 * The associativity is left->right
 */
Expression * ParseExpression::logic()
{
  Expression * result = nullptr;
  TokenPtr t;

  try
  {
    result = relation();
    bool done = false;
    while (!done)
    {
      t = p.pop();
      switch (t->code)
      {
      case TOKEN_KEYWORD:
        if (t->text == Operator::OPVALS[Operator::OP_BAND])
          result = new OpBANDExpression(assertType(result, Type::BOOLEAN, p, ctx, false), assertType(relation(), Type::BOOLEAN, p, ctx), enclosed);
        else if (t->text == Operator::OPVALS[Operator::OP_BIOR])
          result = new OpBIORExpression(assertType(result, Type::BOOLEAN, p, ctx, false), assertType(relation(), Type::BOOLEAN, p, ctx), enclosed);
        else if (t->text == Operator::OPVALS[Operator::OP_BXOR])
          result = new OpBXORExpression(assertType(result, Type::BOOLEAN, p, ctx, false), assertType(relation(), Type::BOOLEAN, p, ctx), enclosed);
        else
          done = true;
        break;
      case TOKEN_AND:
        result = new OpBANDExpression(assertType(result, Type::BOOLEAN, p, ctx, false), assertType(relation(), Type::BOOLEAN, p, ctx), enclosed);
        break;
      case TOKEN_OR:
        result = new OpBIORExpression(assertType(result, Type::BOOLEAN, p, ctx, false), assertType(relation(), Type::BOOLEAN, p, ctx), enclosed);
        break;
      default:
        done = true;
        break;
      }
    }
    p.push(t);
  }
  catch (ParseError& pe)
  {
    DBG(DBG_DEBUG, "exception %p at %s line %d\n", &pe, __PRETTY_FUNCTION__, __LINE__);
    if (result) delete result;
    throw;
  }
  return result;
}

}

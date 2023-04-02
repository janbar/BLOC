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
#include "expression_operator.h"
#include "expression_complex_ctor.h"
#include "expression_item.h"
#include "expression_builtin.h"
#include "expression_member.h"
#include "expression_functor.h"
#include "plugin_manager.h"
#include "functor_manager.h"

#include <string>
#include <cassert>

namespace bloc
{

Expression * ParseExpression::expression(Parser& p, Context& ctx)
{
  ParseExpression pe(p, ctx);
  /* begin the parse by the precedence with the lowest priority */
  return pe.logic();
}

bool ParseExpression::typeChecking(Expression * exp, const Type& type, Parser& p, Context& ctx)
{
  const Type& exp_type = exp->type(ctx);
  return !p.semantic() || exp_type == type ||
          /* opaque */
          type == Type::NO_TYPE || exp_type == Type::NO_TYPE ||
          /* type mixing */
          (exp_type.level() == type.level() && (
            (exp_type == Type::NUMERIC && type == Type::INTEGER) ||
            (exp_type == Type::INTEGER && type == Type::NUMERIC)
          ));
}

Expression * ParseExpression::assertType(Expression * exp, const Type& type, Parser& p, Context& ctx, bool deleteOnFailure /*= true*/)
{
  if (typeChecking(exp, type, p, ctx))
    return exp;
  if (deleteOnFailure) delete exp;
  throw ParseError(EXC_PARSE_TYPE_MISMATCH_S, type.typeName().c_str());
  return nullptr;
}

Expression * ParseExpression::assertTypeUniform(Expression * exp, const Type& type, Parser& p, Context& ctx, bool deleteOnFailure /*= true*/)
{
  const Type& exp_type = exp->type(ctx);
  if (!p.semantic() || exp_type == type ||
      /* opaque */
      type == Type::NO_TYPE || exp_type == Type::NO_TYPE)
    return exp;
  if (deleteOnFailure) delete exp;
  throw ParseError(EXC_PARSE_TYPE_MISMATCH_S, type.typeName().c_str());
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
      try { return IntegerExpression::parse(t->text); }
      catch (std::out_of_range& e) { throw ParseError(EXC_PARSE_OUT_OF_RANGE); }
      break;
    case TOKEN_HEXANUM:
      try { return IntegerExpression::parse(t->text, 16); }
      catch (std::out_of_range& e) { throw ParseError(EXC_PARSE_OUT_OF_RANGE); }
      break;
    case TOKEN_DOUBLE:
    case TOKEN_FLOAT:
      try { return NumericExpression::parse(t->text); }
      catch (std::out_of_range& e) { throw ParseError(EXC_PARSE_OUT_OF_RANGE); }
      break;
    case TOKEN_LITERALSTR:
      result = LiteralExpression::parse(t->text);
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
        /* all names are declared in upper case, so now transform the keyword */
        std::transform(t->text.begin(), t->text.end(), t->text.begin(), ::toupper);
        if (FunctorManager::instance().exists(t->text))
          result = FunctorExpression::parse(p, ctx, t->text);
        else
          result = VariableExpression::parse(p, ctx, t->text);
        return member(result);
      }
    case '(':
      result = expression(p, ctx);
      t = p.pop();
      if (t->code != ')')
        throw ParseError(EXC_PARSE_MM_PARENTHESIS);
      return member(result);
    default:
      throw ParseError(EXC_PARSE_UNEXPECTED_LEX_S, t->text.c_str());
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
 * Primary operator: The precedence (2)
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
        return new OperatorExpression(OperatorExpression::OP_BNOT, assertType(primary(), Type::BOOLEAN, p, ctx));
      break;
    case '!':
      return new OperatorExpression(OperatorExpression::OP_BNOT, assertType(primary(), Type::BOOLEAN, p, ctx));
    case '~':
      return new OperatorExpression(OperatorExpression::OP_NOT, assertType(primary(), Type::NUMERIC, p, ctx));
    case '-':
      return new OperatorExpression(OperatorExpression::OP_NEG, assertType(primary(), Type::NUMERIC, p, ctx));
    case '+':
      return new OperatorExpression(OperatorExpression::OP_POS, assertType(primary(), Type::NUMERIC, p, ctx));
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
  return element();
}

/**
 * Factor operator: The precedence (3)
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
    result = primary();
    t = p.pop();
    switch (t->code)
    {
    case TOKEN_KEYWORD:
      if (t->text == OperatorExpression::OPVALS[OperatorExpression::OP_EXP])
        return new OperatorExpression(OperatorExpression::OP_EXP, assertType(result, Type::NUMERIC, p, ctx, false), assertType(factor(), Type::NUMERIC, p, ctx));
      break;
    case TOKEN_POWER:
      return new OperatorExpression(OperatorExpression::OP_EXP, assertType(result, Type::NUMERIC, p, ctx, false), assertType(factor(), Type::NUMERIC, p, ctx));
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
    result = factor();
    bool done = false;
    while (!done)
    {
      t = p.pop();
      switch (t->code)
      {
      case '*':
        result = new OperatorExpression(OperatorExpression::OP_MUL, assertType(result, Type::NUMERIC, p, ctx, false), assertType(factor(), Type::NUMERIC, p, ctx));
        break;
      case '/':
        result = new OperatorExpression(OperatorExpression::OP_DIV, assertType(result, Type::NUMERIC, p, ctx, false), assertType(factor(), Type::NUMERIC, p, ctx));
        break;
      case '%':
        result = new OperatorExpression(OperatorExpression::OP_MOD, assertType(result, Type::NUMERIC, p, ctx, false), assertType(factor(), Type::NUMERIC, p, ctx));
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
        result = new OperatorExpression(OperatorExpression::OP_ADD, result, assertType(term(), result->type(ctx), p, ctx));
        break;
      case '-':
        result = new OperatorExpression(OperatorExpression::OP_SUB, assertType(result, Type::NUMERIC, p, ctx, false), assertType(term(), Type::NUMERIC, p, ctx));
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
        result =  new OperatorExpression(OperatorExpression::OP_POP, assertTypeUniform(result, Type::INTEGER, p, ctx, false), assertTypeUniform(sum(), Type::INTEGER, p, ctx));
        break;
      case TOKEN_PUSHRIGHT:
        result = new OperatorExpression(OperatorExpression::OP_PUS, assertTypeUniform(result, Type::INTEGER, p, ctx, false), assertTypeUniform(sum(), Type::INTEGER, p, ctx));
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
        result = new OperatorExpression(OperatorExpression::OP_AND, assertTypeUniform(result, Type::INTEGER, p, ctx, false), assertTypeUniform(bitshift(), Type::INTEGER, p, ctx));
        break;
      case '|':
        result = new OperatorExpression(OperatorExpression::OP_IOR, assertTypeUniform(result, Type::INTEGER, p, ctx, false), assertTypeUniform(bitshift(), Type::INTEGER, p, ctx));
        break;
      case '^':
        result = new OperatorExpression(OperatorExpression::OP_XOR, assertTypeUniform(result, Type::INTEGER, p, ctx, false), assertTypeUniform(bitshift(), Type::INTEGER, p, ctx));
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
      return new OperatorExpression(OperatorExpression::OP_NE, result, bitlogic());
    case TOKEN_ISEQUAL:
      return new OperatorExpression(OperatorExpression::OP_EQ, result, bitlogic());
    case TOKEN_ISEQLESS:
      return new OperatorExpression(OperatorExpression::OP_LE, result, assertType(bitlogic(), result->type(ctx), p, ctx));
    case '<':
      return new OperatorExpression(OperatorExpression::OP_LT, result, assertType(bitlogic(), result->type(ctx), p, ctx));
    case TOKEN_ISEQMORE:
      return new OperatorExpression(OperatorExpression::OP_GE, result, assertType(bitlogic(), result->type(ctx), p, ctx));
    case '>':
      return new OperatorExpression(OperatorExpression::OP_GT, result, assertType(bitlogic(), result->type(ctx), p, ctx));
    case TOKEN_KEYWORD:
      if (t->text == OperatorExpression::OPVALS[OperatorExpression::OP_MATCH])
        return new OperatorExpression(OperatorExpression::OP_MATCH, assertType(result, Type::LITERAL, p, ctx, false), assertType(element(), Type::LITERAL, p, ctx));
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
        if (t->text == OperatorExpression::OPVALS[OperatorExpression::OP_BAND])
          result = new OperatorExpression(OperatorExpression::OP_BAND, assertType(result, Type::BOOLEAN, p, ctx, false), assertType(relation(), Type::BOOLEAN, p, ctx));
        else if (t->text == OperatorExpression::OPVALS[OperatorExpression::OP_BIOR])
          result = new OperatorExpression(OperatorExpression::OP_BIOR, assertType(result, Type::BOOLEAN, p, ctx, false), assertType(relation(), Type::BOOLEAN, p, ctx));
        else if (t->text == OperatorExpression::OPVALS[OperatorExpression::OP_BXOR])
          result = new OperatorExpression(OperatorExpression::OP_BXOR, assertType(result, Type::BOOLEAN, p, ctx, false), assertType(relation(), Type::BOOLEAN, p, ctx));
        else
          done = true;
        break;
      case TOKEN_AND:
        result = new OperatorExpression(OperatorExpression::OP_BAND, assertType(result, Type::BOOLEAN, p, ctx, false), assertType(relation(), Type::BOOLEAN, p, ctx));
        break;
      case TOKEN_OR:
        result = new OperatorExpression(OperatorExpression::OP_BIOR, assertType(result, Type::BOOLEAN, p, ctx, false), assertType(relation(), Type::BOOLEAN, p, ctx));
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

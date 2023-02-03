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

#include "expression_operator.h"
#include "expression_boolean.h"
#include "expression_integer.h"
#include "expression_numeric.h"
#include "expression_literal.h"
#include "expression_tabchar.h"
#include "expression_complex.h"
#include "exception_parse.h"
#include "context.h"

#include <cmath>
#include <regex>
#include <climits>

namespace bloc
{

const char * OperatorExpression::OPVALS[] = {
  "",         "+",        "-",        "*",        "/",
  "power",    "-",        "+",        "%",        "",
  "&",        "|",        "^",        "~",        "",
  "<<",       ">>",       ".",        "",         "",
  "==",       "!=",       "<",        "<=",       ">",
  ">=",       "",         "",         "",         "",
  "and",      "or",       "xor",      "!",        "",
  "matches",
};

OperatorExpression::~OperatorExpression() {
  if (arg2)
    delete arg2;
  if (arg1)
    delete arg1;
}

std::string OperatorExpression::unparse(Context&ctx) const
{
  std::string str;
  if (arg1 == nullptr)
    str.append(OPVALS[oper]).append(arg2->unparse(ctx));
  else
    str.append("(").append(arg1->unparse(ctx)).append(" ").append(OPVALS[oper]).append(" ").append(arg2->unparse(ctx)).append(")");
  return str;
}

const Type& OperatorExpression::type(Context&ctx) const
{
  switch (oper)
  {
  /* varrying on its argument type */
  case OP_NOT:  /* integer or boolean */
  case OP_NEG:
  case OP_POS:
    return arg2->type(ctx);

  /* varrying on first argument type */
  case OP_AND:
  case OP_IOR:
  case OP_XOR:
  case OP_POP:
  case OP_PUS:
    return IntegerExpression::type_static;

  /* Concatenation else Numeric expression */
  case OP_ADD:
    if (arg1->type(ctx) == Type::LITERAL)
      return LiteralExpression::type_static;
      /* no break */
  case OP_SUB:
  case OP_MUL:
  case OP_DIV:
  case OP_MOD:
  case OP_EXP:
    if (arg1->type(ctx) == Type::INTEGER && arg2->type(ctx) == Type::INTEGER)
      return IntegerExpression::type_static;
    return NumericExpression::type_static;

  /* Boolean expression */
  case OP_EQ:
  case OP_NE:
  case OP_LT:
  case OP_LE:
  case OP_GT:
  case OP_GE:
  case OP_BAND:
  case OP_BIOR:
  case OP_BXOR:
  case OP_BNOT:
  case OP_MATCH:
    return BooleanExpression::type_static;

  default:
    return ComplexExpression::null;
  }
}

bool OperatorExpression::boolean(Context&ctx) const
{
  switch (oper)
  {
  case OP_AND:
    return arg1->boolean(ctx) && arg2->boolean(ctx);

  case OP_IOR:
    return arg1->boolean(ctx) || arg2->boolean(ctx);

  case OP_XOR:
    return arg1->boolean(ctx) ^ arg2->boolean(ctx);

  case OP_EQ:
  {
    const Type& t1 = arg1->type(ctx);
    const Type& t2 = arg2->type(ctx);
    if (t1 == t2 ||
            (t1 == Type::NUMERIC && t2 == Type::INTEGER) ||
            (t1 == Type::INTEGER && t2 == Type::NUMERIC))
    {
      switch (t1.major())
      {
      case Type::BOOLEAN:
        return (arg1->boolean(ctx) == arg2->boolean(ctx));
      case Type::INTEGER:
        if (t2 == Type::NUMERIC)
          return (arg1->numeric(ctx) == arg2->numeric(ctx));
        else
          return (arg1->integer(ctx) == arg2->integer(ctx));
      case Type::NUMERIC:
        return (arg1->numeric(ctx) == arg2->numeric(ctx));
      case Type::LITERAL:
        return (arg1->literal(ctx).compare(arg2->literal(ctx)) == 0);
      case Type::COMPLEX:
        return (arg1->complex(ctx) == arg2->complex(ctx));
      default:
        return false;
      }
    }
    return false;
  }

  case OP_NE:
  {
    const Type& t1 = arg1->type(ctx);
    const Type& t2 = arg2->type(ctx);
    if (t1 == t2 ||
            (t1 == Type::NUMERIC && t2 == Type::INTEGER) ||
            (t1 == Type::INTEGER && t2 == Type::NUMERIC))
    {
      switch (t1.major())
      {
      case Type::BOOLEAN:
        return (arg1->boolean(ctx) != arg2->boolean(ctx));
      case Type::INTEGER:
        if (t2 == Type::NUMERIC)
          return (arg1->numeric(ctx) != arg2->numeric(ctx));
        else
          return (arg1->integer(ctx) != arg2->integer(ctx));
      case Type::NUMERIC:
        return (arg1->numeric(ctx) != arg2->numeric(ctx));
      case Type::LITERAL:
        return (arg1->literal(ctx).compare(arg2->literal(ctx)) != 0);
      case Type::COMPLEX:
        return (arg1->complex(ctx) != arg2->complex(ctx));
      default:
        return false;
      }
    }
    return false;
  }

  case OP_LT:
    switch (arg1->type(ctx).major())
    {
    case Type::INTEGER:
      if (arg2->type(ctx) == Type::NUMERIC)
        return (arg1->numeric(ctx) < arg2->numeric(ctx));
      else
        return (arg1->integer(ctx) < arg2->integer(ctx));
    case Type::NUMERIC:
      return (arg1->numeric(ctx) < arg2->numeric(ctx));
    case Type::LITERAL:
      return (arg1->literal(ctx).compare(arg2->literal(ctx)) < 0);
    default:
      return false;
    }
    break;

  case OP_LE:
    switch (arg1->type(ctx).major())
    {
    case Type::INTEGER:
      if (arg2->type(ctx) == Type::NUMERIC)
        return (arg1->numeric(ctx) <= arg2->numeric(ctx));
      else
        return (arg1->integer(ctx) <= arg2->integer(ctx));
    case Type::NUMERIC:
      return (arg1->numeric(ctx) <= arg2->numeric(ctx));
    case Type::LITERAL:
      return (arg1->literal(ctx).compare(arg2->literal(ctx)) <= 0);
    default:
      return false;
    }
    break;

  case OP_GT:
    switch (arg1->type(ctx).major())
    {
    case Type::INTEGER:
      if (arg2->type(ctx) == Type::NUMERIC)
        return (arg1->numeric(ctx) > arg2->numeric(ctx));
      else
        return (arg1->integer(ctx) > arg2->integer(ctx));
    case Type::NUMERIC:
      return (arg1->numeric(ctx) > arg2->numeric(ctx));
    case Type::LITERAL:
      return (arg1->literal(ctx).compare(arg2->literal(ctx)) > 0);
    default:
      return false;
    }
    break;

  case OP_GE:
    switch (arg1->type(ctx).major())
    {
    case Type::INTEGER:
      if (arg2->type(ctx) == Type::NUMERIC)
        return (arg1->numeric(ctx) >= arg2->numeric(ctx));
      else
        return (arg1->integer(ctx) >= arg2->integer(ctx));
    case Type::NUMERIC:
      return (arg1->numeric(ctx) >= arg2->numeric(ctx));
    case Type::LITERAL:
      return (arg1->literal(ctx).compare(arg2->literal(ctx)) >= 0);
    default:
      return false;
    }
    break;

  case OP_BAND:
    return (arg1->boolean(ctx) && arg2->boolean(ctx));

  case OP_BIOR:
    return (arg1->boolean(ctx) || arg2->boolean(ctx));

  case OP_BXOR:
    return (arg1->boolean(ctx) ^ arg2->boolean(ctx));

  case OP_BNOT:
    return (arg2->boolean(ctx) == false);

  case OP_MATCH:
    return std::regex_match(arg1->literal(ctx), std::regex(arg2->literal(ctx)));

  default:
    throw RuntimeError(EXC_RT_INV_EXPRESSION);
  }
}

int64_t OperatorExpression::integer(Context&ctx) const
{
  switch (oper)
  {
  case OP_ADD:
    return arg1->integer(ctx) + arg2->integer(ctx);

  case OP_SUB:
    return arg1->integer(ctx) - arg2->integer(ctx);

  case OP_MUL:
    return arg1->integer(ctx) * arg2->integer(ctx);

  case OP_DIV:
  {
    int64_t l = arg2->integer(ctx);
    if (l == 0)
      throw RuntimeError(EXC_RT_DIVIDE_BY_ZERO);
    return arg1->integer(ctx) / l;
  }
  case OP_EXP:
    return (int64_t) std::pow(arg1->integer(ctx), arg2->integer(ctx));

  case OP_NEG:
    return 0 - arg2->integer(ctx);

  case OP_POS:
    return arg2->integer(ctx);

  case OP_MOD:
  {
    int64_t l = arg2->integer(ctx);
    if (l == 0)
      throw RuntimeError(EXC_RT_DIVIDE_BY_ZERO);
    return arg1->integer(ctx) % l;
  }
  case OP_AND:
    return arg1->integer(ctx) & arg2->integer(ctx);

  case OP_IOR:
    return arg1->integer(ctx) | arg2->integer(ctx);

  case OP_XOR:
    return arg1->integer(ctx) ^ arg2->integer(ctx);

  case OP_NOT:
    return INT64_MAX ^ arg2->integer(ctx);

  case OP_POP:
    return arg1->integer(ctx) << arg2->integer(ctx);

  case OP_PUS:
    return arg1->integer(ctx) >> arg2->integer(ctx);

  default:
    throw RuntimeError(EXC_RT_INV_EXPRESSION);
  }
}

double OperatorExpression::numeric(Context&ctx) const
{
  switch (oper)
  {
  case OP_ADD:
    return arg1->numeric(ctx) + arg2->numeric(ctx);

  case OP_SUB:
    return arg1->numeric(ctx) - arg2->numeric(ctx);

  case OP_MUL:
    return arg1->numeric(ctx) * arg2->numeric(ctx);

  case OP_DIV:
  {
    double d = arg2->numeric(ctx);
    if (d == 0.0)
      throw RuntimeError(EXC_RT_DIVIDE_BY_ZERO);
    return arg1->numeric(ctx) / d;
  }
  case OP_EXP:
    return (std::pow(arg1->numeric(ctx), arg2->numeric(ctx)));

  case OP_NEG:
    return 0 - arg2->numeric(ctx);

  case OP_POS:
    return arg2->numeric(ctx);

  case OP_MOD:
  {
    double d = arg2->numeric(ctx);
    if (d == 0.0)
      throw RuntimeError(EXC_RT_DIVIDE_BY_ZERO);
    return std::fmod(arg1->numeric(ctx), d);
  }

  default:
    throw RuntimeError(EXC_RT_INV_EXPRESSION);
  }
}

std::string& OperatorExpression::literal(Context& ctx) const
{
  switch (oper)
  {
  case OP_ADD:
    if (arg1->isRvalue(ctx))
      return arg1->literal(ctx).append(arg2->literal(ctx));
    else
      return ctx.allocate(std::string(arg1->literal(ctx))).append(arg2->literal(ctx));
  default:
    break;
  }
  throw RuntimeError(EXC_RT_INV_EXPRESSION);
}

std::set<OperatorExpression::OP> OperatorExpression::operatorSet()
{
  std::set<OP> lst;
  for (int i = 0; i < (sizeof (OPVALS) / sizeof (char*)); ++i)
    if (OPVALS[i][0] != '\0')
      lst.insert((OP)i);
  return lst;
}

const char * OperatorExpression::HELPS[] = {
  "",
  "addition ( x + y )",
  "subtraction ( x - y )",
  "multiplication ( x * y )",
  "division ( x / y )",
  "factor ( x ** y ), alias ( x power y ), alias pow( x , y )",
  "unary minus ( -x )",
  "unary positive ( +x )",
  "modulo ( x % y ), alias mod( x , y )",
  "",
  "bitwise AND ( x & y )",
  "bitwise inclusive OR ( x | y )",
  "bitwise exclusive OR ( x ^ y )",
  "unary Bitwise NOT ( ~x )",
  "",
  "left shift bit ( x << y )",
  "right shift bit ( x >> y )",
  "member operator {var}.member(...)",
  "", "",
  "equality ( x == y )",
  "inequality ( x != y )",
  "less than ( x < y )",
  "less than or equal ( x <= y )",
  "greater than ( x > y )",
  "greater than or equal ( x >= y )",
  "", "", "", "",
  "logical AND ( x && y ), alias ( x and y )",
  "logical inclusive OR ( x || y ), alias ( x or y )",
  "logical exclusive OR ( x xor y )",
  "logical negation ( !x )",
  "",
  "regular expression ( x matches y )",
};

}

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

#include "builtin_round.h"
#include <blocc/parse_expression.h>
#include <blocc/exception_parse.h>
#include <blocc/context.h>
#include <blocc/parser.h>
#include <blocc/debug.h>

#include <cmath>

namespace bloc
{

Value& ROUNDExpression::value(Context & ctx) const
{
  Value& val = _args[0]->value(ctx);
  if (_args.size() > 1)
  {
    double d = 1.0;
    Value& a1 = _args[1]->value(ctx);
    Value v(Value::type_numeric);
    switch (a1.type().major())
    {
    case Type::NO_TYPE:
      break;
    case Type::INTEGER:
      if (!a1.isNull())
        d = std::pow(10, *a1.integer());
      break;
    case Type::NUMERIC:
      if (!a1.isNull())
        d = std::pow(10, Integer(*a1.numeric()));
      break;
    default:
      throw RuntimeError(EXC_RT_FUNC_ARG_TYPE_S, KEYWORDS[oper]);
    }
    switch (val.type().major())
    {
    case Type::NO_TYPE:
      break;
    case Type::INTEGER:
      if (val.isNull())
        v = Value(Value::type_numeric);
      else
        v = Value(Numeric(std::floor(Numeric(*val.integer()) + 0.5)));
      break;
    case Type::NUMERIC:
      if (val.isNull())
        return val;
      else
        v = Value(Numeric(std::floor(*val.numeric() * d + 0.5) / d));
      break;
    case Type::IMAGINARY:
      if (val.isNull())
        return val;
      else
      {
        Imaginary z = *val.imaginary();
        v = Value(new Imaginary{std::floor(z.a * d + 0.5) / d, std::floor(z.b * d + 0.5) / d});
      }
      break;
    default:
      throw RuntimeError(EXC_RT_FUNC_ARG_TYPE_S, KEYWORDS[oper]);
    }
    if (val.lvalue())
      return ctx.allocate(std::move(v));
    val.swap(Value(std::move(v)));
    return val;
  }
  else
  {
    Value v;
    switch (val.type().major())
    {
    case Type::INTEGER:
      if (val.isNull())
        v = Value(Value::type_numeric);
      else
        v = Value(Numeric(std::floor(Numeric(*val.integer()) + 0.5)));
      break;
    case Type::NUMERIC:
      if (val.isNull())
        return val;
      else
        v = Value(Numeric(std::floor(*val.numeric() + 0.5)));
      break;
    case Type::IMAGINARY:
      if (val.isNull())
        return val;
      else
      {
        Imaginary z = *val.imaginary();
        v = Value(new Imaginary{std::floor(z.a + 0.5), std::floor(z.b + 0.5)});
      }
      break;
    default:
      throw RuntimeError(EXC_RT_FUNC_ARG_TYPE_S, KEYWORDS[oper]);
    }
    if (val.lvalue())
      return ctx.allocate(std::move(v));
    val.swap(Value(std::move(v)));
    return val;
  }
}

ROUNDExpression * ROUNDExpression::parse(Parser& p, Context& ctx)
{
  std::vector<Expression*> args;

  try
  {
    TokenPtr t = p.pop();
    if (t->code != '(')
      throw ParseError(EXC_PARSE_FUNC_ARG_NUM_S, KEYWORDS[FUNC_ROUND]);
    args.push_back(ParseExpression::expression(p, ctx));
    if (!ParseExpression::typeChecking(args.back(), Type::NUMERIC, p, ctx))
      throw ParseError(EXC_PARSE_FUNC_ARG_TYPE_S, KEYWORDS[FUNC_ROUND]);
    if (p.front()->code == Parser::Chain)
    {
      t = p.pop();
      args.push_back(ParseExpression::expression(p, ctx));
      if (!ParseExpression::typeChecking(args.back(), Type::NUMERIC, p, ctx))
        throw ParseError(EXC_PARSE_FUNC_ARG_TYPE_S, KEYWORDS[FUNC_ROUND]);
    }
    assertClosedFunction(p, ctx, FUNC_ROUND);
    return new ROUNDExpression(std::move(args));
  }
  catch (ParseError& pe)
  {
    DBG(DBG_DEBUG, "exception %p at %s line %d\n", &pe, __PRETTY_FUNCTION__, __LINE__);
    for (Expression * e : args)
      delete e;
    throw;
  }
  return nullptr;
}

}

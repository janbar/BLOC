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

#include "builtin_clamp.h"
#include <blocc/parse_expression.h>
#include <blocc/exception_parse.h>
#include <blocc/context.h>
#include <blocc/parser.h>
#include <blocc/debug.h>

#include <cmath>

namespace bloc
{

Value& CLAMPExpression::value(Context & ctx) const
{
  Value& a0 = _args[0]->value(ctx);
  Value& a1 = _args[1]->value(ctx);
  Value& a2 = _args[2]->value(ctx);
  Value v(Value::type_numeric);

  switch (a0.type().major())
  {
  case Type::NO_TYPE:
    break;
  case Type::INTEGER:
    if (a0.isNull() || a1.isNull() || a2.isNull())
      return a0;
    {
      Integer x = *a0.integer();
      Integer y = *a1.integer();
      Integer z = *a2.integer();
      if (x < y)
        v = Value(y);
      if (x > z)
        v = Value(z);
      else
        v = Value(x);
    }
    break;
  case Type::NUMERIC:
    if (a0.isNull() || a1.isNull() || a2.isNull())
      return a0;
    {
      Numeric x = *a0.numeric();
      Numeric y = *a1.numeric();
      Numeric z = *a2.numeric();
      if (x < y)
        v = Value(y);
      if (x > z)
        v = Value(z);
      else
        v = Value(x);
    }
    break;
  default:
    throw RuntimeError(EXC_RT_FUNC_ARG_TYPE_S, KEYWORDS[oper]);
  }
  if (a0.lvalue())
    return ctx.allocate(std::move(v));
  a0.swap(Value(std::move(v)));
  return a0;
}

CLAMPExpression * CLAMPExpression::parse(Parser& p, Context& ctx)
{
  std::vector<Expression*> args;

  try
  {
    TokenPtr t = p.pop();
    if (t->code != '(')
      throw ParseError(EXC_PARSE_FUNC_ARG_NUM_S, KEYWORDS[FUNC_CLAMP]);
    args.push_back(ParseExpression::expression(p, ctx));
    if (!ParseExpression::typeChecking(args.back(), Type::NUMERIC, p, ctx))
      throw ParseError(EXC_PARSE_FUNC_ARG_TYPE_S, KEYWORDS[FUNC_CLAMP]);
    t = p.pop();
    if (t->code != Parser::CHAIN)
      throw ParseError(EXC_PARSE_FUNC_ARG_NUM_S, KEYWORDS[FUNC_CLAMP]);
    args.push_back(ParseExpression::expression(p, ctx));
    if (!ParseExpression::typeChecking(args.back(), Type::NUMERIC, p, ctx))
      throw ParseError(EXC_PARSE_FUNC_ARG_TYPE_S, KEYWORDS[FUNC_CLAMP]);
    t = p.pop();
    if (t->code != Parser::CHAIN)
      throw ParseError(EXC_PARSE_FUNC_ARG_NUM_S, KEYWORDS[FUNC_CLAMP]);
    args.push_back(ParseExpression::expression(p, ctx));
    if (!ParseExpression::typeChecking(args.back(), Type::NUMERIC, p, ctx))
      throw ParseError(EXC_PARSE_FUNC_ARG_TYPE_S, KEYWORDS[FUNC_CLAMP]);
    assertClosedFunction(p, ctx, FUNC_CLAMP);
    return new CLAMPExpression(std::move(args));
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


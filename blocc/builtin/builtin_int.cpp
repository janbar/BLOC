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

#include "builtin_int.h"
#include <blocc/parse_expression.h>
#include <blocc/exception_parse.h>
#include <blocc/context.h>
#include <blocc/parser.h>
#include <blocc/debug.h>

#include <string>

namespace bloc
{

Value& INTExpression::value(Context & ctx) const
{
  if (_args.empty())
    return ctx.allocate(Value(Value::type_integer));

  Value& val = _args[0]->value(ctx);
  Value v(Value::type_integer);

  switch (val.type().major())
  {
  case Type::NO_TYPE:
    break;
  case Type::LITERAL:
    try
    {
      v = Value(Integer(std::stoll(*val.literal())));
    }
    catch (std::invalid_argument& e)
    {
      throw RuntimeError(EXC_RT_STRING_TO_NUM);
    }
    catch (std::out_of_range& e)
    {
      throw RuntimeError(EXC_RT_OUT_OF_RANGE);
    }
    break;
  case Type::NUMERIC:
    v = Value(Integer(*val.numeric()));
    break;
  case Type::INTEGER:
    v = Value(*val.integer());
    break;
  case Type::BOOLEAN:
    v = Value(Integer(*val.boolean() ? 1 : 0));
    break;
  default:
    throw RuntimeError(EXC_RT_FUNC_ARG_TYPE_S, KEYWORDS[oper]);
  }
  if (val.lvalue())
    return ctx.allocate(std::move(v));
  val.swap(Value(std::move(v)));
  return val;
}

INTExpression * INTExpression::parse(Parser& p, Context& ctx)
{
  std::vector<Expression*> args;

  try
  {
    TokenPtr t = p.pop();
    if (t->code != '(')
      throw ParseError(EXC_PARSE_FUNC_ARG_NUM_S, KEYWORDS[FUNC_INT]);
    if (p.front()->code != ')')
    {
      args.push_back(ParseExpression::expression(p, ctx));
      if (args.back()->type(ctx).level() > 0)
        throw ParseError(EXC_PARSE_FUNC_ARG_TYPE_S, KEYWORDS[FUNC_INT]);
      switch (args.back()->type(ctx).major())
      {
      case Type::NO_TYPE:
      case Type::BOOLEAN:
      case Type::LITERAL:
      case Type::INTEGER:
      case Type::NUMERIC:
        break;
      default:
        throw ParseError(EXC_PARSE_FUNC_ARG_TYPE_S, KEYWORDS[FUNC_INT]);
      }
    }
    assertClosedFunction(p, ctx, FUNC_INT);
    return new INTExpression(std::move(args));
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

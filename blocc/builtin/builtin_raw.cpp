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

#include "builtin_raw.h"
#include <blocc/parse_expression.h>
#include <blocc/exception_parse.h>
#include <blocc/context.h>
#include <blocc/parser.h>
#include <blocc/debug.h>

namespace bloc
{

Value& RAWExpression::value(Context & ctx) const
{
  if (_args.empty())
    return ctx.allocate(Value(Value::type_tabchar));

  Value& val = _args[0]->value(ctx);
  Integer n = 0, v = 0;

  if (!val.isNull())
  {
    switch (val.type().major())
    {
    case Type::LITERAL:
    {
      /* return raw of string */
      TabChar * tmp = new TabChar(val.literal()->size());
      tmp->assign(val.literal()->begin(), val.literal()->end());
      if (val.lvalue())
        return ctx.allocate(Value(tmp));
      val.swap(Value(tmp));
      return val;
    }
    case Type::INTEGER:
      n = *val.integer();
      break;
    case Type::NUMERIC:
      n = Integer(*val.numeric());
      break;
    case Type::TABCHAR:
      return val;
    default:
      throw RuntimeError(EXC_RT_FUNC_ARG_TYPE_S, KEYWORDS[FUNC_RAW]);
    }

    if (n < 0)
      throw RuntimeError(EXC_RT_INDEX_RANGE_S, std::to_string(n).c_str());
    if (_args.size() > 1)
    {
      Value& a1 = _args[1]->value(ctx);
      if (!a1.isNull())
        switch (a1.type().major())
        {
        case Type::INTEGER:
          v = *a1.integer();
          break;
        case Type::NUMERIC:
          v = Integer(*a1.numeric());
          break;
        default:
          throw RuntimeError(EXC_RT_FUNC_ARG_TYPE_S, KEYWORDS[FUNC_RAW]);
        }
      if (v < 0 || v > 255)
        throw RuntimeError(EXC_RT_OUT_OF_RANGE);
    }
    if (val.lvalue())
      return ctx.allocate(Value(new TabChar(n, (char)v)));
    val.swap(Value(new TabChar(n, (char)v)));
    return val;
  }
  if (val.lvalue())
    return ctx.allocate(Value(Value::type_tabchar));
  val.swap(Value(Value::type_tabchar));
  return val;
}

RAWExpression * RAWExpression::parse(Parser& p, Context& ctx)
{
  std::vector<Expression*> args;

  try
  {
    TokenPtr t = p.pop();
    if (t->code != '(')
      throw ParseError(EXC_PARSE_FUNC_ARG_NUM_S, KEYWORDS[FUNC_RAW]);
    if (p.front()->code != ')')
    {
      args.push_back(ParseExpression::expression(p, ctx));
      if (!ParseExpression::typeChecking(args.back(), Type::LITERAL, p, ctx))
      {
        if (!ParseExpression::typeChecking(args.back(), Type::INTEGER, p, ctx))
          throw ParseError(EXC_PARSE_FUNC_ARG_TYPE_S, KEYWORDS[FUNC_RAW]);
        if (p.front()->code == Parser::CHAIN)
        {
          t = p.pop();
          args.push_back(ParseExpression::expression(p, ctx));
          if (!ParseExpression::typeChecking(args.back(), Type::INTEGER, p, ctx))
            throw ParseError(EXC_PARSE_FUNC_ARG_TYPE_S, KEYWORDS[FUNC_RAW]);
        }
      }
    }
    assertClosedFunction(p, ctx, FUNC_RAW);
    return new RAWExpression(std::move(args));
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

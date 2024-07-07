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

#include "builtin_strpos.h"
#include <blocc/parse_expression.h>
#include <blocc/exception_parse.h>
#include <blocc/context.h>
#include <blocc/parser.h>
#include <blocc/debug.h>

namespace bloc
{

Value& STRPOSExpression::value(Context & ctx) const
{
  Value& val = _args[0]->value(ctx);
  Value& a1 = _args[1]->value(ctx);
  Value v(Value::type_integer);

  switch (val.type().major())
  {
  case Type::NO_TYPE:
    break;
  case Type::LITERAL:
  {
    if (val.isNull() || a1.isNull())
      break;
    Integer s = 0;
    if (_args.size() > 2)
    {
      Value& a2 = _args[2]->value(ctx);
      switch (a2.type().major())
      {
      case Type::NO_TYPE:
        if (val.lvalue())
          return ctx.allocate(std::move(v));
        val.swap(std::move(v));
        return val;
      case Type::INTEGER:
        s = *a2.integer();
        break;
      case Type::NUMERIC:
        s = *a2.numeric();
        break;
      default:
        throw RuntimeError(EXC_RT_FUNC_ARG_TYPE_S, KEYWORDS[oper]);
      }
      if (s < 0)
        throw RuntimeError(EXC_RT_INDEX_RANGE_S, a2.toString().c_str());
    }
    size_t p = val.literal()->find(*a1.literal(), (size_t)s);
    if (p != std::string::npos)
      v = Value(Integer(p));
    break;
  }
  default:
    throw RuntimeError(EXC_RT_FUNC_ARG_TYPE_S, KEYWORDS[oper]);
  }
  if (!val.lvalue())
  {
    val.swap(Value(std::move(v)));
    return val;
  }
  if (!a1.lvalue())
  {
    a1.swap(Value(std::move(v)));
    return a1;
  }
  return ctx.allocate(std::move(v));
}

STRPOSExpression * STRPOSExpression::parse(Parser& p, Context& ctx)
{
  std::vector<Expression*> args;

  try
  {
    TokenPtr t = p.pop();
    if (t->code != '(')
      throw ParseError(EXC_PARSE_FUNC_ARG_NUM_S, KEYWORDS[FUNC_STRPOS]);
    args.push_back(ParseExpression::expression(p, ctx));
    if (!ParseExpression::typeChecking(args.back(), Type::LITERAL, p, ctx))
      throw ParseError(EXC_PARSE_FUNC_ARG_TYPE_S, KEYWORDS[FUNC_STRPOS]);
    t = p.pop();
    if (t->code != Parser::Chain)
      throw ParseError(EXC_PARSE_FUNC_ARG_NUM_S, KEYWORDS[FUNC_STRPOS]);
    args.push_back(ParseExpression::expression(p, ctx));
    if (!ParseExpression::typeChecking(args.back(), Type::LITERAL, p, ctx))
      throw ParseError(EXC_PARSE_FUNC_ARG_TYPE_S, KEYWORDS[FUNC_STRPOS]);
    if (p.front()->code == Parser::Chain)
    {
      t = p.pop();
      args.push_back(ParseExpression::expression(p, ctx));
      if (!ParseExpression::typeChecking(args.back(), Type::INTEGER, p, ctx))
        throw ParseError(EXC_PARSE_FUNC_ARG_TYPE_S, KEYWORDS[FUNC_STRPOS]);
    }
    assertClosedFunction(p, ctx, FUNC_STRPOS);
    return new STRPOSExpression(std::move(args));
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

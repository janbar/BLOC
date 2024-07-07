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

#include "builtin_substr.h"
#include <blocc/parse_expression.h>
#include <blocc/exception_parse.h>
#include <blocc/context.h>
#include <blocc/parser.h>
#include <blocc/debug.h>

namespace bloc
{

Value& SUBSTRExpression::value(Context & ctx) const
{
  Value& val = _args[0]->value(ctx);

  switch (val.type().major())
  {
  case Type::NO_TYPE:
    if (val.lvalue())
      return ctx.allocate(Value(Value::type_literal));
    val.swap(Value(Value::type_literal));
    return val;
  case Type::LITERAL:
  {
    Value& a1 = _args[1]->value(ctx);
    int64_t a;
    switch (a1.type().major())
    {
    case Type::NO_TYPE:
      return val;
    case Type::INTEGER:
      if (a1.isNull())
        return val;
      a = *a1.integer();
      break;
    case Type::NUMERIC:
      if (a1.isNull())
        return val;
      a = Integer(*a1.numeric());
      break;
    default:
      throw RuntimeError(EXC_RT_FUNC_ARG_TYPE_S, KEYWORDS[oper]);
    }
    if (val.isNull())
      return val;
    int64_t b, c;
    c = val.literal()->size();
    b = c;
    if (_args.size() > 2)
    {
      Value& a2 = _args[2]->value(ctx);
      switch (a2.type().major())
      {
      case Type::NO_TYPE:
        return val;
      case Type::INTEGER:
        if (a2.isNull())
          return val;
        b = *a2.integer();
        break;
      case Type::NUMERIC:
        if (a2.isNull())
          return val;
        b = Integer(*a2.numeric());
        break;
      default:
        throw RuntimeError(EXC_RT_FUNC_ARG_TYPE_S, KEYWORDS[oper]);
      }
    }
    if (c == 0)
      return val;
    a = (a < 0 ? a + c : a);
    b = std::max<int64_t>(std::min(b, c - a), 0L);
    if (a >= 0 && b > 0)
    {
      if (val.lvalue())
        return ctx.allocate(Value(new Literal(val.literal()->substr(a, b))));
      val.literal()->assign(val.literal()->substr(a, b));
      return val;
    }
    if (val.lvalue())
      return ctx.allocate(Value(new Literal()));
    val.literal()->clear();
    return val;
  }
  default:
    throw RuntimeError(EXC_RT_FUNC_ARG_TYPE_S, KEYWORDS[oper]);
  }
}

SUBSTRExpression * SUBSTRExpression::parse(Parser& p, Context& ctx)
{
  std::vector<Expression*> args;

  try
  {
    TokenPtr t = p.pop();
    if (t->code != '(')
      throw ParseError(EXC_PARSE_FUNC_ARG_NUM_S, KEYWORDS[FUNC_SUBSTR]);
    args.push_back(ParseExpression::expression(p, ctx));
    if (!ParseExpression::typeChecking(args.back(), Type::LITERAL, p, ctx))
      throw ParseError(EXC_PARSE_FUNC_ARG_TYPE_S, KEYWORDS[FUNC_SUBSTR]);
    t = p.pop();
    if (t->code != Parser::Chain)
      throw ParseError(EXC_PARSE_FUNC_ARG_NUM_S, KEYWORDS[FUNC_SUBSTR]);
    args.push_back(ParseExpression::expression(p, ctx));
    if (!ParseExpression::typeChecking(args.back(), Type::INTEGER, p, ctx))
      throw ParseError(EXC_PARSE_FUNC_ARG_TYPE_S, KEYWORDS[FUNC_SUBSTR]);
    if (p.front()->code == Parser::Chain)
    {
      t = p.pop();
      args.push_back(ParseExpression::expression(p, ctx));
      if (!ParseExpression::typeChecking(args.back(), Type::INTEGER, p, ctx))
        throw ParseError(EXC_PARSE_FUNC_ARG_TYPE_S, KEYWORDS[FUNC_SUBSTR]);
    }
    assertClosedFunction(p, ctx, FUNC_SUBSTR);
    return new SUBSTRExpression(std::move(args));
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

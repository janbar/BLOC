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

#include "builtin_hex.h"
#include <blocc/parse_expression.h>
#include <blocc/exception_parse.h>
#include <blocc/context.h>
#include <blocc/parser.h>
#include <blocc/debug.h>

namespace bloc
{

std::string HEXExpression::hex(Integer val, Integer n)
{
  static const char g[16] = {
    '0', '1', '2', '3', '4', '5', '6', '7',
    '8', '9', 'a', 'b', 'c', 'd', 'e', 'f',
  };
  Integer s = 0;
  unsigned char len = 0;
  char buf[2 * sizeof(Integer)];

  for (int d = 4 * (sizeof(buf) - 1); d > 0; d -= 4)
  {
    int c = 0xf & (val >> d);
    s += c;
    if (s || n >= Integer(sizeof(buf)))
    {
      buf[len++] = g[c];
    }
    n += 1;
  }
  buf[len++] = g[(0xf & val)];
  return std::string(buf, len);
}

Value& HEXExpression::value(Context & ctx) const
{
  Value& arg0 = _args[0]->value(ctx);
  Value v(Value::type_literal);

  if (arg0.isNull())
    return ctx.allocate(Value(Value::type_literal));
  else
  {
    Integer n = 0;
    if (_args.size() > 1)
    {
      Value& arg1 = _args[1]->value(ctx);
      if (!arg1.isNull())
        switch (arg1.type().major())
        {
        case Type::INTEGER:
          n = *arg1.integer();
          break;
        case Type::NUMERIC:
          n = Integer(*arg1.numeric());
          break;
        default:
          throw RuntimeError(EXC_RT_FUNC_ARG_TYPE_S, KEYWORDS[FUNC_HEX]);
        }
    }
    switch (arg0.type().major())
    {
    case Type::INTEGER:
      v = Value(new Literal(hex(*arg0.integer(), n)));
      break;
    case Type::NUMERIC:
      v = Value(new Literal(hex(Integer(*arg0.numeric()), n)));
      break;
    default:
      throw RuntimeError(EXC_RT_FUNC_ARG_TYPE_S, KEYWORDS[FUNC_HEX]);
    }
  }
  if (arg0.lvalue())
    return ctx.allocate(std::move(v));
  arg0.swap(Value(std::move(v)));
  return arg0;
}

HEXExpression * HEXExpression::parse(Parser& p, Context& ctx)
{
  std::vector<Expression*> args;

  try
  {
    TokenPtr t = p.pop();
    if (t->code != '(')
      throw ParseError(EXC_PARSE_FUNC_ARG_NUM_S, KEYWORDS[FUNC_HEX], t);
    args.push_back(ParseExpression::expression(p, ctx));
    if (!ParseExpression::typeChecking(args.back(), Type::INTEGER, p, ctx))
      throw ParseError(EXC_PARSE_FUNC_ARG_TYPE_S, KEYWORDS[FUNC_HEX], t);
    if (p.front()->code == Parser::Chain)
    {
      t = p.pop();
      args.push_back(ParseExpression::expression(p, ctx));
      if (!ParseExpression::typeChecking(args.back(), Type::INTEGER, p, ctx))
        throw ParseError(EXC_PARSE_FUNC_ARG_TYPE_S, KEYWORDS[FUNC_HEX], t);
    }
    assertClosedFunction(p, ctx, FUNC_HEX);
    return new HEXExpression(std::move(args));
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

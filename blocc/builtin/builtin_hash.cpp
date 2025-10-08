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

#include "builtin_hash.h"
#include <blocc/parse_expression.h>
#include <blocc/exception_parse.h>
#include <blocc/context.h>
#include <blocc/parser.h>
#include <blocc/debug.h>

#include <cstdint>

namespace bloc
{

static uint32_t bloc_builtin_hash(uint32_t maxsize, const char * buf, unsigned len)
{
  /*
   * DJB Hash Function
   */
  uint32_t h = 5381;
  const char * end = buf + len;

  while (buf < end)
  {
    h = ((h << 5) + h) + *buf++;
  }
  return h % maxsize;
}

Value& HASHExpression::value(Context & ctx) const
{
  Value& val = _args[0]->value(ctx);
  Value v(Value::type_integer);
  uint32_t max_size = UINT32_MAX;

  if (_args.size() > 1)
  {
    Value& a1 = _args[1]->value(ctx);
    switch (a1.type().major())
    {
    case Type::NO_TYPE:
      break;
    case Type::INTEGER:
      if (!a1.isNull())
        max_size = (uint32_t)*a1.integer();
      break;
    case Type::NUMERIC:
      if (!a1.isNull())
        max_size = (uint32_t)*a1.numeric();
      break;
    default:
      throw RuntimeError(EXC_RT_FUNC_ARG_TYPE_S, KEYWORDS[oper]);
    }
  }
  switch (val.type().major())
  {
  case Type::NO_TYPE:
    break;
  case Type::LITERAL:
    if (!val.isNull())
      v = Value(Integer(bloc_builtin_hash(max_size, val.literal()->data(), val.literal()->size())));
    break;
  case Type::TABCHAR:
    if (!val.isNull())
      v = Value(Integer(bloc_builtin_hash(max_size, val.tabchar()->data(), val.tabchar()->size())));
    break;
  default:
    throw RuntimeError(EXC_RT_FUNC_ARG_TYPE_S, KEYWORDS[oper]);
  }
  if (val.lvalue())
    return ctx.allocate(std::move(v));
  val.swap(Value(std::move(v)));
  return val;
}

HASHExpression * HASHExpression::parse(Parser& p, Context& ctx)
{
  std::vector<Expression*> args;

  try
  {
    TokenPtr t = p.pop();
    if (t->code != '(')
      throw ParseError(EXC_PARSE_FUNC_ARG_NUM_S, KEYWORDS[FUNC_HASH], t);
    args.push_back(ParseExpression::expression(p, ctx));
    if (args.back()->type(ctx).level() > 0)
      throw ParseError(EXC_PARSE_FUNC_ARG_TYPE_S, KEYWORDS[FUNC_HASH], t);
    switch (args.back()->type(ctx).major())
    {
    case Type::NO_TYPE: /* opaque */
    case Type::LITERAL:
    case Type::TABCHAR:
      break;
    default:
      throw ParseError(EXC_PARSE_FUNC_ARG_TYPE_S, KEYWORDS[FUNC_HASH], t);
    }
    if (p.front()->code == Parser::Chain)
    {
      t = p.pop();
      args.push_back(ParseExpression::expression(p, ctx));
      if (!ParseExpression::typeChecking(args.back(), Type::INTEGER, p, ctx))
        throw ParseError(EXC_PARSE_FUNC_ARG_TYPE_S, KEYWORDS[FUNC_HASH], t);
    }
    assertClosedFunction(p, ctx, FUNC_HASH);
    return new HASHExpression(std::move(args));
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

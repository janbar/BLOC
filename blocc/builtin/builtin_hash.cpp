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

int64_t HASHExpression::integer(Context & ctx) const
{
  uint32_t max_size = UINT32_MAX;
  if (_args.size() > 1)
  {
    int64_t l = _args[1]->integer(ctx);
    if (l <= 0)
      throw RuntimeError(EXC_RT_OUT_OF_RANGE);
    if (l < UINT32_MAX)
    max_size = (uint32_t) l;
  }
  switch (_args[0]->type(ctx).major())
  {
  case Type::LITERAL:
  {
    const std::string& rv = _args[0]->literal(ctx);
    return (int64_t) bloc_builtin_hash(max_size, rv.data(), rv.size());
  }
  case Type::TABCHAR:
  {
    const TabChar& rv = _args[0]->tabchar(ctx);
    return (int64_t) bloc_builtin_hash(max_size, rv.data(), rv.size());
  }
  default:
    throw RuntimeError(EXC_RT_FUNC_ARG_TYPE_S, KEYWORDS[FUNC_HASH]);
  }
}

HASHExpression * HASHExpression::parse(Parser& p, Context& ctx)
{
  std::vector<Expression*> args;

  try
  {
    TokenPtr t = p.pop();
    if (t->code != '(')
      throw ParseError(EXC_PARSE_FUNC_ARG_NUM_S, KEYWORDS[FUNC_HASH]);
    args.push_back(ParseExpression::expression(p, ctx));
    if (args.back()->type(ctx).level() > 0)
      throw ParseError(EXC_PARSE_FUNC_ARG_TYPE_S, KEYWORDS[FUNC_HASH]);
    switch (args.back()->type(ctx).major())
    {
    case Type::LITERAL:
    case Type::TABCHAR:
      break;
    default:
      throw ParseError(EXC_PARSE_FUNC_ARG_TYPE_S, KEYWORDS[FUNC_HASH]);
    }
    if (p.front()->code == Parser::CHAIN)
    {
      t = p.pop();
      args.push_back(ParseExpression::expression(p, ctx));
      if (!ParseExpression::typeChecking(args.back(), Type::INTEGER, p, ctx))
        throw ParseError(EXC_PARSE_FUNC_ARG_TYPE_S, KEYWORDS[FUNC_HASH]);
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

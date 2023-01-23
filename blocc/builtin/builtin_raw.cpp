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

TabChar& RAWExpression::tabchar(Context & ctx) const
{
  int64_t n = 0, v = 0;
  /* return empty raw */
  if (_args.empty())
    return ctx.allocate(TabChar());
  /* return raw of string */
  if (_args[0]->type(ctx) == Type::LITERAL)
  {
    std::string& str = _args[0]->literal(ctx);
    TabChar& rv = ctx.allocate(TabChar(str.size()));
    rv.assign(str.begin(), str.end());
    return rv;
  }
  /* return raw of n byte */
  n = _args[0]->integer(ctx);
  if (n < 0)
    throw RuntimeError(EXC_RT_INDEX_RANGE_S, std::to_string(n).c_str());
  if (_args.size() > 1)
    v = _args[1]->integer(ctx);
  if (v < 0 || v > 255)
    throw RuntimeError(EXC_RT_OUT_OF_RANGE);
  return ctx.allocate(TabChar(n, (char)v));
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

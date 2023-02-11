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

#include "builtin_subraw.h"
#include <blocc/parse_expression.h>
#include <blocc/exception_parse.h>
#include <blocc/context.h>
#include <blocc/parser.h>
#include <blocc/debug.h>

namespace bloc
{

TabChar& SUBRAWExpression::tabchar(Context & ctx) const
{
  int64_t a, b, c;
  if (_args[0]->isRvalue())
  {
    TabChar& rv = _args[0]->tabchar(ctx);
    if ((c = rv.size()) == 0)
      return rv;
    b = (_args[2] ? _args[2]->integer(ctx) : c);
    a = _args[1]->integer(ctx);
    a = (a < 0 ? a + c : a);
    b = std::max<int64_t>(std::min(b, c - a), 0L);
    if (a >= 0 && b > 0)
    {
      rv.erase(rv.begin(), rv.begin() + a);
      rv.erase(rv.begin() + b, rv.end());
    }
    else
      rv.clear();
    return rv;
  }
  const TabChar& var = _args[0]->tabchar(ctx);
  if ((c = var.size()) == 0)
    return ctx.allocate(TabChar());
  b = (_args[2] ? _args[2]->integer(ctx) : c);
  a = _args[1]->integer(ctx);
  a = (a < 0 ? a + c : a);
  b = std::max<int64_t>(std::min(b, c - a), 0L);
  if (a >= 0 && b > 0)
  {
    return ctx.allocate(TabChar(var.begin() + a, var.begin() + a + b));
  }
  return ctx.allocate(TabChar());
}

SUBRAWExpression * SUBRAWExpression::parse(Parser& p, Context& ctx)
{
  std::vector<Expression*> args;

  try
  {
    TokenPtr t = p.pop();
    if (t->code != '(')
      throw ParseError(EXC_PARSE_FUNC_ARG_NUM_S, KEYWORDS[FUNC_SUBRAW]);
    args.push_back(ParseExpression::expression(p, ctx));
    if (!ParseExpression::typeChecking(args.back(), Type::TABCHAR, p, ctx))
      throw ParseError(EXC_PARSE_FUNC_ARG_TYPE_S, KEYWORDS[FUNC_SUBRAW]);
    t = p.pop();
    if (t->code != Parser::CHAIN)
      throw ParseError(EXC_PARSE_FUNC_ARG_NUM_S, KEYWORDS[FUNC_SUBRAW]);
    args.push_back(ParseExpression::expression(p, ctx));
    if (!ParseExpression::typeChecking(args.back(), Type::INTEGER, p, ctx))
      throw ParseError(EXC_PARSE_FUNC_ARG_TYPE_S, KEYWORDS[FUNC_SUBRAW]);
    if (p.front()->code == Parser::CHAIN)
    {
      t = p.pop();
      args.push_back(ParseExpression::expression(p, ctx));
      if (!ParseExpression::typeChecking(args.back(), Type::INTEGER, p, ctx))
        throw ParseError(EXC_PARSE_FUNC_ARG_TYPE_S, KEYWORDS[FUNC_SUBRAW]);
    }
    assertClosedFunction(p, ctx, FUNC_SUBRAW);
    return new SUBRAWExpression(std::move(args));
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

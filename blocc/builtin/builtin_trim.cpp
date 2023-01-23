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

#include "builtin_trim.h"
#include <blocc/parse_expression.h>
#include <blocc/exception_parse.h>
#include <blocc/context.h>
#include <blocc/parser.h>
#include <blocc/debug.h>

#include <cstring>

namespace bloc
{

std::string& TRIMExpression::literal(Context & ctx) const
{
  int64_t a, b, c;
  if (_args[0]->isRvalue(ctx))
  {
    std::string& rv = _args[0]->literal(ctx);
    b = rv.length() - 1;
    while (b >= 0 && rv[b] == ' ') --b;
    if (b < 0)
      rv.clear();
    else
    {
      c = rv.length();
      a = 0;
      while (a < c && rv[a] == ' ') ++a;
      rv.assign(rv.substr(a, b - a + 1));
    }
    return rv;
  }
  const std::string& var = _args[0]->literal(ctx);
  b = var.length() - 1;
  while (b >= 0 && var[b] == ' ') --b;
  if (b < 0)
    return ctx.allocate(std::string());
  c = var.length();
  a = 0;
  while (a < c && var[a] == ' ') ++a;
  return ctx.allocate(var.substr(a, b - a + 1));
}

TRIMExpression * TRIMExpression::parse(Parser& p, Context& ctx)
{
  std::vector<Expression*> args;

  try
  {
    TokenPtr t = p.pop();
    if (t->code != '(')
      throw ParseError(EXC_PARSE_FUNC_ARG_NUM_S, KEYWORDS[FUNC_TRIM]);
    args.push_back(ParseExpression::expression(p, ctx));
    if (!ParseExpression::typeChecking(args.back(), Type::LITERAL, p, ctx))
      throw ParseError(EXC_PARSE_FUNC_ARG_TYPE_S, KEYWORDS[FUNC_TRIM]);
    assertClosedFunction(p, ctx, FUNC_TRIM);
    return new TRIMExpression(std::move(args));
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

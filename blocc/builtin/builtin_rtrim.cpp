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

#include "builtin_rtrim.h"
#include <blocc/parse_expression.h>
#include <blocc/exception_parse.h>
#include <blocc/context.h>
#include <blocc/parser.h>
#include <blocc/debug.h>

namespace bloc
{

std::string& RTRIMExpression::literal(Context & ctx) const
{
  int64_t a;
  if (_args[0]->isRvalue())
  {
    std::string& rv = _args[0]->literal(ctx);
    a = rv.length() - 1;
    while (a >= 0 && rv[a] == ' ') --a;
    if (a >= 0)
      rv.assign(rv.substr(0, a + 1));
    else
      rv.clear();
    return rv;
  }
  const std::string& var = _args[0]->literal(ctx);
  a = var.length() - 1;
  while (a >= 0 && var[a] == ' ') --a;
  if (a >= 0)
    return ctx.allocate(var.substr(0, a + 1));
  return ctx.allocate(std::string());
}

RTRIMExpression * RTRIMExpression::parse(Parser& p, Context& ctx)
{
  std::vector<Expression*> args;

  try
  {
    TokenPtr t = p.pop();
    if (t->code != '(')
      throw ParseError(EXC_PARSE_FUNC_ARG_NUM_S, KEYWORDS[FUNC_RTRIM]);
    args.push_back(ParseExpression::expression(p, ctx));
    if (!ParseExpression::typeChecking(args.back(), Type::LITERAL, p, ctx))
      throw ParseError(EXC_PARSE_FUNC_ARG_TYPE_S, KEYWORDS[FUNC_RTRIM]);
    assertClosedFunction(p, ctx, FUNC_RTRIM);
    return new RTRIMExpression(std::move(args));
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

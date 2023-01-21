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

#include "builtin_getsys.h"
#include <blocc/parse_expression.h>
#include <blocc/exception_parse.h>
#include <blocc/context.h>
#include <blocc/parser.h>

#include <climits>
#include <chrono>

namespace bloc
{

std::string& GETSYSExpression::literal(Context & ctx) const
{
  std::string& rv = _args[0]->literal(ctx);
  if (rv == "compatible")
    return ctx.allocate(std::to_string(ctx.compatible()));
  if (rv == "language")
    return ctx.allocate(std::string(ctx.language()));
  if (rv == "country")
    return ctx.allocate(std::string(ctx.country()));
  if (rv == "integer_max")
    return ctx.allocate(std::to_string(INT64_MAX));
  throw RuntimeError(EXC_RT_NOT_IMPLEMENTED);
}

GETSYSExpression * GETSYSExpression::parse(Parser& p, Context& ctx)
{
  std::vector<Expression*> args;

  try
  {
    TokenPtr t = p.pop();
    if (t->code != '(')
      throw ParseError(EXC_PARSE_FUNC_ARG_NUM_S, KEYWORDS[FUNC_GETSYS]);
    args.push_back(ParseExpression::expression(p, ctx));
    if (args.back()->type(ctx).level() > 0 ||
            args.back()->type(ctx).major() != Type::LITERAL)
      throw ParseError(EXC_PARSE_FUNC_ARG_TYPE_S, KEYWORDS[FUNC_GETSYS]);
    assertClosedFunction(p, ctx, FUNC_GETSYS);
    return new GETSYSExpression(std::move(args));
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

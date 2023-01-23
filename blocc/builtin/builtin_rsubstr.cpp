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

#include "builtin_rsubstr.h"
#include <blocc/parse_expression.h>
#include <blocc/exception_parse.h>
#include <blocc/context.h>
#include <blocc/parser.h>
#include <blocc/debug.h>

namespace bloc
{

std::string& RSUBSTRExpression::literal(Context & ctx) const
{
  int64_t a, c;
  if (_args[0]->isRvalue(ctx))
  {
    std::string& rv = _args[0]->literal(ctx);
    c = rv.size();
    a = std::max<int64_t>(std::min<int64_t>(_args[1]->integer(ctx), c), 0L);
    rv.assign(rv.substr(c - a));
    return rv;
  }
  const std::string& var = _args[0]->literal(ctx);
  c = var.size();
  a = std::max<int64_t>(std::min<int64_t>(_args[1]->integer(ctx), c), 0L);
  return ctx.allocate(var.substr(c - a));
}

RSUBSTRExpression * RSUBSTRExpression::parse(Parser& p, Context& ctx)
{
  std::vector<Expression*> args;

  try
  {
    TokenPtr t = p.pop();
    if (t->code != '(')
      throw ParseError(EXC_PARSE_FUNC_ARG_NUM_S, KEYWORDS[FUNC_RSUB]);
    args.push_back(ParseExpression::expression(p, ctx));
    if (!ParseExpression::typeChecking(args.back(), Type::LITERAL, p, ctx))
      throw ParseError(EXC_PARSE_FUNC_ARG_TYPE_S, KEYWORDS[FUNC_RSUB]);
    t = p.pop();
    if (t->code != Parser::CHAIN)
      throw ParseError(EXC_PARSE_FUNC_ARG_NUM_S, KEYWORDS[FUNC_RSUB]);
    args.push_back(ParseExpression::expression(p, ctx));
    if (!ParseExpression::typeChecking(args.back(), Type::INTEGER, p, ctx))
      throw ParseError(EXC_PARSE_FUNC_ARG_TYPE_S, KEYWORDS[FUNC_RSUB]);
    assertClosedFunction(p, ctx, FUNC_RSUB);
    return new RSUBSTRExpression(std::move(args));
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

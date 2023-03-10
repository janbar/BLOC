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

#include "builtin_readln.h"
#include <blocc/parse_expression.h>
#include <blocc/exception_parse.h>
#include <blocc/context.h>
#include <blocc/parser.h>
#include <blocc/readstdin.h>
#include <blocc/debug.h>

#include <cstdlib>

namespace bloc
{

std::string& READLNExpression::literal(Context & ctx) const
{
  int n = 1024;
  char * buf = new char[n];
  if (!_args.empty())
  {
    fputs(_args[0]->literal(ctx).c_str(), stdout);
    fflush(stdout);
  }
  n = bloc_readstdin(buf, n);
  /* discard nl */
  if (buf[n - 1] == '\n')
    --n;
  std::string& rv = ctx.allocate(std::string(buf, n));
  delete [] buf;
  return rv;
}

READLNExpression * READLNExpression::parse(Parser& p, Context& ctx)
{
  std::vector<Expression*> args;

  try
  {
    TokenPtr t = p.pop();
    if (t->code != '(')
      throw ParseError(EXC_PARSE_FUNC_ARG_NUM_S, KEYWORDS[FUNC_READLN]);
    if (p.front()->code != ')')
    {
      args.push_back(ParseExpression::expression(p, ctx));
      if (!ParseExpression::typeChecking(args.back(), Type::LITERAL, p, ctx))
        throw ParseError(EXC_PARSE_FUNC_ARG_TYPE_S, KEYWORDS[FUNC_READLN]);
    }
    assertClosedFunction(p, ctx, FUNC_READLN);
    return new READLNExpression(std::move(args));
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

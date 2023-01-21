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

#include "builtin_isnum.h"
#include <blocc/parse_expression.h>
#include <blocc/exception_parse.h>
#include <blocc/context.h>
#include <blocc/parser.h>

#include <cstring>
#include <cstdlib>

namespace bloc
{

bool ISNUMExpression::boolean(Context & ctx) const
{
  const Type& arg_type = _args[0]->type(ctx);
  if (arg_type.level() > 0)
    return false;
  switch (arg_type.major())
  {
  case Type::LITERAL:
    /* test for a decimal */
    try
    {
      double d = std::stod(_args[0]->literal(ctx));
      (void)d;
    }
    catch (...)
    {
      return false;
    }
    return true;
  case Type::INTEGER:
  case Type::NUMERIC:
    return true;
  default:
    return false;
  }
}

ISNUMExpression * ISNUMExpression::parse(Parser& p, Context& ctx)
{
  std::vector<Expression*> args;

  try
  {
    TokenPtr t = p.pop();
    if (t->code != '(')
      throw ParseError(EXC_PARSE_FUNC_ARG_NUM_S, KEYWORDS[FUNC_ISNUM]);
    args.push_back(ParseExpression::expression(p, ctx));
    assertClosedFunction(p, ctx, FUNC_ISNUM);
    return new ISNUMExpression(std::move(args));
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

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
#include <blocc/debug.h>

#include <cstring>
#include <cstdlib>

namespace bloc
{

Value& ISNUMExpression::value(Context & ctx) const
{
  Value& val = _args[0]->value(ctx);
  Value v;
  if (val.isNull() || val.type().level() > 0)
    v = Value(Bool(false));
  else
    switch (val.type().major())
    {
    case Type::LITERAL:
      /* test for a decimal */
      try
      {
        double d = std::stod(*val.literal());
        (void)d;
        v = Value(Bool(true));
      }
      catch (...)
      {
        v = Value(Bool(false));
      }
      break;
    case Type::TABCHAR:
      /* test for a decimal */
      try
      {
        double d = std::stod(std::string(val.tabchar()->data(), val.tabchar()->size()));
        (void)d;
        v = Value(Bool(true));
      }
      catch (...)
      {
        v = Value(Bool(false));
      }
      break;
    case Type::INTEGER:
    case Type::NUMERIC:
      v = Value(Bool(true));
      break;
    default:
      v = Value(Bool(false));
    }
  if (val.lvalue())
    return ctx.allocate(std::move(v));
  val.swap(Value(std::move(v)));
  return val;
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

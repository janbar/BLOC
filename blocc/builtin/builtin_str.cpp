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

#include "builtin_str.h"
#include <blocc/parse_expression.h>
#include <blocc/exception_parse.h>
#include <blocc/context.h>
#include <blocc/parser.h>
#include <blocc/debug.h>

namespace bloc
{

std::string& STRExpression::literal(Context & ctx) const
{
  if (_args.empty())
    return ctx.allocate(std::string());

  switch (_args[0]->type(ctx).major())
  {
  case Type::BOOLEAN:
    return ctx.allocate((std::string(_args[0]->boolean(ctx) ? BooleanExpression::TRUE : BooleanExpression::FALSE)));
  case Type::INTEGER:
    return ctx.allocate((IntegerExpression::readableInteger(_args[0]->integer(ctx))));
  case Type::NUMERIC:
    return ctx.allocate((NumericExpression::readableNumeric(_args[0]->numeric(ctx))));
  case Type::LITERAL:
    if (_args[0]->isRvalue(ctx))
      return _args[0]->literal(ctx);
    return ctx.allocate(std::string(_args[0]->literal(ctx)));
  case Type::TABCHAR:
  {
    const TabChar& tab = _args[0]->tabchar(ctx);
    return ctx.allocate(std::string(tab.data(), tab.size()));
  }
  default:
    throw RuntimeError(EXC_RT_FUNC_ARG_TYPE_S, KEYWORDS[FUNC_STR]);
  }
}

STRExpression * STRExpression::parse(Parser& p, Context& ctx)
{
  std::vector<Expression*> args;

  try
  {
    TokenPtr t = p.pop();
    if (t->code != '(')
      throw ParseError(EXC_PARSE_FUNC_ARG_NUM_S, KEYWORDS[FUNC_STR]);
    if (p.front()->code != ')')
    {
      args.push_back(ParseExpression::expression(p, ctx));
      if (args.back()->type(ctx).level() > 0)
        throw ParseError(EXC_PARSE_FUNC_ARG_TYPE_S, KEYWORDS[FUNC_STR]);
      switch (args.back()->type(ctx).major())
      {
      case Type::COMPLEX:
      case Type::ROWTYPE:
        throw ParseError(EXC_PARSE_FUNC_ARG_TYPE_S, KEYWORDS[FUNC_STR]);
      default:
        break;
      }
    }
    assertClosedFunction(p, ctx, FUNC_STR);
    return new STRExpression(std::move(args));
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

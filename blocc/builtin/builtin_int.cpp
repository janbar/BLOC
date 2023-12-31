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

#include "builtin_int.h"
#include <blocc/parse_expression.h>
#include <blocc/exception_parse.h>
#include <blocc/context.h>
#include <blocc/parser.h>
#include <blocc/debug.h>

#include <cmath>

namespace bloc
{

int64_t INTExpression::integer(Context & ctx) const
{
  int64_t l;
  switch (_args[0]->type(ctx).major())
  {
  case Type::LITERAL:
    try
    {
      l = (int64_t) std::stoll(_args[0]->literal(ctx));
    }
    catch (std::invalid_argument& e)
    {
      throw RuntimeError(EXC_RT_STRING_TO_NUM);
    }
    catch (std::out_of_range& e)
    {
      throw RuntimeError(EXC_RT_OUT_OF_RANGE);
    }
    break;
  case Type::NUMERIC:
    l = (int64_t)_args[0]->numeric(ctx);
    break;
  case Type::INTEGER:
    l = _args[0]->integer(ctx);
    break;
  default:
    throw RuntimeError(EXC_RT_FUNC_ARG_TYPE_S, KEYWORDS[oper]);
  }
  return l;
}

INTExpression * INTExpression::parse(Parser& p, Context& ctx)
{
  std::vector<Expression*> args;

  try
  {
    TokenPtr t = p.pop();
    if (t->code != '(')
      throw ParseError(EXC_PARSE_FUNC_ARG_NUM_S, KEYWORDS[FUNC_INT]);
    args.push_back(ParseExpression::expression(p, ctx));
    if (args.back()->type(ctx).level() > 0)
      throw ParseError(EXC_PARSE_FUNC_ARG_TYPE_S, KEYWORDS[FUNC_INT]);
    switch (args.back()->type(ctx).major())
    {
    case Type::NO_TYPE:
    case Type::LITERAL:
    case Type::INTEGER:
    case Type::NUMERIC:
      assertClosedFunction(p, ctx, FUNC_INT);
      return new INTExpression(std::move(args));
    default:
      break;
    }
    throw ParseError(EXC_PARSE_FUNC_ARG_TYPE_S, KEYWORDS[FUNC_INT]);
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

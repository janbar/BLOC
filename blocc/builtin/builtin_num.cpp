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

#include "builtin_num.h"
#include <blocc/parse_expression.h>
#include <blocc/exception_parse.h>
#include <blocc/context.h>
#include <blocc/parser.h>
#include <blocc/debug.h>

#include <cstring>

namespace bloc
{

double NUMExpression::numeric(Context & ctx) const
{
  double d;
  switch (_args[0]->type(ctx).major())
  {
  case Type::LITERAL:
    try
    {
      d = std::stod(_args[0]->literal(ctx));
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
  case Type::TABCHAR:
    try
    {
      TabChar& tmp = _args[0]->tabchar(ctx);
      d = std::stod(std::string(tmp.data(), tmp.size()));
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
  case Type::INTEGER:
    d = _args[0]->numeric(ctx);
    break;
  case Type::BOOLEAN:
    d = (_args[0]->boolean(ctx) ? 1.0 : 0.0);
    break;
  case Type::COMPLEX:
    d = (_args[0]->complex(ctx).typeId() != 0 ? 1.0 : 0.0);
    break;
  default:
    throw RuntimeError(EXC_RT_FUNC_ARG_TYPE_S, KEYWORDS[oper]);
  }
  return d;
}

NUMExpression * NUMExpression::parse(Parser& p, Context& ctx)
{
  std::vector<Expression*> args;

  try
  {
    TokenPtr t = p.pop();
    if (t->code != '(')
      throw ParseError(EXC_PARSE_FUNC_ARG_NUM_S, KEYWORDS[FUNC_NUM]);
    args.push_back(ParseExpression::expression(p, ctx));
    if (args.back()->type(ctx).level() > 0)
      throw ParseError(EXC_PARSE_FUNC_ARG_TYPE_S, KEYWORDS[FUNC_NUM]);
    switch (args.back()->type(ctx).major())
    {
    case Type::NO_TYPE: /* opaque */
    case Type::LITERAL:
    case Type::INTEGER:
    case Type::NUMERIC:
      assertClosedFunction(p, ctx, FUNC_NUM);
      return new NUMExpression(std::move(args));
    default:
      break;
    }
    throw ParseError(EXC_PARSE_FUNC_ARG_TYPE_S, KEYWORDS[FUNC_NUM]);
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

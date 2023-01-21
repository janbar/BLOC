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

#include "builtin_mod.h"
#include <blocc/parse_expression.h>
#include <blocc/exception_parse.h>
#include <blocc/context.h>
#include <blocc/parser.h>

#include <cmath>

namespace bloc
{

const Type& MODExpression::type (Context &ctx) const
{
  if (_args[0]->type(ctx) == Type::INTEGER && _args[1]->type(ctx) == Type::INTEGER)
    return IntegerExpression::type_static;
  return NumericExpression::type_static;
}

int64_t MODExpression::integer(Context & ctx) const
{
  int64_t l = _args[1]->integer(ctx);
  if (l == 0)
    throw RuntimeError(EXC_RT_DIVIDE_BY_ZERO);
  return _args[0]->integer(ctx) % l;
}

double MODExpression::numeric(Context & ctx) const
{
  double d = _args[1]->numeric(ctx);
  if (d == 0.0)
    throw RuntimeError(EXC_RT_DIVIDE_BY_ZERO);
  return std::fmod(_args[0]->numeric(ctx), d);
}

MODExpression * MODExpression::parse(Parser& p, Context& ctx)
{
  std::vector<Expression*> args;

  try
  {
    TokenPtr t = p.pop();
    if (t->code != '(')
      throw ParseError(EXC_PARSE_FUNC_ARG_NUM_S, KEYWORDS[FUNC_MOD]);
    args.push_back(ParseExpression::expression(p, ctx));
    if (!ParseExpression::typeChecking(args.back(), Type::NUMERIC, p, ctx))
      throw ParseError(EXC_PARSE_FUNC_ARG_TYPE_S, KEYWORDS[FUNC_MOD]);
    t = p.pop();
    if (t->code != Parser::CHAIN)
      throw ParseError(EXC_PARSE_FUNC_ARG_NUM_S, KEYWORDS[FUNC_MOD]);
    args.push_back(ParseExpression::expression(p, ctx));
    if (!ParseExpression::typeChecking(args.back(), Type::NUMERIC, p, ctx))
      throw ParseError(EXC_PARSE_FUNC_ARG_TYPE_S, KEYWORDS[FUNC_MOD]);
    assertClosedFunction(p, ctx, FUNC_MOD);
    return new MODExpression(std::move(args));
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

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

Value& RTRIMExpression::value(Context & ctx) const
{
  Value& val = _args[0]->value(ctx);

  switch (val.type().major())
  {
  case Type::NO_TYPE:
    if (val.lvalue())
      return ctx.allocate(Value(Value::type_literal));
    val.swap(Value(Value::type_literal));
    return val;
  case Type::LITERAL:
  {
    if (val.isNull())
      return val;
    int64_t a;
    Literal * rv = val.literal();
    a = rv->size() - 1;
    while (a >= 0 && rv->at(a) == ' ') --a;
    if (a >= 0)
    {
      if (val.lvalue())
        return ctx.allocate(Value(new Literal(rv->substr(0, a + 1))));
      val.literal()->assign(rv->substr(0, a + 1));
      return val;
    }
    if (val.lvalue())
      return ctx.allocate(Value(new Literal()));
    val.literal()->clear();
    return val;
  }
  default:
    throw RuntimeError(EXC_RT_FUNC_ARG_TYPE_S, KEYWORDS[oper]);
  }
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

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

#include "builtin_abs.h"
#include <blocc/parse_expression.h>
#include <blocc/exception_parse.h>
#include <blocc/context.h>
#include <blocc/parser.h>
#include <blocc/debug.h>

#include <cmath>
#include <complex>

namespace bloc
{
#define IMAGINARY_TO_COMPLEX(i) std::complex<Numeric>((i).a, (i).b)

Value& ABSExpression::value(Context & ctx) const
{
  Value& val = _args[0]->value(ctx);
  Value v(Value::type_numeric);

  switch (val.type().major())
  {
  case Type::NO_TYPE:
    break;
  case Type::INTEGER:
  {
    if (val.isNull())
      return val;
    Integer l = *val.integer();
    v = Value(Integer(l < 0 ? -l : l));
    break;
  }
  case Type::NUMERIC:
  {
    if (val.isNull())
      return val;
    v = Value(Numeric(std::abs(*val.numeric())));
    break;
  }
  case Type::IMAGINARY:
  {
    if (val.isNull())
      return val;
    v = Value(Numeric(std::abs(IMAGINARY_TO_COMPLEX(*val.imaginary()))));
    break;
  }
  default:
    throw RuntimeError(EXC_RT_FUNC_ARG_TYPE_S, KEYWORDS[oper]);
  }
  if (val.lvalue())
    return ctx.allocate(std::move(v));
  val.swap(Value(std::move(v)));
  return val;
}

ABSExpression * ABSExpression::parse(Parser& p, Context& ctx)
{
  std::vector<Expression*> args;

  try
  {
    TokenPtr t = p.pop();
    if (t->code != '(')
      throw ParseError(EXC_PARSE_FUNC_ARG_NUM_S, KEYWORDS[FUNC_ABS]);
    args.push_back(ParseExpression::expression(p, ctx));
    if (!ParseExpression::typeChecking(args.back(), Type::NUMERIC, p, ctx))
      throw ParseError(EXC_PARSE_FUNC_ARG_TYPE_S, KEYWORDS[FUNC_ABS]);
    assertClosedFunction(p, ctx, FUNC_ABS);
    return new ABSExpression(std::move(args));
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

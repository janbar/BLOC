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

#include "builtin_getsys.h"
#include <blocc/parse_expression.h>
#include <blocc/exception_parse.h>
#include <blocc/context.h>
#include <blocc/parser.h>
#include <blocc/debug.h>

#include <climits>
#include <chrono>

namespace bloc
{

namespace
{
#ifndef BYTE_ORDER
#define LITTLE_ENDIAN     1234
#define BIG_ENDIAN        4321
#define BYTE_ORDER        machine_bom
#endif
#define is_big_endian     (BYTE_ORDER == BIG_ENDIAN)
#define is_little_endian  (BYTE_ORDER == LITTLE_ENDIAN)

static int test_endianess() {
  unsigned int test = 1;
  return (*((char*)(&test))) ? LITTLE_ENDIAN : BIG_ENDIAN;
}

int machine_bom = test_endianess();
}

Value& GETSYSExpression::value(Context & ctx) const
{
  Value& val = _args[0]->value(ctx);
  Value v;

  switch (val.type().major())
  {
  case Type::NO_TYPE:
    break;
  case Type::LITERAL:
    if (val.isNull())
      break;
    else if (*(val.literal()) == "clock")
      v = Value(Numeric(ctx.timestamp()));
    else if (*(val.literal()) == "compatible")
      v = Value(Integer(Context::compatible()));
    else if (*(val.literal()) == "language")
      v = Value(new Literal(Context::language()));
    else if (*(val.literal()) == "country")
      v = Value(new Literal(Context::country()));
    else if (*(val.literal()) == "integer_max")
      v = Value(Integer(INT64_MAX));
    else if (*(val.literal()) == "integer_min")
      v = Value(Integer(INT64_MIN));
    else if (*(val.literal()) == "system")
#if defined(LIBBLOC_MSWIN)
      v = Value(new Literal("MSWIN"));
#elif defined(LIBBLOC_UNIX)
      v = Value(new Literal("UNIX"));
#else
      v = Value(Value::type_literal);
#endif
    else if (*(val.literal()) == "endianess")
      v = Value(new Literal((machine_bom == BIG_ENDIAN ? "BE" : "LE")));
    else
      throw RuntimeError(EXC_RT_NOT_IMPLEMENTED);
    break;
  default:
    throw RuntimeError(EXC_RT_FUNC_ARG_TYPE_S, KEYWORDS[oper]);
  }
  if (val.lvalue())
    return ctx.allocate(std::move(v));
  val.swap(Value(std::move(v)));
  return val;
}

GETSYSExpression * GETSYSExpression::parse(Parser& p, Context& ctx)
{
  std::vector<Expression*> args;

  try
  {
    TokenPtr t = p.pop();
    if (t->code != '(')
      throw ParseError(EXC_PARSE_FUNC_ARG_NUM_S, KEYWORDS[FUNC_GETSYS]);
    args.push_back(ParseExpression::expression(p, ctx));
    if (args.back()->type(ctx).level() > 0 ||
            args.back()->type(ctx).major() != Type::LITERAL)
      throw ParseError(EXC_PARSE_FUNC_ARG_TYPE_S, KEYWORDS[FUNC_GETSYS]);
    assertClosedFunction(p, ctx, FUNC_GETSYS);
    return new GETSYSExpression(std::move(args));
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

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

#include "builtin_b64dec.h"
#include "base64.h"
#include <blocc/parse_expression.h>
#include <blocc/exception_parse.h>
#include <blocc/context.h>
#include <blocc/parser.h>
#include <blocc/debug.h>

namespace bloc
{

Value& B64DECExpression::value(Context & ctx) const
{
  Value& arg0 = _args[0]->value(ctx);
  Value v(Value::type_literal);

  if (arg0.isNull())
    return ctx.allocate(Value(Value::type_literal));
  else
  {
    switch (arg0.type().major())
    {
    case Type::LITERAL:
    {
      v = Value(new TabChar());
      Literal * str = arg0.literal();
      b64decode(str->data(), str->size(), *v.tabchar());
      break;
    }
    case Type::TABCHAR:
    {
      v = Value(new TabChar());
      TabChar * raw = arg0.tabchar();
      b64decode(raw->data(), raw->size(), *v.tabchar());
      break;
    }
    default:
      throw RuntimeError(EXC_RT_FUNC_ARG_TYPE_S, KEYWORDS[FUNC_B64DEC]);
    }
  }
  if (arg0.lvalue())
    return ctx.allocate(std::move(v));
  arg0.swap(Value(std::move(v)));
  return arg0;
}

B64DECExpression * B64DECExpression::parse(Parser& p, Context& ctx)
{
  std::vector<Expression*> args;

  try
  {
    TokenPtr t = p.pop();
    if (t->code != '(')
      throw ParseError(EXC_PARSE_FUNC_ARG_NUM_S, KEYWORDS[FUNC_B64DEC]);
    args.push_back(ParseExpression::expression(p, ctx));
    const Type& type = args.back()->type(ctx);
    if (type.level() > 0)
      throw ParseError(EXC_PARSE_FUNC_ARG_TYPE_S, KEYWORDS[FUNC_B64DEC]);
    switch (type.major())
    {
    case Type::NO_TYPE:
    case Type::LITERAL:
    case Type::TABCHAR:
      break;
    default:
      throw ParseError(EXC_PARSE_FUNC_ARG_TYPE_S, KEYWORDS[FUNC_B64DEC]);
    }
    assertClosedFunction(p, ctx, FUNC_B64DEC);
    return new B64DECExpression(std::move(args));
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

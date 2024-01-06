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

#include "builtin_replace.h"
#include <blocc/parse_expression.h>
#include <blocc/exception_parse.h>
#include <blocc/context.h>
#include <blocc/parser.h>
#include <blocc/debug.h>

namespace bloc
{

Value& REPLACEExpression::value(Context & ctx) const
{
  Value& val = _args[0]->value(ctx);
  Value& a1 = _args[1]->value(ctx);
  Value v(Value::type_literal);

  switch (val.type().major())
  {
  case Type::NO_TYPE:
    break;
  case Type::LITERAL:
  {
    switch (a1.type().major())
    {
    case Type::NO_TYPE:
      return val;
    case Type::LITERAL:
      if (a1.isNull())
        return val;
      break;
    default:
      throw RuntimeError(EXC_RT_FUNC_ARG_TYPE_S, KEYWORDS[oper]);
    }
    if (val.isNull())
      return val;
    Value& a2 = _args[2]->value(ctx);
    switch (a2.type().major())
    {
    case Type::NO_TYPE: /* undef as null */
    case Type::LITERAL:
      break;
    default:
      throw RuntimeError(EXC_RT_FUNC_ARG_TYPE_S, KEYWORDS[oper]);
    }
    Literal * tmp = new Literal();
    size_t p = 0;
    while (p < val.literal()->size())
    {
      size_t e = val.literal()->find(*a1.literal(), p);
      tmp->append(*val.literal(), p, e - p);
      if (e != std::string::npos)
      {
        if (!a2.isNull())
          tmp->append(*a2.literal());
        p = e + a1.literal()->size();
      }
      else
        p = e;
    }
    v = Value(tmp);
    break;
  }
  default:
    throw RuntimeError(EXC_RT_FUNC_ARG_TYPE_S, KEYWORDS[oper]);
  }
  if (!val.lvalue())
  {
    val.swap(Value(std::move(v)));
    return val;
  }
  if (!a1.lvalue())
  {
    a1.swap(Value(std::move(v)));
    return a1;
  }
  return ctx.allocate(std::move(v));
}

REPLACEExpression * REPLACEExpression::parse(Parser& p, Context& ctx)
{
  std::vector<Expression*> args;

  try
  {
    TokenPtr t = p.pop();
    if (t->code != '(')
      throw ParseError(EXC_PARSE_FUNC_ARG_NUM_S, KEYWORDS[FUNC_REPSTR]);
    args.push_back(ParseExpression::expression(p, ctx));
    if (!ParseExpression::typeChecking(args.back(), Type::LITERAL, p, ctx))
      throw ParseError(EXC_PARSE_FUNC_ARG_TYPE_S, KEYWORDS[FUNC_REPSTR]);
    t = p.pop();
    if (t->code != Parser::CHAIN)
      throw ParseError(EXC_PARSE_FUNC_ARG_NUM_S, KEYWORDS[FUNC_REPSTR]);
    args.push_back(ParseExpression::expression(p, ctx));
    if (!ParseExpression::typeChecking(args.back(), Type::LITERAL, p, ctx))
      throw ParseError(EXC_PARSE_FUNC_ARG_TYPE_S, KEYWORDS[FUNC_REPSTR]);
    t = p.pop();
    if (t->code != Parser::CHAIN)
      throw ParseError(EXC_PARSE_FUNC_ARG_NUM_S, KEYWORDS[FUNC_REPSTR]);
    args.push_back(ParseExpression::expression(p, ctx));
    if (!ParseExpression::typeChecking(args.back(), Type::LITERAL, p, ctx))
      throw ParseError(EXC_PARSE_FUNC_ARG_TYPE_S, KEYWORDS[FUNC_REPSTR]);
    assertClosedFunction(p, ctx, FUNC_REPSTR);
    return new REPLACEExpression(std::move(args));
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

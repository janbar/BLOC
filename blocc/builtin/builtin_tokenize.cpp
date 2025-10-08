/*
 *      Copyright (C) 2025 Jean-Luc Barriere
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

#include "builtin_tokenize.h"
#include "blocc/collection.h"
#include <blocc/parse_expression.h>
#include <blocc/exception_parse.h>
#include <blocc/context.h>
#include <blocc/parser.h>
#include <blocc/debug.h>

namespace bloc
{

Value& TOKENIZEExpression::value(Context & ctx) const
{
  Value& val = _args[0]->value(ctx);

  switch (val.type().major())
  {
  case Type::NO_TYPE:
    if (val.lvalue())
      return ctx.allocate(Value(_type));
    val.swap(Value(_type));
    return val;
  case Type::LITERAL:
  {
    Literal sep;
    Value& a1 = _args[1]->value(ctx);
    switch (a1.type().major())
    {
    case Type::NO_TYPE:
      break;
    case Type::LITERAL:
      if (!a1.isNull())
        sep = *a1.literal();
      break;
    default:
      throw RuntimeError(EXC_RT_FUNC_ARG_TYPE_S, KEYWORDS[oper]);
    }
    bool trim = false;
    if (_args.size() > 2)
    {
      Value& a2 = _args[2]->value(ctx);
      if (!a2.isNull())
        trim = *a2.boolean();
    }
    if (val.lvalue())
    {
      if (val.isNull())
        return ctx.allocate(Value(_type));
      return ctx.allocate(Value(tokenize(*val.literal(), sep, trim)));
    }
    if (val.isNull())
      val.swap(Value(_type));
    else
      val.swap(Value(tokenize(*val.literal(), sep, trim)));
    return val;
  }
  default:
    throw RuntimeError(EXC_RT_FUNC_ARG_TYPE_S, KEYWORDS[oper]);
  }
}

TOKENIZEExpression * TOKENIZEExpression::parse(Parser& p, Context& ctx)
{
  std::vector<Expression*> args;

  try
  {
    TokenPtr t = p.pop();
    if (t->code != '(')
      throw ParseError(EXC_PARSE_FUNC_ARG_NUM_S, KEYWORDS[FUNC_TKNIZE], t);
    args.push_back(ParseExpression::expression(p, ctx));
    if (!ParseExpression::typeChecking(args.back(), Type::LITERAL, p, ctx))
      throw ParseError(EXC_PARSE_FUNC_ARG_TYPE_S, KEYWORDS[FUNC_TKNIZE], t);
    t = p.pop();
    if (t->code != Parser::Chain)
      throw ParseError(EXC_PARSE_FUNC_ARG_NUM_S, KEYWORDS[FUNC_TKNIZE], t);
    args.push_back(ParseExpression::expression(p, ctx));
    if (!ParseExpression::typeChecking(args.back(), Type::LITERAL, p, ctx))
      throw ParseError(EXC_PARSE_FUNC_ARG_TYPE_S, KEYWORDS[FUNC_TKNIZE], t);
    if (p.front()->code == Parser::Chain)
    {
      t = p.pop();
      args.push_back(ParseExpression::expression(p, ctx));
      if (!ParseExpression::typeChecking(args.back(), Type::BOOLEAN, p, ctx))
        throw ParseError(EXC_PARSE_FUNC_ARG_TYPE_S, KEYWORDS[FUNC_TKNIZE], t);
    }
    assertClosedFunction(p, ctx, FUNC_TKNIZE);
    return new TOKENIZEExpression(std::move(args));
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

Collection * TOKENIZEExpression::tokenize(const std::string& str, const std::string& sep, bool trimnull) const
{
  Collection * tokens = new Collection(_type);
  size_t pos = 0;
  size_t end = str.size();
  if (pos == end)
    return tokens;
  size_t lsep = sep.size();
  std::string token;
  while (pos != end)
  {
    if (lsep && str.compare(pos, lsep, sep) == 0)
    {
      // trim null token
      if (!trimnull || !token.empty())
      {
        tokens->push_back(Value(new Literal(std::move(token))));
        token.clear();
      }
      pos += lsep;
    }
    else
    {
      token.push_back(str[pos]);
      ++pos;
    }
  }
  if (!trimnull || !token.empty())
    tokens->push_back(Value(new Literal(std::move(token))));
  return tokens;
}

}

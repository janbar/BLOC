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

#include "builtin_tup.h"
#include <blocc/parse_expression.h>
#include <blocc/exception_parse.h>
#include <blocc/expression_tuple.h>
#include <blocc/context.h>
#include <blocc/parser.h>
#include <blocc/debug.h>

namespace bloc
{

TUPExpression::TUPExpression(std::vector<Expression*>&& args, Context& ctx) : BuiltinExpression(FUNC_TUP, std::move(args))
{
  for (const Expression * e : _args)
    _decl.push_back(e->type(ctx));
  _type = _decl.make_type(0);
}

Tuple& TUPExpression::tuple(Context & ctx) const
{
  /* build items, then move */
  Tuple::container_t items;
  for (const Expression * a : _args)
  {
    const Type& arg_type = a->type(ctx);
    switch(arg_type.major())
    {
    case Type::BOOLEAN:
      items.push_back(new BooleanExpression(a->boolean(ctx)));
      break;
    case Type::INTEGER:
      items.push_back(new IntegerExpression(a->integer(ctx)));
      break;
    case Type::NUMERIC:
      items.push_back(new NumericExpression(a->numeric(ctx)));
      break;
    case Type::LITERAL:
      if (a->isRvalue())
        items.push_back(new LiteralExpression(std::move(a->literal(ctx))));
      else
        items.push_back(new LiteralExpression(a->literal(ctx)));
      break;
    case Type::COMPLEX:
      if (a->isRvalue())
        items.push_back(new ComplexExpression(std::move(a->complex(ctx))));
      else
        items.push_back(new ComplexExpression(a->complex(ctx)));
      break;
    case Type::TABCHAR:
      if (a->isRvalue())
        items.push_back(new TabcharExpression(std::move(a->tabchar(ctx))));
      else
        items.push_back(new TabcharExpression(a->tabchar(ctx)));
      break;
    default:
      throw RuntimeError(EXC_RT_FUNC_ARG_TYPE_S, KEYWORDS[FUNC_TUP]);
    }
  }
  return ctx.allocate(Tuple(std::move(items)));
}

TUPExpression * TUPExpression::parse(Parser& p, Context& ctx)
{
  std::vector<Expression*> args;

  try
  {
    TokenPtr t = p.pop();
    if (t->code != '(')
      throw ParseError(EXC_PARSE_FUNC_ARG_NUM_S, KEYWORDS[FUNC_TUP]);
    for (;;)
    {
      args.push_back(ParseExpression::expression(p, ctx));
      const Type& arg_type = args.back()->type(ctx);
      if (arg_type.level() > 0)
        throw ParseError(EXC_PARSE_FUNC_ARG_TYPE_S, KEYWORDS[FUNC_TUP]);
      switch (arg_type.major())
      {
      case Type::NO_TYPE: /* opaque */
      case Type::BOOLEAN:
      case Type::INTEGER:
      case Type::NUMERIC:
      case Type::LITERAL:
      case Type::COMPLEX:
      case Type::TABCHAR:
        break;
      default:
        throw ParseError(EXC_PARSE_FUNC_ARG_TYPE_S, KEYWORDS[FUNC_TUP]);
      }
      t = p.pop();
      if (t->code == Parser::CHAIN)
        continue;
      p.push(t);
      break;
    }
    if (args.size() < 2)
      throw ParseError(EXC_PARSE_FUNC_ARG_NUM_S, KEYWORDS[FUNC_TUP]);
    assertClosedFunction(p, ctx, FUNC_TUP);
    return new TUPExpression(std::move(args), ctx);
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

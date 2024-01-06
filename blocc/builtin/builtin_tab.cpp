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

#include "builtin_tab.h"
#include <blocc/parse_expression.h>
#include <blocc/exception_parse.h>
#include <blocc/context.h>
#include <blocc/parser.h>
#include <blocc/plugin_manager.h>
#include <blocc/collection.h>
#include <blocc/tuple.h>
#include <blocc/debug.h>

namespace bloc
{

Value& TABExpression::value(Context & ctx) const
{
  if (_args.empty())
    return ctx.allocate(Value(Value::type_no_type.levelUp()));
  Value& a0 = _args[0]->value(ctx);
  if (a0.isNull())
    return ctx.allocate(Value(_args[1]->value(ctx).type().levelUp()));
  Integer n = *a0.integer();
  if (n < 0)
    throw RuntimeError(EXC_RT_INDEX_RANGE_S, std::to_string(n).c_str());

  Collection * tab = nullptr;
  do
  {
    Value& a1 = _args[1]->value(ctx); /* execute expression */
    if (tab == nullptr)
    {
      /* initialize the collection */
      if (a1.type().level() == TYPE_LEVEL_MAX - 1)
        throw RuntimeError(EXC_RT_OUT_OF_DIMENSION);
      /* initialize with the type of value */
      if (a1.type() != Type::ROWTYPE || a1.isNull())
        tab = new Collection(a1.type().levelUp());
      else if (a1.type().level() > 0)
        tab = new Collection(a1.collection()->table_decl(), a1.type().level()+1);
      else
        tab = new Collection(a1.tuple()->tuple_decl(), a1.type().level()+1);
      tab->reserve(n);
    }
    /* break now for an empty collection */
    if (n == 0)
      break;
    if (a1.lvalue())
      tab->push_back(a1.clone());
    else
      tab->push_back(std::move(a1));
  } while (--n > 0);
  return ctx.allocate(Value(tab));
}

std::string TABExpression::typeName(Context& ctx) const
{
  switch (type(ctx).major())
  {
  case Type::COMPLEX:
    return type(ctx).typeName(PluginManager::instance().plugged(type(ctx).minor()).interface.name);
  case Type::ROWTYPE:
    return type(ctx).typeName(tuple_decl(ctx).tupleName().c_str());
  default:
    return type(ctx).typeName();
  }
}

TABExpression * TABExpression::parse(Parser& p, Context& ctx)
{
  std::vector<Expression*> args;

  try
  {
    TokenPtr t = p.pop();
    if (t->code != '(')
      throw ParseError(EXC_PARSE_FUNC_ARG_NUM_S, KEYWORDS[FUNC_TAB]);
    if (p.front()->code != ')')
    {
      args.push_back(ParseExpression::expression(p, ctx));
      if (!ParseExpression::typeChecking(args.back(), Type::INTEGER, p, ctx))
        throw ParseError(EXC_PARSE_FUNC_ARG_TYPE_S, KEYWORDS[FUNC_TAB]);
      t = p.pop();
      if (t->code != Parser::CHAIN)
        throw ParseError(EXC_PARSE_FUNC_ARG_NUM_S, KEYWORDS[FUNC_TAB]);
      args.push_back(ParseExpression::expression(p, ctx));
      Type b_type = args.back()->type(ctx);
      switch (b_type.major())
      {
      case Type::NO_TYPE: /* opaque */
      case Type::BOOLEAN:
      case Type::INTEGER:
      case Type::NUMERIC:
      case Type::LITERAL:
      case Type::COMPLEX:
      case Type::TABCHAR:
      case Type::ROWTYPE:
        break;
      default:
        throw ParseError(EXC_PARSE_MEMB_ARG_TYPE_S, KEYWORDS[FUNC_TAB]);
      }
      if (b_type.level() == TYPE_LEVEL_MAX)
        throw ParseError(EXC_PARSE_OUT_OF_DIMENSION);
    }
    assertClosedFunction(p, ctx, FUNC_TAB);
    return new TABExpression(std::move(args));
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

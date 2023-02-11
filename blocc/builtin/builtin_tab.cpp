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
#include <blocc/expression_collection.h>
#include <blocc/expression_tuple.h>
#include <blocc/parse_expression.h>
#include <blocc/exception_parse.h>
#include <blocc/context.h>
#include <blocc/parser.h>
#include <blocc/plugin_manager.h>
#include <blocc/debug.h>

namespace bloc
{

Collection& TABExpression::collection(Context & ctx) const
{
  int64_t n = _args[0]->integer(ctx);
  if (n < 0)
    throw RuntimeError(EXC_RT_INDEX_RANGE_S, std::to_string(n).c_str());

  const Type& arg1_type = _args[1]->type(ctx);
  if (arg1_type.level() == TYPE_LEVEL_MAX - 1)
    throw RuntimeError(EXC_RT_OUT_OF_DIMENSION);

  Collection * tab;
  if (arg1_type.major() == Type::ROWTYPE)
    tab = &ctx.allocate(Collection(_args[1]->tuple_decl(ctx), arg1_type.level() + 1));
  else
    tab = &ctx.allocate(Collection(arg1_type.levelUp()));

  tab->reserve(n);

  if (arg1_type.level() == 0)
  {
    switch (arg1_type.major())
    {
    case Type::BOOLEAN:
    {
      bool b = _args[1]->boolean(ctx);
      for (int i = 0; i < n; ++i)
        tab->push_back(new BooleanExpression(b));
      break;
    }
    case Type::INTEGER:
    {
      int64_t l = _args[1]->integer(ctx);
      for (int i = 0; i < n; ++i)
        tab->push_back(new IntegerExpression(l));
      break;
    }
    case Type::NUMERIC:
    {
      double d = _args[1]->numeric(ctx);
      for (int i = 0; i < n; ++i)
        tab->push_back(new NumericExpression(d));
      break;
    }
    case Type::LITERAL:
    {
      const std::string& s = _args[1]->literal(ctx);
      for (int i = 0; i < n; ++i)
        tab->push_back(new LiteralExpression(s));
      break;
    }
    case Type::COMPLEX:
    {
      /* execute ctor or method for each */
      if (_args[1]->isRvalue())
      {
        for (int i = 0; i < n; ++i)
          tab->push_back(new ComplexExpression(std::move(_args[1]->complex(ctx))));
      }
      else
      {
        for (int i = 0; i < n; ++i)
          tab->push_back(new ComplexExpression(_args[1]->complex(ctx)));
      }
      break;
    }
    case Type::TABCHAR:
    {
      const TabChar& r = _args[1]->tabchar(ctx);
      for (int i = 0; i < n; ++i)
        tab->push_back(new TabcharExpression(r));
      break;
    }
    case Type::ROWTYPE:
    {
      /* tuple item could be complex ctor, so execute ctor for each */
      for (int i = 0; i < n; ++i)
        tab->push_back(new TupleExpression(_args[1]->tuple(ctx)));
      break;
    }
    default:
      throw RuntimeError(EXC_RT_MEMB_ARG_TYPE_S, KEYWORDS[FUNC_TAB]);
    }
  }
  else
  {
    const Collection& t = _args[1]->collection(ctx);
    for (int i = 0; i < n; ++i)
      tab->push_back(new CollectionExpression(t));
  }
  return *tab;
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

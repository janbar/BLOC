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

#include "expression_variable.h"
#include "exception_parse.h"
#include "parser.h"
#include "plugin_manager.h"
#include "complex.h"
#include "collection.h"
#include "tuple.h"

namespace bloc
{

const Type& VariableExpression::type(Context& ctx) const
{
  if (ctx.parsing())
    /* return the type registered in the context with this id */
    return *(ctx.getSymbol(_symbol.id()));

  /* return the type of the stored expression */
  Value& val = ctx.loadVariable(_symbol);
  if (val.type() != Type::POINTER)
    return val.type();
  return val.deref_value().type();
}

const TupleDecl::Decl& VariableExpression::tuple_decl(Context& ctx) const
{
  if (ctx.parsing())
    /* return the decl registered in the context with this id */
    return ctx.getSymbol(_symbol.id())->tuple_decl();
  else
  {
    /* return the decl of the stored expression */
    Value& z = ctx.loadVariable(_symbol);
    if (z.isNull())
      return TupleDecl::no_decl;
    if (z.type() != Type::POINTER)
    {
      if (z.type().level() > 0)
        return z.collection()->table_decl();
      return z.tuple()->tuple_decl();
    }
    if (z.value()->isNull())
      return TupleDecl::no_decl;
    if (z.value()->type().level() > 0)
      return z.value()->collection()->table_decl();
    return z.value()->tuple()->tuple_decl();
  }
}

std::string VariableExpression::typeName(Context& ctx) const
{
  const Type& t = type(ctx);
  switch (t.major())
  {
  case Type::COMPLEX:
    return t.typeName(PluginManager::instance().plugged(t.minor()).interface.name);
  case Type::ROWTYPE:
    return t.typeName(tuple_decl(ctx).tupleName());
  default:
    return t.typeName();
  }
}

VariableExpression * VariableExpression::parse(Parser& p, Context& ctx, TokenPtr& token)
{
  const Symbol * symbol = ctx.findSymbol(token->text);
  if (!symbol)
    throw ParseError(EXC_PARSE_UNDEFINED_SYMBOL_S, token->text.c_str(), token);
  return new VariableExpression(*symbol);
}

}

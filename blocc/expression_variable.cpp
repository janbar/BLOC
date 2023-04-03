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
#include "context.h"
#include "parser.h"
#include "plugin_manager.h"

namespace bloc
{

const Type& VariableExpression::type(Context& ctx) const {
  /* retrieve the type of the stored expression at runtime */
  const StaticExpression * z = ctx.loadVariable(_symbol);
  if (z == nullptr) {
    /* memory pointed by this variable has not been populated yet */
    /* so return the type fixed during semantic analysis */
    const Symbol * s = ctx.findSymbol(_symbol.name);
    if (s == nullptr)
      throw ParseError(EXC_PARSE_UNDEFINED_SYMBOL_S, _symbol.name.c_str());
    return *s;
  }
  return z->refType();
}

bool VariableExpression::boolean(Context& ctx) const {
  const StaticExpression * z = ctx.loadVariable(_symbol);
  if (z == nullptr)
    throw RuntimeError(EXC_RT_UNDEFINED_SYMBOL_S, _symbol.name.c_str());
  return z->boolean(ctx);
}

int64_t VariableExpression::integer(Context& ctx) const {
  const StaticExpression * z = ctx.loadVariable(_symbol);
  if (z == nullptr)
    throw RuntimeError(EXC_RT_UNDEFINED_SYMBOL_S, _symbol.name.c_str());
  return z->integer(ctx);
}

double VariableExpression::numeric(Context& ctx) const {
  const StaticExpression * z = ctx.loadVariable(_symbol);
  if (z == nullptr)
    throw RuntimeError(EXC_RT_UNDEFINED_SYMBOL_S, _symbol.name.c_str());
  return z->numeric(ctx);
}

std::string& VariableExpression::literal(Context& ctx) const {
  const StaticExpression * z = ctx.loadVariable(_symbol);
  if (z == nullptr)
    throw RuntimeError(EXC_RT_UNDEFINED_SYMBOL_S, _symbol.name.c_str());
  return z->literal(ctx);
}

TabChar& VariableExpression::tabchar(Context& ctx) const {
  const StaticExpression * z = ctx.loadVariable(_symbol);
  if (z == nullptr)
    throw RuntimeError(EXC_RT_UNDEFINED_SYMBOL_S, _symbol.name.c_str());
  return z->tabchar(ctx);
}

Collection& VariableExpression::collection(Context& ctx) const {
  const StaticExpression * z = ctx.loadVariable(_symbol);
  if (z == nullptr)
    throw RuntimeError(EXC_RT_UNDEFINED_SYMBOL_S, _symbol.name.c_str());
  return z->collection(ctx);
}

Tuple& VariableExpression::tuple(Context& ctx) const {
  const StaticExpression * z = ctx.loadVariable(_symbol);
  if (z == nullptr)
    throw RuntimeError(EXC_RT_UNDEFINED_SYMBOL_S, _symbol.name.c_str());
  return z->tuple(ctx);
}

const Tuple::Decl& VariableExpression::tuple_decl(Context& ctx) const
{
  const StaticExpression * z = ctx.loadVariable(_symbol);
  if (z == nullptr) {
    /* memory pointed by this variable has not been populated yet */
    /* so return the decl fixed during semantic analysis */
    const Symbol * s = ctx.findSymbol(_symbol.name);
    if (s == nullptr)
      throw ParseError(EXC_PARSE_UNDEFINED_SYMBOL_S, _symbol.name.c_str());
    return s->tuple_decl();
  }
  return z->tuple_decl(ctx);
}

Complex& VariableExpression::complex(Context& ctx) const {
  const StaticExpression * z = ctx.loadVariable(_symbol);
  if (z == nullptr)
    throw RuntimeError(EXC_RT_UNDEFINED_SYMBOL_S, _symbol.name.c_str());
  return z->complex(ctx);
}

std::string VariableExpression::typeName(Context& ctx) const {
  /* retrieve the type of the stored expression at runtime */
  const StaticExpression * z = ctx.loadVariable(_symbol);
  if (z == nullptr) {
    /* memory pointed by this variable has not been populated yet */
    /* so return the type fixed during semantic analysis */
    const Symbol * s = ctx.findSymbol(_symbol.name);
    if (s == nullptr)
      throw ParseError(EXC_PARSE_UNDEFINED_SYMBOL_S, _symbol.name.c_str());
    switch (s->major())
    {
    case Type::COMPLEX:
      return s->typeName(PluginManager::instance().plugged(s->minor()).interface.name);
    case Type::ROWTYPE:
      return s->tuple_decl().tupleName();
    default:
      return s->typeName();
    }
  }
  return z->typeName(ctx);
}

void VariableExpression::store(Context& ctx, Expression * exp) const
{
  if (exp != nullptr)
  {
    const Type& exp_type = exp->type(ctx);
    if (exp_type.level() == 0)
    {
      switch (exp_type.major())
      {
      case Type::NO_TYPE:
        break;
      case Type::BOOLEAN:
        ctx.storeVariable(_symbol, std::move(BooleanExpression(exp->boolean(ctx))));
        break;
      case Type::INTEGER:
        ctx.storeVariable(_symbol, std::move(IntegerExpression(exp->integer(ctx))));
        break;
      case Type::NUMERIC:
        ctx.storeVariable(_symbol, std::move(NumericExpression(exp->numeric(ctx))));
        break;
      case Type::COMPLEX:
      {
        if (exp->isRvalue())
          ctx.storeVariable(_symbol, std::move(ComplexExpression(std::move(exp->complex(ctx)))));
        else
          ctx.storeVariable(_symbol, std::move(ComplexExpression(exp->complex(ctx))));
        break;
      }
      case Type::LITERAL:
      {
        if (exp->isRvalue())
          ctx.storeVariable(_symbol, std::move(LiteralExpression(std::move(exp->literal(ctx)))));
        else
          ctx.storeVariable(_symbol, std::move(LiteralExpression(exp->literal(ctx))));
        break;
      }
      case Type::TABCHAR:
      {
        if (exp->isRvalue())
          ctx.storeVariable(_symbol, std::move(TabcharExpression(std::move(exp->tabchar(ctx)))));
        else
          ctx.storeVariable(_symbol, std::move(TabcharExpression(exp->tabchar(ctx))));
        break;
      }
      case Type::ROWTYPE:
      {
        if (exp->isRvalue())
          ctx.storeVariable(_symbol, std::move(TupleExpression(std::move(exp->tuple(ctx)))));
        else
          ctx.storeVariable(_symbol, std::move(TupleExpression(exp->tuple(ctx))));
        break;
      }
      }
    }
    else
    {
      /* first check for an rvalue, if not then make a copy */
      if (exp->isRvalue())
        ctx.storeVariable(_symbol, std::move(CollectionExpression(std::move(exp->collection(ctx)))));
      else
        ctx.storeVariable(_symbol, std::move(CollectionExpression(exp->collection(ctx))));
    }
  }
}

VariableExpression * VariableExpression::parse(Parser& p, Context& ctx, const std::string& symbol_name)
{
  std::string name(symbol_name);
  std::transform(name.begin(), name.end(), name.begin(), ::toupper);
  const Symbol * symbol = ctx.findSymbol(name);
  if (!symbol)
    throw ParseError(EXC_PARSE_UNDEFINED_SYMBOL_S, name.c_str());
  return new VariableExpression(*symbol);
}

}

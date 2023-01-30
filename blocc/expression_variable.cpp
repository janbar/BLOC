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
  StaticExpression * z = ctx.loadVariable(_symbol);
  if (z == nullptr) {
    /* memory pointed by this variable has not been populated yet */
    /* so return the type fixed during semantic analysis */
    Symbol * s = ctx.findSymbol(_symbol.name);
    if (s == nullptr)
      throw ParseError(EXC_PARSE_UNDEFINED_SYMBOL_S, _symbol.name.c_str());
    return *s;
  }
  return z->refType();
}

bool VariableExpression::boolean(Context& ctx) const {
  StaticExpression * z = ctx.loadVariable(_symbol);
  if (z == nullptr)
    throw RuntimeError(EXC_RT_UNDEFINED_SYMBOL_S, _symbol.name.c_str());
  return z->boolean(ctx);
}

int64_t VariableExpression::integer(Context& ctx) const {
  StaticExpression * z = ctx.loadVariable(_symbol);
  if (z == nullptr)
    throw RuntimeError(EXC_RT_UNDEFINED_SYMBOL_S, _symbol.name.c_str());
  return z->integer(ctx);
}

double VariableExpression::numeric(Context& ctx) const {
  StaticExpression * z = ctx.loadVariable(_symbol);
  if (z == nullptr)
    throw RuntimeError(EXC_RT_UNDEFINED_SYMBOL_S, _symbol.name.c_str());
  return z->numeric(ctx);
}

std::string& VariableExpression::literal(Context& ctx) const {
  StaticExpression * z = ctx.loadVariable(_symbol);
  if (z == nullptr)
    throw RuntimeError(EXC_RT_UNDEFINED_SYMBOL_S, _symbol.name.c_str());
  return z->literal(ctx);
}

TabChar& VariableExpression::tabchar(Context& ctx) const {
  StaticExpression * z = ctx.loadVariable(_symbol);
  if (z == nullptr)
    throw RuntimeError(EXC_RT_UNDEFINED_SYMBOL_S, _symbol.name.c_str());
  return z->tabchar(ctx);
}

Collection& VariableExpression::collection(Context& ctx) const {
  StaticExpression * z = ctx.loadVariable(_symbol);
  if (z == nullptr)
    throw RuntimeError(EXC_RT_UNDEFINED_SYMBOL_S, _symbol.name.c_str());
  return z->collection(ctx);
}

Tuple& VariableExpression::tuple(Context& ctx) const {
  StaticExpression * z = ctx.loadVariable(_symbol);
  if (z == nullptr)
    throw RuntimeError(EXC_RT_UNDEFINED_SYMBOL_S, _symbol.name.c_str());
  return z->tuple(ctx);
}

const Tuple::Decl& VariableExpression::tuple_decl(Context& ctx) const
{
  StaticExpression * z = ctx.loadVariable(_symbol);
  if (z == nullptr) {
    /* memory pointed by this variable has not been populated yet */
    /* so return the decl fixed during semantic analysis */
    Symbol * s = ctx.findSymbol(_symbol.name);
    if (s == nullptr)
      throw ParseError(EXC_PARSE_UNDEFINED_SYMBOL_S, _symbol.name.c_str());
    return s->tuple_decl();
  }
  return z->tuple_decl(ctx);
}

Complex VariableExpression::complex(Context& ctx) const {
  StaticExpression * z = ctx.loadVariable(_symbol);
  if (z == nullptr)
    throw RuntimeError(EXC_RT_UNDEFINED_SYMBOL_S, _symbol.name.c_str());
  return z->complex(ctx);
}

std::string VariableExpression::typeName(Context& ctx) const {
  /* retrieve the type of the stored expression at runtime */
  StaticExpression * z = ctx.loadVariable(_symbol);
  if (z == nullptr) {
    /* memory pointed by this variable has not been populated yet */
    /* so return the type fixed during semantic analysis */
    Symbol * s = ctx.findSymbol(_symbol.name);
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

VariableExpression * VariableExpression::parse(Parser& p, Context& ctx, const std::string& symbol_name)
{
  std::string name(symbol_name);
  std::transform(name.begin(), name.end(), name.begin(), ::toupper);
  Symbol * symbol = ctx.findSymbol(name);
  if (!symbol)
    throw ParseError(EXC_PARSE_UNDEFINED_SYMBOL_S, name.c_str());
  return new VariableExpression(*symbol);
}

}

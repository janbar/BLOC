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

const Type& VariableExpression::type(Context& ctx) const
{
  if (ctx.parsing())
    /* return the type registered in the context with this id */
    return *(ctx.getSymbol(_symbol.id));
  else
  {
    /* return the type of the stored expression */
    const StaticExpression * z = ctx.loadVariable(_symbol);
    if (z)
      return z->refType();
    return *(ctx.getSymbol(_symbol.id));
  }
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
  if (ctx.parsing())
    /* return the decl registered in the context with this id */
    return ctx.getSymbol(_symbol.id)->tuple_decl();
  else
  {
    /* return the decl of the stored expression */
    const StaticExpression * z = ctx.loadVariable(_symbol);
    if (z)
      return z->tuple_decl(ctx);
    return ctx.getSymbol(_symbol.id)->tuple_decl();
  }
}

Complex& VariableExpression::complex(Context& ctx) const {
  const StaticExpression * z = ctx.loadVariable(_symbol);
  if (z == nullptr)
    throw RuntimeError(EXC_RT_UNDEFINED_SYMBOL_S, _symbol.name.c_str());
  return z->complex(ctx);
}

std::string VariableExpression::typeName(Context& ctx) const
{
  const Symbol * s = ctx.getSymbol(_symbol.id);
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

void VariableExpression::store(Context& d_ctx, Context& s_ctx, Expression * s_exp) const
{
  if (s_exp != nullptr)
  {
    const Type& exp_type = s_exp->type(s_ctx);
    StaticExpression * exp = load(d_ctx);
    if (exp && exp->type(d_ctx) == exp_type)
    {
      if (exp_type.level() == 0)
      {
        switch (exp_type.major())
        {
        case Type::NO_TYPE:
          throw RuntimeError(EXC_RT_OPAQUE_INLINE);

        case Type::BOOLEAN:
          exp->refBoolean() = s_exp->boolean(s_ctx);
          break;
        case Type::INTEGER:
          exp->refInteger() = s_exp->integer(s_ctx);
          break;
        case Type::NUMERIC:
          exp->refNumeric() = s_exp->numeric(s_ctx);
          break;
        case Type::COMPLEX:
        {
          if (s_exp->isRvalue())
            exp->refComplex().swap(std::move(s_exp->complex(s_ctx)));
          else
            exp->refComplex() = s_exp->complex(s_ctx);
          break;
        }
        case Type::LITERAL:
        {
          if (s_exp->isRvalue())
            exp->refLiteral().swap(s_exp->literal(s_ctx));
          else
            exp->refLiteral() = s_exp->literal(s_ctx);
          break;
        }
        case Type::TABCHAR:
        {
          if (s_exp->isRvalue())
            exp->refTabchar().swap(s_exp->tabchar(s_ctx));
          else
            exp->refTabchar() = s_exp->tabchar(s_ctx);
          break;
        }
        case Type::ROWTYPE:
        {
          if (s_exp->isRvalue())
            exp->refTuple().swap(s_exp->tuple(s_ctx));
          else
            exp->refTuple().copy(s_exp->tuple(s_ctx));
          break;
        }
        }
      }
      else
      {
        /* first check for an rvalue, if not then make a copy */
        if (s_exp->isRvalue())
          exp->refCollection().swap(s_exp->collection(s_ctx));
        else
          exp->refCollection().copy(s_exp->collection(s_ctx));
      }
    }
    else
    {
      if (exp_type.level() == 0)
      {
        switch (exp_type.major())
        {
        case Type::NO_TYPE:
          throw RuntimeError(EXC_RT_OPAQUE_INLINE);

        case Type::BOOLEAN:
          d_ctx.storeVariable(_symbol, std::move(BooleanExpression(s_exp->boolean(s_ctx))));
          break;
        case Type::INTEGER:
          d_ctx.storeVariable(_symbol, std::move(IntegerExpression(s_exp->integer(s_ctx))));
          break;
        case Type::NUMERIC:
          d_ctx.storeVariable(_symbol, std::move(NumericExpression(s_exp->numeric(s_ctx))));
          break;
        case Type::COMPLEX:
        {
          if (s_exp->isRvalue())
            d_ctx.storeVariable(_symbol, std::move(ComplexExpression(std::move(s_exp->complex(s_ctx)))));
          else
            d_ctx.storeVariable(_symbol, std::move(ComplexExpression(s_exp->complex(s_ctx))));
          break;
        }
        case Type::LITERAL:
        {
          if (s_exp->isRvalue())
            d_ctx.storeVariable(_symbol, std::move(LiteralExpression(std::move(s_exp->literal(s_ctx)))));
          else
            d_ctx.storeVariable(_symbol, std::move(LiteralExpression(s_exp->literal(s_ctx))));
          break;
        }
        case Type::TABCHAR:
        {
          if (s_exp->isRvalue())
            d_ctx.storeVariable(_symbol, std::move(TabcharExpression(std::move(s_exp->tabchar(s_ctx)))));
          else
            d_ctx.storeVariable(_symbol, std::move(TabcharExpression(s_exp->tabchar(s_ctx))));
          break;
        }
        case Type::ROWTYPE:
        {
          if (s_exp->isRvalue())
            d_ctx.storeVariable(_symbol, std::move(TupleExpression(std::move(s_exp->tuple(s_ctx)))));
          else
            d_ctx.storeVariable(_symbol, std::move(TupleExpression(s_exp->tuple(s_ctx))));
          break;
        }
        }
      }
      else
      {
        /* first check for an rvalue, if not then make a copy */
        if (s_exp->isRvalue())
          d_ctx.storeVariable(_symbol, std::move(CollectionExpression(std::move(s_exp->collection(s_ctx)))));
        else
          d_ctx.storeVariable(_symbol, std::move(CollectionExpression(s_exp->collection(s_ctx))));
      }
    }
  }
}

VariableExpression * VariableExpression::parse(Parser& p, Context& ctx, const std::string& symbol_name)
{
  const Symbol * symbol = ctx.findSymbol(symbol_name);
  if (!symbol)
    throw ParseError(EXC_PARSE_UNDEFINED_SYMBOL_S, symbol_name.c_str());
  return new VariableExpression(*symbol);
}

}

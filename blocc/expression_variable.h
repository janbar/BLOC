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

#ifndef EXPRESSION_VARIABLE_H
#define EXPRESSION_VARIABLE_H

#include "expression.h"
#include "context.h"

#include <string>

namespace bloc
{

class Parser;

/**
 * This class implements an expression that is a named reference to static
 * expression in the storage, or more simply a variable.
 *
 */
class VariableExpression : public Expression
{
private:
  const Symbol& _symbol;

public:
  virtual ~VariableExpression() { }
  explicit VariableExpression(const Symbol& symbol) : Expression(), _symbol(symbol) { }

  const Type& type(Context& ctx) const override;

  const TupleDecl::Decl& tuple_decl(Context& ctx) const override;

  Value& value(Context& ctx) const override
  {
    Value& val = ctx.loadVariable(_symbol);
    if (val.type() != Type::POINTER)
      return val;
    return val.deref_value();
  }

  const Symbol * symbol() const override { return &_symbol; }

  std::string unparse(Context& ctx) const override
  {
    return _symbol.name();
  }

  std::string toString(Context& ctx) const override
  {
    return std::string("variable ").append(_symbol.name());
  }

  std::string typeName(Context& ctx) const override;

  /**
   * Store a temporary value into memory pointed by this variable.
   * @param ctx
   * @param val
   * @return a reference to the stored value
   */
  Value& store(Context& ctx, Value&& val) const
  {
    return ctx.storeVariable(_symbol, std::move(val));
  }

  /**
   * Execute the given expression, then store the result into memory pointed by
   * this variable. This helper should be run once and only once for the given
   * expression.
   * @param d_ctx storage context
   * @param s_ctx running context
   * @param s_exp
   */
  void store(Context& d_ctx, Context& s_ctx, Expression * s_exp) const
  {
    d_ctx.storeVariable(_symbol, std::move(s_exp->value(s_ctx)));
  }

  /**
   * Returns the reference of the content in memory pointed by this variable.
   * Obviously never assigned variable returns null.
   * @param ctx
   * @return the reference of the stored expression or null
   */
  Value& load(Context& ctx) const
  {
    return ctx.loadVariable(_symbol);
  }

  void clear(Context& ctx)
  {
    ctx.clearVariable(_symbol);
  }

  static VariableExpression * parse(Parser& p, Context& ctx, const std::string& symbol_name);
};

}

#endif /* EXPRESSION_VARIABLE_H */

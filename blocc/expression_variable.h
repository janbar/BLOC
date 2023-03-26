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

  bool boolean (Context& ctx) const override;

  int64_t integer(Context& ctx) const override;

  double numeric(Context& ctx) const override;

  std::string& literal(Context& ctx) const override;

  TabChar& tabchar(Context& ctx) const override;

  Collection& collection(Context& ctx) const override;

  Tuple& tuple(Context& ctx) const override;

  const Tuple::Decl& tuple_decl(Context& ctx) const override;

  Complex& complex(Context& ctx) const override;

  bool isStored() const override { return true; }

  std::string unparse(Context& ctx) const override
  {
    return _symbol.name;
  }

  std::string toString(Context& ctx) const override
  {
    return std::string("variable ").append(_symbol.name);
  }

  std::string typeName(Context& ctx) const override;

  /**
   * Returns the symbol name of this variable.
   * @return name
   */
  const std::string& symbolName() const { return _symbol.name; }

  /**
   * Store a temporary result into memory pointed by this variable. The result
   * instance must be derived from the class "StaticExpression" such as the
   * following types:
   *   BooleanExpression, IntegerExpression, NumericExpression,
   *   LiteralExpression, ComplexExpression
   *
   * @param ctx
   * @param exp
   * @return a reference to the stored expression
   */
  StaticExpression& store(Context& ctx, StaticExpression&& exp) const
  {
    return ctx.storeVariable(_symbol, std::move(exp));
  }

  /**
   * Returns the reference of the content in memory pointed by this variable.
   * Obviously never assigned variable returns null.
   * @param ctx
   * @return the reference of the stored expression or null
   */
  StaticExpression * load(Context& ctx) const
  {
    return ctx.loadVariable(_symbol);
  }

  static VariableExpression * parse(Parser& p, Context& ctx, const std::string& symbol_name);
};

}

#endif /* EXPRESSION_VARIABLE_H */

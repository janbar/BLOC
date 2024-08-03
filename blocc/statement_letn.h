/*
 *      Copyright (C) 2024 Jean-Luc Barriere
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

#ifndef STATEMENT_LETN_H
#define STATEMENT_LETN_H

#include "statement.h"
#include "expression_variable.h"

#include <string>

namespace bloc
{

class Parser;
class Context;
class Expression;

/**
 * The LET statement is the standard variable declaration and assignment.
 * Syntax is: let {var} = {expression}
 *
 * However we allow you to omit the LET and right value for null assignment.
 * {var} : {type name}.
 */
class LETNStatement : public Statement
{
  VariableExpression _var;
  Type _typ;

public:
  virtual ~LETNStatement() = default;

  LETNStatement(const VariableExpression& var, const Type& typ)
  : Statement(STMT_LET), _var(var), _typ(typ) { }

  const Statement * doit(Context& ctx) const override;

  void unparse(Context& ctx, FILE * out) const override;

  /**
   * Parse LETN Statement.
   */
  static LETNStatement * parse(Parser& p, Context& ctx);
};

}

#endif /* STATEMENT_LETN_H */

/*
 *      Copyright (C) 2023 Jean-Luc Barriere
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

#ifndef STATEMENT_FUNCTION_H
#define STATEMENT_FUNCTION_H

#include "statement.h"
#include "functor_manager.h"

#include <string>

namespace bloc
{

class Parser;
class Context;
class Expression;
class VariableExpression;
class Executable;

/**
 * The FUNCTION statement.
 * The syntax is:
 * function {name} ( [{var} , ...] ) returns {type name} is
 * begin
 *     [statement ...]
 * end
 */
class FUNCTIONStatement : public Statement
{
private:
  FunctorPtr _functor;

public:
  virtual ~FUNCTIONStatement();

  FUNCTIONStatement(const FunctorPtr& functor) : Statement(STMT_FUNCTION), _functor(functor) { }

  const Statement * doit(Context& ctx) const override;

  void unparse(Context& ctx, FILE * out) const override;

  /**
   * Parse FUNCTION Statement.
   */
  static FUNCTIONStatement * parse(Parser& p, Context& ctx);
};

}

#endif /* STATEMENT_FUNCTION_H */

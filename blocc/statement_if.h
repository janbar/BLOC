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

#ifndef STATEMENT_IF_H
#define STATEMENT_IF_H

#include "statement.h"

#include <string>
#include <list>

namespace bloc
{

class Parser;
class Context;
class Expression;
class Executable;

/**
 * The IF statement provides for conditional control flow in execution.
 * It has a boolean expression associated with it that is evaluated at runtime.
 * If the result of that evaluation is a 'true' value, then statements beyond
 * the THEN keyword will be executed until end delimited by ELSIF or ELSE or
 * END IF.
 * The syntax is:
 * if {boolean expression} then [statement ...]
 * [ elsif {boolean expression} then [statement ...] ...]
 * [ else [statement ...] ]
 * end if
 */
class IFStatement : public Statement
{
private:
  std::list<std::pair<Expression*, Executable*> > _rules;
  static Executable * parse_clause(Parser& p, Context& ctx);

public:
  virtual ~IFStatement();

  IFStatement() : Statement(STMT_IF) { }

  const Statement * doit(Context& ctx) const override;

  void unparse(Context& ctx, FILE * out) const override;

  /**
   * Parse FOR Statement.
   */
  static IFStatement * parse(Parser& p, Context& ctx);
};

}

#endif /* STATEMENT_IF_H */

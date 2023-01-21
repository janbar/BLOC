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

#ifndef STATEMENT_WHILE_H
#define STATEMENT_WHILE_H

#include "statement.h"

#include <string>

namespace bloc
{

class Parser;
class Context;
class Expression;
class Executable;

/**
 * The WHILE statement provides for loop flow in execution.
 * It has a boolean expression associated with it that is evaluated at runtime.
 * While the result of that evaluation is a 'true' value, then statement after
 * the LOOP keyword will be executed. If multiple statements follow the LOOP
 * keyword, they are executed in sequence but only while the expression is true.
 * The syntax is:
 * while {boolean expression} loop [statement ...] end loop
 */
class WHILEStatement : public Statement
{
private:
  Expression * exp = nullptr;
  Executable * _exec = nullptr;

  static Executable * parse_clause(Parser& p, Context& ctx, Statement * ihw);

public:
  virtual ~WHILEStatement();

  WHILEStatement() : Statement(STMT_WHILE) { }

  const Statement * doit(Context& ctx) const override;

  void unparse(Context& ctx, FILE * out) const override;

  /**
   * Parse WHILE Statement.
   */
  static WHILEStatement * parse(Parser& p, Context& ctx);
};

}

#endif /* STATEMENT_WHILE_H */

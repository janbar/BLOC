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

#ifndef STATEMENT_FOR_H
#define STATEMENT_FOR_H

#include "statement.h"
#include "executable.h"

#include <string>

namespace bloc
{

class Parser;
class Context;
class Expression;
class VariableExpression;
class Executable;

/**
 * The FOR statement provides iterator construct. It defines a control variable
 * that is initialized to an initial value. Then block of statements after the
 * LOOP keyword will be executed while the specified limit isn't reached. The
 * control value is incremented or decremented after each iteration.
 * The syntax is:
 * for {var} in {integer expression} to {integer expression} loop
 *     [statement ...]
 * end loop
 */
class FORStatement : public Statement
{
private:
  VariableExpression * _iterator = nullptr;
  Expression * _expBeg = nullptr;
  Expression * _expEnd = nullptr;
  Executable * _exec = nullptr;

  mutable struct RT
  {
    int64_t min = 0;
    int64_t max = 0;
    int64_t step = 1;
    StaticExpression * var = nullptr;
  } _data;

  static Executable * parse_clause(Parser& p, Context& ctx, FORStatement * rof);

public:
  virtual ~FORStatement();

  FORStatement() : Statement(STMT_FOR) { }

  const Statement * doit(Context& ctx) const override;

  void unparse(Context& ctx, FILE * out) const override;

  /**
   * Parse FOR Statement.
   */
  static FORStatement * parse(Parser& p, Context& ctx);
};

}

#endif /* STATEMENT_FOR_H */

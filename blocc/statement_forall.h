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

#ifndef STATEMENT_FORALL_H
#define STATEMENT_FORALL_H

#include "statement.h"
#include "executable.h"
#include "expression_iterator.h"

#include <string>

namespace bloc
{

class Parser;
class Context;
class Expression;
class VariableExpression;
class Executable;

/**
 * The FORALL statement provides row fetch construct. It defines a control
 * variable that is store current index. Then block of statements after the
 * LOOP keyword will be executed until last row is reached.
 * The syntax is:
 * forall {var} in {table expression} [asc|desc] loop
 *     [statement ...]
 * end loop
 */
class FORALLStatement : public Statement
{
private:
  VariableExpression * _var = nullptr;
  Expression * _exp = nullptr;
  Executable * _exec = nullptr;
  Symbol * _expSymbol = nullptr; // not freeable
  enum { AUTO = 0, ASC, DESC } _order = AUTO;

  mutable struct RT
  {
    Collection * table = nullptr;
    int64_t step = 1;
    IteratorExpression * iterator = nullptr;
  } _data;

  static Executable * parse_clause(Parser& p, Context& ctx, FORALLStatement * rof);

public:
  virtual ~FORALLStatement();

  FORALLStatement() : Statement(STMT_FORALL) { }

  const Statement * doit(Context& ctx) const override;

  void unparse(Context& ctx, FILE * out) const override;

  /**
   * Parse FOR Statement.
   */
  static FORALLStatement * parse(Parser& p, Context& ctx);
};

}

#endif /* STATEMENT_FORALL_H */

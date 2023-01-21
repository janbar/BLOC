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

#ifndef STATEMENT_PUT_H
#define STATEMENT_PUT_H

#include "statement.h"

#include <string>
#include <vector>

namespace bloc
{

class Parser;
class Context;
class Expression;

/**
 * The PUT statement writes out the string value of an expression.
 * Syntax is: put {expression}
 *
 * Only boolean, integer, numeric, and literal expression can be written out.
 * For others types we have to convert them using function STR(expression),
 * i.e : put str( raw | complex ).
 */
class PUTStatement : public Statement
{
  std::vector<Expression*> _args;

public:
  virtual ~PUTStatement();

  PUTStatement() : Statement(STMT_PUT) { }

  const Statement * doit(Context& ctx) const override;

  void unparse(Context& ctx, FILE * out) const override;

  /**
   * Parse PUT Statement.
   */
  static PUTStatement * parse(Parser& p, Context& ctx);
};

}

#endif /* STATEMENT_PUT_H */

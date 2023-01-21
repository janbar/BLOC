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

#ifndef STATEMENT_PRINT_H
#define STATEMENT_PRINT_H

#include "statement.h"

#include <string>
#include <vector>

namespace bloc
{

class Parser;
class Context;
class Expression;

/**
 * The PRINT statement writes out the string representation of an expression,
 * terminated by a newline.
 * Syntax is: print {expression}
 *
 * The printed value is a human readable text, depending of expression type.
 */
class PRINTStatement : public Statement
{
  std::vector<Expression*> _args;

public:

  virtual ~PRINTStatement();

  PRINTStatement() : Statement(STMT_PRINT) { }

  const Statement * doit(Context& ctx) const override;

  void unparse(Context& ctx, FILE * out) const override;

  /**
   * Parse PRINT Statement.
   */
  static PRINTStatement * parse(Parser& p, Context& ctx);
};

}

#endif /* STATEMENT_PRINT_H */

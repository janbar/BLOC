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

#ifndef STATEMENT_CONTINUE_H
#define STATEMENT_CONTINUE_H

#include "statement.h"
#include "parser.h"

#include <string>

namespace bloc
{

class Parser;
class Context;
class Expression;

/**
 * The CONTINUE statement "jumps over" one iteration in the loop.
 * Syntax is: continue
 */
class CONTINUEStatement : public Statement
{
public:
  virtual ~CONTINUEStatement() { }

  CONTINUEStatement() : Statement(STMT_CONTINUE) { }

  const Statement * doit(Context& ctx) const override
  {
    if (ctx.topControl())
      ctx.continueCondition(true);
    return _next;
  }

  /**
   * Parse CONTINUE Statement.
   */
  static CONTINUEStatement * parse(Parser& p, Context& ctx)
  {
    return new CONTINUEStatement();
  }
};

}

#endif /* STATEMENT_CONTINUE_H */

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

#ifndef STATEMENT_END_H
#define STATEMENT_END_H

#include "statement.h"
#include "token.h"
#include "parser.h"

#include <string>

namespace bloc
{

class Parser;
class Context;
class Expression;

/**
 * The END statement close the latest block of statements.
 */
class ENDStatement : public Statement
{
public:
  virtual ~ENDStatement() { }

  explicit ENDStatement(STATEMENT endof) : Statement(endof) { }

  const Statement * doit(Context& ctx) const override
  {
    return _next;
  }

  /**
   * Parse END Statement.
   */
  static ENDStatement * parse(Parser& p, Context& ctx, STATEMENT endof);
};

}

#endif /* STATEMENT_END_H */

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

#ifndef STATEMENT_RETURN_H
#define STATEMENT_RETURN_H

#include "statement.h"

#include <string>

namespace bloc
{

class Parser;
class Context;
class Expression;

/**
 * The RETURN statement exit from program returning a payload value or not.
 * Syntax is:
 * return [expression]
 *
 * Only boolean, integer, numeric, and literal expression can be returned.
 */
class RETURNStatement : public Statement
{
  Expression * _exp = nullptr;

public:
  virtual ~RETURNStatement();

  RETURNStatement() : Statement(STMT_RETURN) { }

  const Statement * doit(Context& ctx) const override;

  void unparse(Context& ctx, FILE * out) const override;

  /**
   * Parse RETURN Statement.
   */
  static RETURNStatement * parse(Parser& p, Context& ctx);
};

}

#endif /* STATEMENT_RETURN_H */

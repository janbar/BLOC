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

#ifndef STATEMENT_RAISE_H
#define STATEMENT_RAISE_H

#include "statement.h"

#include <string>

namespace bloc
{

class Parser;
class Context;
class Expression;

/**
 * The RAISE statement enable or disable trace verbosity.
 * Syntax is:
 * trace {boolean expression}
 */
class RAISEStatement : public Statement
{
  std::string _name;

public:
  virtual ~RAISEStatement() { }

  explicit RAISEStatement(const std::string& name) : Statement(STMT_RAISE), _name(name) { }

  const Statement * doit(Context& ctx) const override;

  void unparse(Context& ctx, FILE * out) const override;

  /**
   * Parse RAISE Statement.
   */
  static RAISEStatement * parse(Parser& p, Context& ctx);
};

}

#endif /* STATEMENT_RAISE_H */

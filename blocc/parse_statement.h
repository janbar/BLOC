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

#ifndef PARSE_STATEMENT_H
#define PARSE_STATEMENT_H

#include "statement.h"

namespace bloc
{

class Parser;
class Context;

class ParseStatement {

public:

  /**
   * Parse input stream for a valid statement.
   * Throws exception ParseError.
   */
  static Statement * statement(Parser& p, Context& ctx);

private:

  Parser& p;
  Context& ctx;

  ~ParseStatement() = default;
  ParseStatement(Parser& p, Context& ctx) : p(p), ctx(ctx) { }

  Statement * parse();
  Statement * beyond_statement(Statement * s);
  Statement * chain_statement(Statement * s);
};

}

#endif /* PARSE_STATEMENT_H */

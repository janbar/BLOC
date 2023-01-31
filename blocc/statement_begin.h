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

#ifndef STATEMENT_BEGIN_H
#define STATEMENT_BEGIN_H

#include "statement.h"
#include "executable.h"

#include <string>

namespace bloc
{

class Parser;
class Context;
class Expression;

/**
 * The BEGIN statement begins a new block of statements.
 * Syntax is: begin [statement ...] end
 */
class BEGINStatement : public Statement
{
private:
  Executable * _exec = nullptr;
  std::list<std::pair<std::string, Executable*> > _catches;
  static Executable * parse_catch(Parser& p, Context& ctx);

  void docatch(const RuntimeError& rt, Context& ctx) const;

public:
  virtual ~BEGINStatement();

  BEGINStatement() : Statement(STMT_BEGIN) { }

  const Statement * doit(Context& ctx) const override;

  void unparse(Context& ctx, FILE * out) const override;

  /**
   * Parse BEGIN Statement.
   */
  static BEGINStatement * parse(Parser& p, Context& ctx);
};

}

#endif /* STATEMENT_BEGIN_H */

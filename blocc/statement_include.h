/*
 *      Copyright (C) 2025 Jean-Luc Barriere
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

#ifndef STATEMENT_INCLUDE_H
#define STATEMENT_INCLUDE_H

#include "statement.h"
#include "executable.h"

#include <string>

namespace bloc
{

class Parser;
class Context;
class Expression;

/**
 * The INCLUDE statement is the directive to embed another source.
 * Syntax is:
 * include {encapsulated string containing the file path}
 */
class INCLUDEStatement : public Statement
{
  Expression * _exp = nullptr;
  Executable * _exec = nullptr;

public:
  virtual ~INCLUDEStatement();

  explicit INCLUDEStatement(Expression * exp) : Statement(STMT_INCLUDE), _exp(exp) { }

  const Statement * doit(Context& ctx) const override
  {
    /* it should run with the given context */
    _exec->run(ctx, _exec->statements());
    return _next;
  }

  void unparse(Context& ctx, FILE * out) const override;

  void loadSource(Parser& p, Context& ctx);

  /**
   * Parse INCLUDE Statement.
   */
  static INCLUDEStatement * parse(Parser& p, Context& ctx);
};

}

#endif /* STATEMENT_INCLUDE_H */

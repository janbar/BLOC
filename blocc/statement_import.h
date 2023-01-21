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

#ifndef STATEMENT_IMPORT_H
#define STATEMENT_IMPORT_H

#include "statement.h"

#include <string>

namespace bloc
{

class Parser;
class Context;
class Expression;

/**
 * The IMPORT statement is the directive to load an external module.
 * Loading is performed during parse.
 * Syntax is:
 * import {module name}
 * import {encapsulated string containing the library file path}
 *
 * Using module name will try to load the library "libbloc_{name}" from cache.
 */
class IMPORTStatement : public Statement
{
  std::string _name;
  Expression * _exp = nullptr;

public:
  virtual ~IMPORTStatement();

  explicit IMPORTStatement(const std::string& name) : Statement(STMT_IMPORT), _name(name) { }
  explicit IMPORTStatement(Expression * exp) : Statement(STMT_IMPORT), _exp(exp) { }

  const Statement * doit(Context& ctx) const override { return _next; }

  void unparse(Context& ctx, FILE * out) const override;

  void loadModule(Context& ctx);

  /**
   * Parse IMPORT Statement.
   */
  static IMPORTStatement * parse(Parser& p, Context& ctx);
};

}

#endif /* STATEMENT_IMPORT_H */

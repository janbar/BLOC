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

#ifndef EXECUTABLE_H
#define EXECUTABLE_H

#include "context.h"
#include "statement.h"

#include <list>

namespace bloc
{

class Statement;

class Executable {

public:
  virtual ~Executable();

  Executable(Context& ctx, const std::list<const Statement*>& statements)
  : _context(ctx)
  , _statements(statements)
  { }

  static int run(Context& ctx, const std::list<const Statement*>& statements);

  int run() { return run(_context, _statements); }

  std::list<const Statement*>& statements() { return _statements; }

  Context& context() { return _context; }

  void unparse(FILE * out);

private:
  Context& _context;
  std::list<const Statement*> _statements;
};

}

#endif /* EXECUTABLE_H */


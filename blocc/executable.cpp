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

#include "executable.h"
#include "parser.h"

namespace bloc
{

Executable::~Executable()
{
  for (auto * s : _statements)
    delete s;
  _statements.clear();
}

int Executable::run(Context& ctx, const std::list<const Statement*>& statements)
{
  try
  {
    for (auto s : statements)
    {
      while (s != nullptr)
        s = s->execute(ctx);
      if (ctx.stopCondition())
        break;
    }
  }
  catch (...)
  {
    ctx.onRuntimeError();
    throw;
  }
  return 0;
}

void Executable::unparse(FILE * out)
{
  for (auto s : _statements)
  {
    bool eos = (s->keyword() == Statement::STMT_END ||
                s->keyword() == Statement::STMT_ENDIF ||
                s->keyword() == Statement::STMT_ENDLOOP ||
                s->keyword() == Statement::STMT_ELSIF ||
                s->keyword() == Statement::STMT_ELSE);
    if (!eos)
      for (size_t i = 0; i < _context.execLevel(); ++i) fputs("    ", out);
    else
      for (size_t i = 1; i < _context.execLevel(); ++i) fputs("    ", out);
    s->unparse(_context, out);
    if (!eos)
    {
      fputc(Parser::SEPARATOR, out);
      fputc(Parser::NEWLINE, out);
    }
  }
}

}

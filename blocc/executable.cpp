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
    /* return condition must be cleared before rerun */
    if (ctx.returnCondition())
      throw RuntimeError(EXC_RT_REQUIRE_PURGE);
    /* the statement loop */
    for (auto s : statements)
    {
      /* the statement chain */
      while (s != nullptr)
        s = s->execute(ctx);
      /* a stop condition breaks the loop */
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
    switch (s->keyword())
    {
    case Statement::STMT_END:
    case Statement::STMT_ENDIF:
    case Statement::STMT_ENDLOOP:
    case Statement::STMT_ELSIF:
    case Statement::STMT_ELSE:
    case Statement::STMT_EXCEPTION:
    case Statement::STMT_WHEN:
      for (size_t i = 1; i < _context.execLevel(); ++i) fputs(Parser::Indent, out);
      s->unparse(_context, out);
      break;
    /* declaration of functor is on top level */
    case Statement::STMT_FUNCTION:
      s->unparse(_context, out);
      fputc(Parser::Separator, out);
      fputc(Parser::NewLine, out);
      break;
    default:
      for (size_t i = 0; i < _context.execLevel(); ++i) fputs(Parser::Indent, out);
      s->unparse(_context, out);
      fputc(Parser::Separator, out);
      fputc(Parser::NewLine, out);
    }
  }
}

}

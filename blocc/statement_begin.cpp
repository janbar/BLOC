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

#include "statement_begin.h"
#include "exception_parse.h"
#include "parse_expression.h"
#include "parse_statement.h"
#include "parser.h"
#include "context.h"
#include "debug.h"

#include <string>
#include <list>

namespace bloc
{

BEGINStatement::~BEGINStatement()
{
  if (_exec)
    delete _exec;
}

const Statement * BEGINStatement::doit(Context& ctx) const
{
  ctx.execBegin(this);
  try
  {
    _exec->run();
  }
  catch (...)
  {
    ctx.execEnd();
    throw;
  }
  ctx.execEnd();
  return _next;
}

void BEGINStatement::unparse(Context& ctx, FILE * out) const
{
  fputs(Statement::KEYWORDS[keyword()], out);
  fputc(Parser::NEWLINE, out);
  ctx.execBegin(this);
  _exec->unparse(out);
  ctx.execEnd();
}

BEGINStatement * BEGINStatement::parse(Parser& p, Context& ctx)
{
  BEGINStatement * s = new BEGINStatement();
  ctx.execBegin(s);
  std::list<const Statement*> statements;
  try
  {
    bool end = false;
    TokenPtr t;
    while (!end && (t = p.pop()))
    {
      if (t->code == Parser::SEPARATOR)
        continue;
      /* parse the statement */
      p.push(t);
      Statement * ss = ParseStatement::statement(p, ctx);
      statements.push_back(ss);
      /* check for ending */
      if (ss->keyword() == STMT_END)
        end = true;
    }
    if (statements.back()->keyword() != Statement::STMT_END)
      throw ParseError(EXC_PARSE_MESSAGE_S, "Endless BEGIN statement.");
  }
  catch (ParseError& pe)
  {
    DBG(DBG_DEBUG, "exception %p at %s line %d\n", &pe, __PRETTY_FUNCTION__, __LINE__);
    ctx.execEnd();
    for (auto ss : statements)
      delete ss;
    delete s;
    throw;
  }
  ctx.execEnd();
  s->_exec = new Executable(ctx, statements);
  return s;
}

}

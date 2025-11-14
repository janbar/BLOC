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

#include "statement.h"
#include "parser.h"

#include <cstring>

namespace bloc
{

/**
 * These are all of the statement KEYWORDS we can parse.
 */
const char * Statement::KEYWORDS[] = {
  "nop",      "trace",    "let",      "import",   "function",
  "if",       "then",     "else",     "elsif",    "for",
  "while",    "loop",     "in",       "to",       "return",
  "begin",    "break",    "continue", "end",      "end if",
  "end loop", "print",    "put",      "do",       "exception",
  "when",     "raise",    "asc",      "desc",     "is",
  "forall",   "include",  "step",
};

Statement::~Statement()
{
  delete_next();
}

const Statement *Statement::execute(Context& ctx) const
{
  bool trace = ctx.trace();
  _level = ctx.execLevel();
  if (trace) trace_pre(ctx);
  const Statement * next = doit(ctx);
  if (trace) trace_post(ctx);
  ctx.onStatementEnd(this);
  return next;
}

void Statement::delete_next()
{
  /* delete next chain */
  if (_next)
  {
    _next->delete_next();
    delete _next;
    _next = nullptr;
  }
}

void Statement::unparse_next(Context& ctx, FILE * out) const
{
  if (_next)
  {
    fputc(' ', out);
    fputc(Parser::Chain, out);
    fputc(' ', out);
    _next->unparse(ctx, out);
  }
}

void Statement::trace_pre(Context& ctx) const
{
  if (ctx.ctxerr())
  {
    fprintf(ctx.ctxerr(), "%012.6f: %s\n", ctx.timestamp(), KEYWORDS[_keyword]);
    fflush(ctx.ctxerr());
  }
}

void Statement::trace_post(Context& ctx) const
{
  if (ctx.ctxerr())
  {
    size_t count = ctx.allocationCount();
    fprintf(ctx.ctxerr(), "%012.6f: %s : alloc=%u bcr=%d%d%d\n",
            ctx.timestamp(), KEYWORDS[_keyword], (unsigned)count,
            ctx.breakCondition(), ctx.continueCondition(), ctx.returnCondition());
    fflush(ctx.ctxerr());
  }
}

int Statement::findKeyword(const std::string& s)
{
  for (size_t i = 0; i < (sizeof (KEYWORDS) / sizeof (char*)); ++i)
  {
    if (strcmp(KEYWORDS[i], s.c_str()) == 0)
      return i;
  }
  return unknown;
}

std::set<std::string> Statement::keywordSet()
{
  std::set<std::string> lst;
  for (int i = 0; i < (sizeof (KEYWORDS) / sizeof (char*)); ++i)
    if (KEYWORDS[i][0] != '\0')
      lst.insert(KEYWORDS[i]);
  return lst;
}

}

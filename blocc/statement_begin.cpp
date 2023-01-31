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
#include "statement_end.h"
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
  for (auto& c : _catches)
    delete c.second;
  _catches.clear();
}

void BEGINStatement::docatch(const RuntimeError& rt, Context& ctx) const
{
  if (!_catches.empty())
  {
    for (auto& c : _catches)
    {
      EXC_RT ec = RuntimeError::findThrowable(c.first);
      if ((c.first == "OTHERS" && (rt.no == EXC_RT_USER_S || RuntimeError::throwable(rt.no) > 0)) ||
          (ec == rt.no && (
              /* catch known exception */
              (ec != EXC_RT_USER_S) ||
              /* catch user defined exception */
              (ec == EXC_RT_USER_S && c.first == rt.what())
              )))
      {
        /* catch the user defined exception */
        try
        {
          /* save catched error in the context */
          ctx.error(rt);
          c.second->run();
          /* clear error in the context */
          ctx.error(RuntimeError());
        }
        catch (RuntimeError& rte)
        {
          /* note: the saved error will be kept for debug */
          /* close block before throw */
          ctx.execEnd();
          throw;
        }
        return;
      }
    }
  }
  /* no catch */
  /* close block before throw */
  ctx.execEnd();
  throw rt;
}

const Statement * BEGINStatement::doit(Context& ctx) const
{
  ctx.execBegin(this);
  try
  {
    _exec->run();
  }
  catch (RuntimeError& rt)
  {
    docatch(rt, ctx);
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
  if (!_catches.empty())
  {
    for (size_t i = 0; i < ctx.execLevel(); ++i) fputs(Parser::INDENT, out);
    fputs(Statement::KEYWORDS[STMT_EXCEPTION], out);
    fputc(Parser::NEWLINE, out);
    for (auto& c : _catches)
    {
      for (size_t i = 0; i < ctx.execLevel(); ++i) fputs(Parser::INDENT, out);
      fputs(Statement::KEYWORDS[STMT_WHEN], out);
      fputc(' ', out);
      fputs(c.first.c_str(), out);
      fputc(' ', out);
      fputs(Statement::KEYWORDS[STMT_THEN], out);
      fputc(Parser::NEWLINE, out);
      ctx.execBegin(this);
      c.second->unparse(out);
      ctx.execEnd();
    }
  }
}

Executable * BEGINStatement::parse_catch(Parser& p, Context& ctx)
{
  std::list<const Statement*> statements;
  try
  {
    TokenPtr t;
    while ((t = p.pop()))
    {
      if (t->code == Parser::SEPARATOR)
        continue;
      /* check for ending */
      if (t->code == TOKEN_KEYWORD &&
              (t->text == KEYWORDS[STMT_END] || t->text == KEYWORDS[STMT_WHEN]))
        break;
      /* parse the statement */
      p.push(t);
      const Statement * ss = ParseStatement::statement(p, ctx);
      statements.push_back(ss);
    }
    /* requires at least one statement, even NOP */
    if (statements.empty())
      throw ParseError(EXC_PARSE_UNEXPECTED_LEX_S, t->text.c_str());
    p.push(t);
  }
  catch (ParseError& pe)
  {
    DBG(DBG_DEBUG, "exception %p at %s line %d\n", &pe, __PRETTY_FUNCTION__, __LINE__);
    for (auto ss : statements)
      delete ss;
    throw;
  }
  return new Executable(ctx, statements);
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
    for (;;)
    {
      t = p.pop();
      if (t->code == Parser::SEPARATOR)
        continue;
      /* check for ending */
      if (t->text == KEYWORDS[STMT_END] || t->text == KEYWORDS[STMT_EXCEPTION])
        break;
      /* parse the statement */
      p.push(t);
      const Statement * ss = ParseStatement::statement(p, ctx);
      statements.push_back(ss);
    }
    if (t->text == KEYWORDS[STMT_EXCEPTION])
    {
      t = p.pop();
      /* parse exception section */
      for (;;)
      {
        /* when */
        if (t->code != TOKEN_KEYWORD || t->text != KEYWORDS[STMT_WHEN])
          throw ParseError(EXC_PARSE_OTHER_S, "Missing WHEN keyword in EXCEPTION clause.");
        /* exception name */
        t = p.pop();
        if (t->code != TOKEN_KEYWORD)
          throw ParseError(EXC_PARSE_UNEXPECTED_LEX_S, t->text.c_str());
        if (Parser::reservedKeyword(t->text))
          throw ParseError(EXC_PARSE_RESERVED_WORD_S, t->text.c_str());
        std::string name(t->text);
        std::transform(name.begin(), name.end(), name.begin(), ::toupper);
        /* then */
        t = p.pop();
        if (t->code != TOKEN_KEYWORD || t->text != KEYWORDS[STMT_THEN])
          throw ParseError(EXC_PARSE_OTHER_S, "Missing THEN keyword in EXCEPTION clause.");
        /* clause */
        Executable * exec = parse_catch(p, ctx);
        s->_catches.push_back(std::make_pair(name, exec));
        t = p.pop();
        if (t->text != KEYWORDS[STMT_END])
          continue;
        /* parse statement END */
        exec->statements().push_back(ENDStatement::parse(p, ctx, STMT_END));
        break;
      }
    }
    else
    {
      /* parse statement END */
      statements.push_back(ENDStatement::parse(p, ctx, STMT_END));
    }
    t = p.pop();
    if (t->code != Parser::SEPARATOR)
      throw ParseError(EXC_PARSE_STATEMENT_END_S, t->text.c_str());
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

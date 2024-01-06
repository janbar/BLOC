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

#include "statement_while.h"
#include "statement_end.h"
#include "exception_parse.h"
#include "parse_expression.h"
#include "parse_statement.h"
#include "parser.h"
#include "context.h"
#include "debug.h"

#include <string>
#include <cassert>

namespace bloc
{

WHILEStatement::~WHILEStatement()
{
  if (exp)
    delete exp;
  if (_exec)
    delete _exec;
}

const Statement * WHILEStatement::doit(Context& ctx) const
{
  if (this != ctx.topControl())
  {
    ctx.stackControl(this);
  }
  Value& val = exp->value(ctx);
  if (!val.isNull() && *val.boolean())
  {
    /* it should run with the given context */
    _exec->run(ctx, _exec->statements());
    if (!ctx.stopCondition())
      return this;
    if (ctx.continueCondition())
    {
      ctx.continueCondition(false);
      return this;
    }
    ctx.breakCondition(false);
  }
  ctx.unstackControl();
  return _next;
}

void WHILEStatement::unparse(Context& ctx, FILE * out) const
{
  fputs(Statement::KEYWORDS[keyword()], out);
  fputc(' ', out);
  fputs(exp->unparse(ctx).c_str(), out);
  fputc(' ', out);
  fputs(KEYWORDS[STMT_LOOP], out);
  fputc(Parser::NEWLINE, out);
  ctx.execBegin(this);
  _exec->unparse(out);
  ctx.execEnd();
}

Executable * WHILEStatement::parse_clause(Parser& p, Context& ctx, Statement * ihw)
{
  ctx.execBegin(ihw);
  std::list<const Statement*> statements;
  try
  {
    bool end = false;
    TokenPtr t;
    while (!end && (t = p.pop()))
    {
      if (t->code == Parser::SEPARATOR)
        continue;
      /* check for ending */
      if (t->code == TOKEN_KEYWORD && t->text == KEYWORDS[STMT_END])
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
    ctx.execEnd();
    for (auto ss : statements)
      delete ss;
    throw;
  }
  ctx.execEnd();
  return new Executable(ctx, statements);
}

WHILEStatement * WHILEStatement::parse(Parser& p, Context& ctx)
{
  WHILEStatement * s = new WHILEStatement();
  try
  {
    s->exp = ParseExpression::expression(p, ctx);
    if (s->exp->type(ctx) != Type::BOOLEAN)
      throw ParseError(EXC_PARSE_OTHER_S, "Boolean expression required for WHILE.");
    TokenPtr t = p.pop();
    if (t->code == ')')
      throw ParseError(EXC_PARSE_MM_PARENTHESIS);
    if (t->code != TOKEN_KEYWORD || t->text != KEYWORDS[STMT_LOOP])
      throw ParseError(EXC_PARSE_OTHER_S, "Missing LOOP keyword in WHILE statement.");
    s->_exec = parse_clause(p, ctx, s);
    t = p.pop();
    if (t->text != KEYWORDS[STMT_END])
      throw ParseError(EXC_PARSE_OTHER_S, "Endless WHILE LOOP statement.");
    /* parse statement END */
    s->_exec->statements().push_back(ENDStatement::parse(p, ctx, STMT_ENDLOOP));
    t = p.pop();
    if (t->code != Parser::SEPARATOR)
      throw ParseError(EXC_PARSE_STATEMENT_END_S, t->text.c_str());
    return s;
  }
  catch (ParseError& pe)
  {
    DBG(DBG_DEBUG, "exception %p at %s line %d\n", &pe, __PRETTY_FUNCTION__, __LINE__);
    delete s;
    throw;
  }
  return nullptr;
}

}

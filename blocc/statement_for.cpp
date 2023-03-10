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

#include "statement_for.h"
#include "statement_end.h"
#include "exception_parse.h"
#include "parse_expression.h"
#include "parse_statement.h"
#include "expression_integer.h"
#include "expression_variable.h"
#include "parser.h"
#include "context.h"
#include "debug.h"

#include <string>
#include <cassert>

namespace bloc
{

FORStatement::~FORStatement()
{
  if (_expBeg)
    delete _expBeg;
  if (_expEnd)
    delete _expEnd;
  if (_exec)
    delete _exec;
  if (_iterator)
    delete _iterator;
}

const Statement * FORStatement::doit(Context& ctx) const
{
  if (this != ctx.topControl())
  {
    int64_t n = _expBeg->integer(ctx);
    _data.limit = _expEnd->integer(ctx);
    _data.step = (_data.limit < n ? -1 : 1);
    /* initialize var as type safe */
    _data.var = &_iterator->store(ctx, IntegerExpression(n));
    _data.var->safety(true);
    ctx.stackControl(this);
  }
  else
  {
    /* var is type safe, so it can be read without care */
    int64_t& nref = _data.var->refInteger();
    if (nref >= _data.limit)
    {
      /* now var can be unsafe */
      _data.var->safety(false);
      ctx.unstackControl();
      return _next;
    }
    nref += _data.step;
  }
  try
  {
    _exec->run();
  }
  catch (...)
  {
    _data.var->safety(false);
    throw;
  }
  if (!ctx.stopCondition())
    return this;
  if (ctx.continueCondition())
  {
    ctx.continueCondition(false);
    return this;
  }
  ctx.breakCondition(false);
  ctx.unstackControl();
  return _next;
}

void FORStatement::unparse(Context& ctx, FILE * out) const
{
  fputs(Statement::KEYWORDS[keyword()], out);
  fputc(' ', out);
  fputs(_iterator->symbolName().c_str(), out);
  fputc(' ', out);
  fputs(KEYWORDS[STMT_IN], out);
  fputc(' ', out);
  fputs(_expBeg->unparse(ctx).c_str(), out);
  fputc(' ', out);
  fputs(KEYWORDS[STMT_TO], out);
  fputc(' ', out);
  fputs(_expEnd->unparse(ctx).c_str(), out);
  fputc(' ', out);
  fputs(KEYWORDS[STMT_LOOP], out);
  fputc(Parser::NEWLINE, out);
  ctx.execBegin(this);
  _exec->unparse(out);
  ctx.execEnd();
}

Executable * FORStatement::parse_clause(Parser& p, Context& ctx, FORStatement * rof)
{
  ctx.execBegin(rof);
  std::list<const Statement*> statements;
  // iterator must be protected against type change
  Symbol& vt = *ctx.findSymbol(rof->_iterator->symbolName());
  vt.safety(true);
  // parsing expressions will check types first from existing variables, then
  // from registered symbols, so reset the variable if any, and make it safe
  StaticExpression * vv = rof->_iterator->load(ctx);
  if (vv)
  {
    rof->_iterator->store(ctx, IntegerExpression(0));
    vv->safety(true);
  }
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
      Statement * ss = ParseStatement::statement(p, ctx);
      statements.push_back(ss);
    }
    /* requires at least one statement, even NOP */
    if (statements.empty())
      throw ParseError(EXC_PARSE_UNEXPECTED_LEX_S, t->text.c_str());
    p.push(t);
  }
  catch (ParseError& pe)
  {
    // cleanup
    if (vv)
      vv->safety(false);
    vt.safety(false);
    ctx.execEnd();
    for (auto ss : statements)
      delete ss;
    throw;
  }
  vt.safety(false);
  ctx.execEnd();
  return new Executable(ctx, statements);
}

FORStatement * FORStatement::parse(Parser& p, Context& ctx)
{
  FORStatement * s = new FORStatement();
  try
  {
    TokenPtr t = p.pop();
    if (t->code != TOKEN_KEYWORD)
      throw ParseError(EXC_PARSE_OTHER_S, "Symbol of variable required for FOR.");
    std::string vname = t->text;
    std::transform(vname.begin(), vname.end(), vname.begin(), ::toupper);
    s->_iterator = new VariableExpression(ctx.registerSymbol(vname, Type::INTEGER));
    t = p.pop();
    if (t->code != TOKEN_KEYWORD || t->text != KEYWORDS[STMT_IN])
      throw ParseError(EXC_PARSE_OTHER_S, "Keyword IN required for FOR.");
    s->_expBeg = ParseExpression::expression(p, ctx);
    if (s->_expBeg->type(ctx) != Type::INTEGER)
      throw ParseError(EXC_PARSE_OTHER_S, "Numeric or Integer expression required for FOR.");
    t = p.pop();
    if (t->code == ')')
      throw ParseError(EXC_PARSE_MM_PARENTHESIS);
    if (t->code != TOKEN_KEYWORD || t->text != KEYWORDS[STMT_TO])
      throw ParseError(EXC_PARSE_OTHER_S, "Keyword TO required for FOR.");
    s->_expEnd = ParseExpression::expression(p, ctx);
    if (s->_expEnd->type(ctx) != Type::INTEGER)
      throw ParseError(EXC_PARSE_OTHER_S, "Numeric or Integer expression required for FOR.");
    t = p.pop();
    if (t->code == ')')
      throw ParseError(EXC_PARSE_MM_PARENTHESIS);
    if (t->code != TOKEN_KEYWORD || t->text != KEYWORDS[STMT_LOOP])
      throw ParseError(EXC_PARSE_OTHER_S, "Missing LOOP keyword in FOR statement.");
    s->_exec = parse_clause(p, ctx, s);
    t = p.pop();
    if (t->text != KEYWORDS[STMT_END])
      throw ParseError(EXC_PARSE_OTHER_S, "Endless FOR LOOP statement.");
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

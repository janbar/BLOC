/*
 *      Copyright (C) 2023 Jean-Luc Barriere
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

#include "statement_forall.h"
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

FORALLStatement::~FORALLStatement()
{
  if (_exec)
    delete _exec;
  if (_exp)
    delete _exp;
  if (_var)
    delete _var;
}

const Statement * FORALLStatement::doit(Context& ctx) const
{
  if (this != ctx.topControl())
  {
    if (_expSymbol)
    {
      /* reference to the store */
      _data.table = &_exp->collection(ctx);
      if (_data.table->size() == 0)
        return _next;
      _expSymbol->safety(true);
    }
    else
    {
      /* it must be free later */
      _data.table = new Collection(std::move(_exp->collection(ctx)));
      if (_data.table->size() == 0)
      {
        delete _data.table;
        return _next;
      }
    }
    /* set fetch order */
    if (_order == DESC)
    {
      /* pointer to the store */
      _data.iterator = static_cast<IteratorExpression*>(&_var->store(ctx, IteratorExpression(*_data.table, _data.table->size()-1)));
      _data.step = -1;
    }
    else
    {
      /* pointer to the store */
      _data.iterator = static_cast<IteratorExpression*>(&_var->store(ctx, IteratorExpression(*_data.table, 0)));
      _data.step = 1;
    }
    /* iterator is type safe in the loop body */
    _data.iterator->safety(true);
    ctx.stackControl(this);
  }
  else
  {
    if ((_data.step > 0 && _data.iterator->index() < _data.table->size()-1) ||
        (_data.step < 0 && _data.iterator->index() > 0))
    {
      _data.iterator->index() += _data.step;
    }
    else
    {
      _var->clear(ctx);
      if (_expSymbol)
        _expSymbol->safety(false);
      else
        /* delete temporary data */
        delete _data.table;
      ctx.unstackControl();
      return _next;
    }
  }
  try
  {
    /* it should run with the given context */
    _exec->run(ctx, _exec->statements());
  }
  catch (...)
  {
    _var->clear(ctx);
    if (_expSymbol)
      _expSymbol->safety(false);
    else
      delete _data.table;
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

void FORALLStatement::unparse(Context& ctx, FILE * out) const
{
  fputs(Statement::KEYWORDS[keyword()], out);
  fputc(' ', out);
  fputs(_var->symbolName().c_str(), out);
  fputc(' ', out);
  fputs(KEYWORDS[STMT_IN], out);
  fputc(' ', out);
  fputs(_exp->unparse(ctx).c_str(), out);
  fputc(' ', out);
  switch(_order)
  {
  case AUTO:
    break;
  case ASC:
    fputs(KEYWORDS[STMT_ASC], out);
    fputc(' ', out);
    break;
  case DESC:
    fputs(KEYWORDS[STMT_DESC], out);
    fputc(' ', out);
    break;
  }
  fputs(KEYWORDS[STMT_LOOP], out);
  fputc(Parser::NEWLINE, out);
  ctx.execBegin(this);
  _exec->unparse(out);
  ctx.execEnd();
}

Executable * FORALLStatement::parse_clause(Parser& p, Context& ctx, FORALLStatement * rof)
{
  ctx.execBegin(rof);
  std::list<const Statement*> statements;
  // iterator must be protected against type change
  Symbol& vt = *ctx.getSymbol(rof->_var->symbolId());
  vt.safety(true);
  // parsing expressions will check types first from existing variables, then
  // from registered symbols, so reset the variable if any
  ctx.clearVariable(vt);
  // fetched expression must be protected against type change
  if (rof->_expSymbol)
    rof->_expSymbol->safety(true);
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
    if (rof->_expSymbol)
      rof->_expSymbol->safety(false);
    vt.safety(false);
    ctx.execEnd();
    for (auto ss : statements)
      delete ss;
    throw;
  }
  if (rof->_expSymbol)
    rof->_expSymbol->safety(false);
  vt.safety(false);
  ctx.execEnd();
  return new Executable(ctx, statements);
}

FORALLStatement * FORALLStatement::parse(Parser& p, Context& ctx)
{
  FORALLStatement * s = new FORALLStatement();
  try
  {
    TokenPtr t = p.pop();
    if (t->code != TOKEN_KEYWORD)
      throw ParseError(EXC_PARSE_OTHER_S, "Symbol of variable required for FOR.");
    std::string vname = t->text;
    std::transform(vname.begin(), vname.end(), vname.begin(), ::toupper);
    t = p.pop();
    if (t->code != TOKEN_KEYWORD || t->text != KEYWORDS[STMT_IN])
      throw ParseError(EXC_PARSE_OTHER_S, "Keyword IN required for FORALL.");
    s->_exp = ParseExpression::expression(p, ctx);
    t = p.pop();
    if (t->code == ')')
      throw ParseError(EXC_PARSE_MM_PARENTHESIS);
    /* retrieve the symbol name for a variable expression, that is required to
     * make it safety when processing the clause of the statement */
    if (s->_exp->isStored())
      s->_expSymbol = ctx.getSymbol(dynamic_cast<VariableExpression*>(s->_exp)->symbolId());
    /* check the type if defined */
    const Type& exp_type = s->_exp->type(ctx);
    if (exp_type == Type::NO_TYPE)
    {
      s->_var = new VariableExpression(ctx.registerSymbol(vname, exp_type));
    }
    else
    {
      if (exp_type.level() == 0)
        throw ParseError(EXC_PARSE_OTHER_S, "Table expression required for FORALL.");
      /* define the variable to store fetched element */
      if (exp_type.major() == Type::ROWTYPE)
        /* register symbol of tuple */
        s->_var = new VariableExpression(ctx.registerSymbol(vname, s->_exp->tuple_decl(ctx), exp_type.level()-1));
      else
        /* register symbol of intrinsic type */
        s->_var = new VariableExpression(ctx.registerSymbol(vname, exp_type.levelDown()));
    }
    if (t->code == TOKEN_KEYWORD)
    {
      if (t->text == KEYWORDS[STMT_ASC])
      {
        s->_order = ASC;
        t = p.pop();
      }
      else if (t->text == KEYWORDS[STMT_DESC])
      {
        s->_order = DESC;
        t = p.pop();
      }
    }
    if (t->code != TOKEN_KEYWORD || t->text != KEYWORDS[STMT_LOOP])
      throw ParseError(EXC_PARSE_OTHER_S, "Missing LOOP keyword in FORALL statement.");
    s->_exec = parse_clause(p, ctx, s);
    t = p.pop();
    if (t->text != KEYWORDS[STMT_END])
      throw ParseError(EXC_PARSE_OTHER_S, "Endless FORALL LOOP statement.");
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
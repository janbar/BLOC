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
#include "expression_variable.h"
#include "collection.h"
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
    Value& val = _exp->value(ctx);
    if (val.isNull() || val.collection()->size() == 0)
      return _next;
    if (_expSymbol)
    {
      _data.tmp = &val;
      /* value is type safe in the loop body */
      _data.safety_ex_bak = _expSymbol->safety();
      _expSymbol->safety(true);
      _data.tmp->to_safety(true);
    }
    else
    {
      /* store rvalue */
      _data.tmp = new Value(std::move(val));
    }
    /* set fetch order */
    if (_order == DESC)
    {
      _data.step = -1;
      _data.index = _data.tmp->collection()->size()-1;
    }
    else
    {
      _data.step = 1;
      _data.index = 0;
    }
    Value& it = ctx.loadVariable(*_var->symbol());
    /* backup the state of the variable used as iterator */
    _data.value_it_bak.swap(Value(it.type()).to_lvalue(true).to_safety(it.safety()));
    /* store a safe pointer to the value as lvalue */
    it.swap(Value(&(_data.tmp->collection()->at(_data.index).to_lvalue(true)))
      .to_lvalue(true).to_safety(true));
    _var->symbol()->safety(true);
    ctx.stackControl(this);
  }
  else
  {
    _data.index += _data.step;
    if (_data.index < 0 || _data.index >= _data.tmp->collection()->size())
    {
      /* restore the state of the variable used as iterator */
      _var->symbol()->safety(_data.value_it_bak.safety());
      ctx.loadVariable(*_var->symbol()).swap(std::move(_data.value_it_bak));
      if (_expSymbol)
      {
        /* restore the safety state of the variable value */
        _data.tmp->to_safety(_data.safety_ex_bak);
        _expSymbol->safety(_data.safety_ex_bak);
      }
      else
      {
        /* delete allocated value */
        delete _data.tmp;
      }
      ctx.unstackControl();
      return _next;
    }
    else
    {
      /* store a safe pointer to the value as lvalue */
      Value& it = ctx.loadVariable(*_var->symbol());
      it.swap(Value(&(_data.tmp->collection()->at(_data.index).to_lvalue(true)))
        .to_lvalue(true).to_safety(true));
    }
  }
  try
  {
    /* it should run with the given context */
    _exec->run(ctx, _exec->statements());
  }
  catch (...)
  {
    /* restore the state of the variable used as iterator */
    _var->symbol()->safety(_data.value_it_bak.safety());
    ctx.loadVariable(*_var->symbol()).swap(std::move(_data.value_it_bak));
    if (_expSymbol)
    {
      /* restore the safety state of the variable value */
      _data.tmp->to_safety(_data.safety_ex_bak);
      _expSymbol->safety(_data.safety_ex_bak);
    }
    else
    {
      /* delete allocated value */
      delete _data.tmp;
    }
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
  fputs(_var->symbol()->name().c_str(), out);
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
  Symbol& vt = *ctx.getSymbol(rof->_var->symbol()->id());
  bool safety_vt_bak = vt.safety();
  vt.safety(true);
  // fetched expression must be protected against type change
  bool safety_ex_bak = false;
  if (rof->_expSymbol)
  {
    safety_ex_bak = rof->_expSymbol->safety();
    rof->_expSymbol->safety(true);
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
    if (rof->_expSymbol)
      rof->_expSymbol->safety(safety_ex_bak);
    vt.safety(safety_vt_bak);
    ctx.execEnd();
    for (auto ss : statements)
      delete ss;
    throw;
  }
  if (rof->_expSymbol)
    rof->_expSymbol->safety(safety_ex_bak);
  vt.safety(safety_vt_bak);
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
    s->_expSymbol = s->_exp->symbol();
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

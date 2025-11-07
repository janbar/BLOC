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
  if (_expStp)
    delete _expStp;
  if (_exec)
    delete _exec;
  if (_var)
    delete _var;
}

const Statement * FORStatement::doit(Context& ctx) const
{
  if (this != ctx.topControl())
  {
    Integer s = 1;
    Value& vb = _expBeg->value(ctx);
    if (vb.isNull())
      return _next;
    Value& ve = _expEnd->value(ctx);
    if (ve.isNull())
      return _next;
    if (_expStp)
    {
      Value& vs = _expStp->value(ctx);
      if (vs.isNull())
        return _next;
      s = *vs.integer();
      if (s < 1)
        throw RuntimeError(EXC_RT_OUT_OF_RANGE);
    }
    Integer b = *vb.integer();
    Integer e = *ve.integer();
    if (e > b)
    {
      if (_order == DESC)
        return _next;
      _data.min = b;
      _data.max = e;
      _data.step = s;
    }
    else
    {
      if (_order == ASC && e != b)
        return _next;
      _data.min = e;
      _data.max = b;
      _data.step = -s;
    }
    _data.iterator = &(_var->store(ctx, std::move(vb)));
    /* value is type safe in the loop body */
    _data.safety_bak = _var->symbol()->safety();
    _var->symbol()->safety(true);
    ctx.stackControl(this);
  }
  else
  {
    /* var is type safe, so it can be read/write without care */
    Integer nxt = *_data.iterator->integer() + _data.step;
    if ((_data.step > 0 && nxt > _data.max) ||
        (_data.step < 0 && nxt < _data.min))
    {
      /* restore the safety state of the variable */
      _var->symbol()->safety(_data.safety_bak);
      ctx.unstackControl();
      return _next;
    }
    *_data.iterator->integer() = nxt;
  }
  try
  {
    /* it should run with the given context */
    _exec->run(ctx, _exec->statements());
  }
  catch (...)
  {
    /* restore the safety state of the variable */
    _var->symbol()->safety(_data.safety_bak);
    throw;
  }
  if (!ctx.stopCondition())
    return this;
  if (ctx.continueCondition())
  {
    ctx.continueCondition(false);
    return this;
  }
  /* restore the safety state of the variable */
  _var->symbol()->safety(_data.safety_bak);
  ctx.breakCondition(false);
  ctx.unstackControl();
  return _next;
}

void FORStatement::unparse(Context& ctx, FILE * out) const
{
  fputs(Statement::KEYWORDS[keyword()], out);
  fputc(' ', out);
  fputs(_var->symbol()->name().c_str(), out);
  fputc(' ', out);
  fputs(KEYWORDS[STMT_IN], out);
  fputc(' ', out);
  fputs(_expBeg->unparse(ctx).c_str(), out);
  fputc(' ', out);
  fputs(KEYWORDS[STMT_TO], out);
  fputc(' ', out);
  fputs(_expEnd->unparse(ctx).c_str(), out);
  fputc(' ', out);
  if (_expStp)
  {
    fputs(KEYWORDS[STMT_STEP], out);
    fputc(' ', out);
    fputs(_expStp->unparse(ctx).c_str(), out);
    fputc(' ', out);
  }
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
  fputc(Parser::NewLine, out);
  ctx.execBegin(this);
  _exec->unparse(out);
  ctx.execEnd();
}

Executable * FORStatement::parse_clause(Parser& p, Context& ctx, FORStatement * rof)
{
  ctx.execBegin(rof);
  std::list<const Statement*> statements;
  /* get symbol non-const */
  Symbol& vt = *ctx.getSymbol(rof->_var->symbol()->id());
  /* iterator must be protected against type change */
  bool safety_bak = vt.safety();
  vt.safety(true);
  try
  {
    bool end = false;
    TokenPtr t;
    while (!end && (t = p.pop()))
    {
      if (t->code == Parser::Separator)
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
      throw ParseError(EXC_PARSE_UNEXPECTED_LEX_S, t->text.c_str(), t);
    p.push(t);
  }
  catch (ParseError& pe)
  {
    // cleanup
    vt.safety(safety_bak);
    ctx.execEnd();
    for (auto ss : statements)
      delete ss;
    throw;
  }
  vt.safety(safety_bak);
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
      throw ParseError(EXC_PARSE_OTHER_S, "Symbol of variable required for FOR.", t);
    /* check if keyword is reserved */
    if (p.reservedKeyword(t->text))
      throw ParseError(EXC_PARSE_RESERVED_WORD_S, t->text.c_str(), t);
    std::string vname = t->text;
    std::transform(vname.begin(), vname.end(), vname.begin(), ::toupper);
    s->_var = new VariableExpression(ctx.registerSymbol(vname, Type::INTEGER));
    t = p.pop();
    if (t->code != TOKEN_KEYWORD || t->text != KEYWORDS[STMT_IN])
      throw ParseError(EXC_PARSE_OTHER_S, "Keyword IN required for FOR.", t);
    s->_expBeg = ParseExpression::expression(p, ctx);
    if (!ParseExpression::typeChecking(s->_expBeg, Type::NUMERIC, p, ctx))
      throw ParseError(EXC_PARSE_OTHER_S, "Numeric or Integer expression required for FOR.", t);
    t = p.pop();
    if (t->code == ')')
      throw ParseError(EXC_PARSE_MM_PARENTHESIS, t);
    if (t->code != TOKEN_KEYWORD || t->text != KEYWORDS[STMT_TO])
      throw ParseError(EXC_PARSE_OTHER_S, "Keyword TO required for FOR.", t);
    s->_expEnd = ParseExpression::expression(p, ctx);
    if (!ParseExpression::typeChecking(s->_expEnd, Type::NUMERIC, p, ctx))
      throw ParseError(EXC_PARSE_OTHER_S, "Numeric or Integer expression required for FOR.", t);
    t = p.pop();
    if (t->code == ')')
      throw ParseError(EXC_PARSE_MM_PARENTHESIS, t);
    if (t->code == TOKEN_KEYWORD && t->text  == KEYWORDS[STMT_STEP])
    {
      s->_expStp = ParseExpression::expression(p, ctx);
      if (!ParseExpression::typeChecking(s->_expStp, Type::NUMERIC, p, ctx))
        throw ParseError(EXC_PARSE_OTHER_S, "Numeric or Integer expression required for STEP.", t);
      t = p.pop();
    }
    if (t->code == ')')
      throw ParseError(EXC_PARSE_MM_PARENTHESIS, t);
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
      throw ParseError(EXC_PARSE_OTHER_S, "Missing LOOP keyword in FOR statement.", t);
    s->_exec = parse_clause(p, ctx, s);
    t = p.pop();
    if (t->text != KEYWORDS[STMT_END])
      throw ParseError(EXC_PARSE_OTHER_S, "Endless FOR LOOP statement.", t);
    /* parse statement END */
    s->_exec->statements().push_back(ENDStatement::parse(p, ctx, STMT_ENDLOOP));
    t = p.pop();
    if (t->code != Parser::Separator)
      throw ParseError(EXC_PARSE_STATEMENT_END_S, t->text.c_str(), t);
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

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

void FORALLStatement::finalizeControl(Context& ctx, void * data) const
{
  RT * _data = reinterpret_cast<RT*>(data);

  /* restore the state of the iterator variable */
  Symbol& vs = ctx.getSymbol(_var->symbolId());
  ctx.loadVariable(vs.id()).swap(Value(_data->it_type_bak).to_lvalue(true));
  vs.safety(_data->it_safety_bak);
  vs.locked(_data->it_locked_bak);
  if (_exp->symbolId() != Expression::nid)
  {
    /* restore the state of the symbol */
    ctx.getSymbol(_exp->symbolId()).locked(_data->ex_locked_bak);
  }
  else
  {
    /* delete temporary storage */
    delete _data->target;
  }

  delete _data;
}

const Statement * FORALLStatement::doit(Context& ctx) const
{
  if (this != ctx.topControl())
  {
    RT * data = new RT();
    Value& val = _exp->value(ctx);
    if (val.isNull() || val.collection()->size() == 0)
      return _next;
    /* the variable, which will point to the fetched value, cannot already be
     * used by a running iteration i.e in parent loop; typically a protected
     * symbol can mean such a case */
    Symbol& vs = ctx.getSymbol(_var->symbolId());
    if (vs.safety())
      throw RuntimeError(EXC_RT_NOT_IMPLEMENTED);
    /* setup fetching order */
    if (_order == DESC)
    {
      data->step = -1;
      data->index = val.collection()->size()-1;
    }
    else
    {
      data->step = 1;
      data->index = 0;
    }
    /* setup fetched target */
    if (_exp->symbolId() != Expression::nid)
    {
      /* target is sustainable and accessible by a symbol, so point to it */
      data->target = &val;
      /* the symbol must be protected in the body loop */
      Symbol& es = ctx.getSymbol(_exp->symbolId());
      data->ex_locked_bak = es.locked();
      es.locked(true);
    }
    else if (!val.lvalue())
    {
      /* the target value is temporary and it must be preserved during the
       * loop execution, so move it in private stack */
      data->target = new Value(std::move(val.to_lvalue(true)));
    }
    else
    {
      throw RuntimeError(EXC_RT_NOT_IMPLEMENTED);
    }
    /* backup the state of the variable used as iterator; it will be restored
     * at end */
    data->it_safety_bak = vs.safety();
    data->it_locked_bak = vs.locked();
    data->it_type_bak = ctx.loadVariable(vs.id()).type();

    /* fetch first item: make a pointer to the element value */
    make_pointer(data->target->collection(), data->index,
              ctx.loadVariable(vs.id())).to_lvalue(true);
    vs.safety(true);
    /* iterator inherits constness of the target */
    vs.locked(data->ex_locked_bak);
    ctx.stackControl(this, data);
  }
  else
  {
    RT * data = reinterpret_cast<RT*>(ctx.topControlData());
    data->index += data->step;
    if (data->index < 0 || data->index >= data->target->collection()->size())
    {
      ctx.unstackControl();
      return _next;
    }
    else
    {
      /* fetch current item: make a pointer to the element value */
      make_pointer(data->target->collection(), data->index,
                ctx.loadVariable(_var->symbolId())).to_lvalue(true);
    }
  }

  /* it should run with the given context, and will throw on error */
  _exec->run(ctx, _exec->statements());

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
  fputs(ctx.getSymbol(_var->symbolId()).name().c_str(), out);
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
  fputc(Parser::NewLine, out);
  ctx.execBegin(this);
  _exec->unparse(out);
  ctx.execEnd();
}

Value& FORALLStatement::make_pointer(Collection * tgt, unsigned i, Value& itr) noexcept
{
  /* make a pointer to index value */
  itr = Value(&(tgt->at(i).to_lvalue(true)));
  return itr;
}

Executable * FORALLStatement::parse_clause(Parser& p, Context& ctx, FORALLStatement * rof)
{
  ctx.execBegin(rof);
  std::list<const Statement*> statements;
  /* get symbol non-const */
  Symbol& vt = ctx.getSymbol(rof->_var->symbolId());
  /* iterator must be protected against type change */
  bool safety_vt_bak = vt.safety();
  bool locked_vt_bak = vt.locked();
  vt.safety(true);
  /* fetched expression must be protected against change */
  bool locked_ex_bak = false;
  unsigned sid = rof->_exp->symbolId();
  if (sid != Expression::nid)
  {
    Symbol& es = ctx.getSymbol(sid);
    locked_ex_bak = es.locked();
    es.locked(true);
    /* iterator inherits constness of the target */
    vt.locked(locked_ex_bak);
  }
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
    if (sid != Expression::nid)
      ctx.getSymbol(sid).locked(locked_ex_bak);
    vt.safety(safety_vt_bak);
    vt.locked(locked_vt_bak);
    ctx.execEnd();
    for (auto ss : statements)
      delete ss;
    throw;
  }
  if (sid != Expression::nid)
    ctx.getSymbol(sid).locked(locked_ex_bak);
  vt.safety(safety_vt_bak);
  vt.locked(locked_vt_bak);
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
      throw ParseError(EXC_PARSE_OTHER_S, "Symbol of iterator required for FORALL.", t);
    /* check if keyword is reserved */
    if (p.reservedKeyword(t->text))
      throw ParseError(EXC_PARSE_RESERVED_WORD_S, t->text.c_str(), t);
    std::string vname = t->text;
    std::transform(vname.begin(), vname.end(), vname.begin(), ::toupper);
    {
      /* the variable, which will point to the fetched value, cannot already be
       * used by a running iteration i.e in parent loop; typically a protected
       * symbol can mean such a case */
      const Symbol * s = ctx.findSymbol(vname);
      if (s && s->safety())
        throw ParseError(EXC_PARSE_OTHER_S, "Cannot use a protected symbol as iterator variable.", t);
    }
    t = p.pop();
    if (t->code != TOKEN_KEYWORD || t->text != KEYWORDS[STMT_IN])
      throw ParseError(EXC_PARSE_OTHER_S, "Keyword IN required for FORALL.", t);
    s->_exp = ParseExpression::expression(p, ctx);
    t = p.pop();
    if (t->code == ')')
      throw ParseError(EXC_PARSE_MM_PARENTHESIS, t);
    /* check the type if defined */
    const Type& exp_type = s->_exp->type(ctx);
    if (exp_type == Type::NO_TYPE)
    {
      if (exp_type.level() > 0)
        s->_var = new VariableExpression(ctx.registerSymbol(vname, exp_type.levelDown()));
      else
        s->_var = new VariableExpression(ctx.registerSymbol(vname, exp_type));
    }
    else
    {
      if (exp_type.level() == 0)
        throw ParseError(EXC_PARSE_OTHER_S, "Table expression required for FORALL.", t);
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
      throw ParseError(EXC_PARSE_OTHER_S, "Missing LOOP keyword in FORALL statement.", t);
    s->_exec = parse_clause(p, ctx, s);
    t = p.pop();
    if (t->text != KEYWORDS[STMT_END])
      throw ParseError(EXC_PARSE_OTHER_S, "Endless FORALL LOOP statement.", t);
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

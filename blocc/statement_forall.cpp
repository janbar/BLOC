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
    /* the variable, which will point to the fetched value, cannot already be
     * used by a running iteration i.e in parent loop; typically a protected
     * symbol can mean such a case */
    if (_var->symbol()->safety())
      throw RuntimeError(EXC_RT_NOT_IMPLEMENTED);
    /* setup fetching order */
    if (_order == DESC)
    {
      _data.step = -1;
      _data.index = val.collection()->size()-1;
    }
    else
    {
      _data.step = 1;
      _data.index = 0;
    }
    /* setup fetched target */
    if (_expSymbol)
    {
      /* target is sustainable and accessible by a symbol, so point to it */
      _data.target = &val;
      /* the symbol must be type safe in the body loop */
      _data.ex_safety_bak = _expSymbol->safety();
      _expSymbol->safety(true);
    }
    else
    {
      /* target must be preserved during the duration of the loop;
       * the temporary storage will be free at end */
      if (val.lvalue())
        /* unfortunately I have to copy it */
        _data.target = new Value(std::move(val.clone().to_lvalue(true)));
      else
        _data.target = new Value(std::move(val.to_lvalue(true)));
    }
    /* backup the state of the variable used as iterator; it will be restored
     * at end */
    _data.it_safety_bak = _var->symbol()->safety();
    _data.it_type_bak = ctx.loadVariable(*_var->symbol()).type();

    /* fetch first item: exchange the value payload with a pointer to the
     * storage */
    swapValue(_data.target->collection(), _data.index,
              &ctx.loadVariable(*_var->symbol()))->to_lvalue(true);
    _var->symbol()->safety(true);
    ctx.stackControl(this);
  }
  else
  {
    /* restore the value payload into the target if it still exists */
    restoreValue(_data.target->collection(), _data.index,
                 &ctx.loadVariable(*_var->symbol()));
    _data.index += _data.step;
    if (_data.index < 0 || _data.index >= _data.target->collection()->size())
    {
      /* restore the state of the iterator variable; the value is cleared */
      ctx.loadVariable(*_var->symbol()).swap(Value(_data.it_type_bak).to_lvalue(true));
      _var->symbol()->safety(_data.it_safety_bak);
      if (_expSymbol)
      {
        /* restore the safety state of the target symbol */
        _expSymbol->safety(_data.ex_safety_bak);
      }
      else
      {
        /* delete temporary storage */
        delete _data.target;
      }
      ctx.unstackControl();
      return _next;
    }
    else
    {
      /* fetch current item: exchange the value payload with a pointer to the
       * storage */
      swapValue(_data.target->collection(), _data.index,
                &ctx.loadVariable(*_var->symbol()))->to_lvalue(true);
    }
  }
  try
  {
    /* it should run with the given context */
    _exec->run(ctx, _exec->statements());
  }
  catch (...)
  {
    /* restore the value payload into the target if it still exists */
    restoreValue(_data.target->collection(), _data.index,
                 &ctx.loadVariable(*_var->symbol()));
    /* restore the state of the iterator variable */
    ctx.loadVariable(*_var->symbol()).swap(Value(_data.it_type_bak).to_lvalue(true));
    _var->symbol()->safety(_data.it_safety_bak);
    if (_expSymbol)
    {
      /* restore the safety state of the symbol */
      _expSymbol->safety(_data.ex_safety_bak);
    }
    else
    {
      /* delete temporary storage */
      delete _data.target;
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
  fputc(Parser::NewLine, out);
  ctx.execBegin(this);
  _exec->unparse(out);
  ctx.execEnd();
}

Value * FORALLStatement::swapValue(Collection * tgt, unsigned i, Value * itr) noexcept
{
  /* make a pointer to iterator value i.e self: dangerous */
  *itr = Value(itr);
  /* exchange target value and iterator value (pointer) */
  tgt->at(i).swap(*itr);
  return itr;
}

void FORALLStatement::restoreValue(Collection * tgt, unsigned i, Value * itr) noexcept
{
  /* search the pointer to itr at the advised index */
  if (i < tgt->size())
  {
    Value& v = tgt->at(i);
    if (v.type() == Type::POINTER && v.get().p == itr)
    {
      v.swap(*itr);
      return;
    }
  }
  /* finally scan the whole table to find it, in case the item is moved */
  for (Value& v : *tgt)
  {
    if (v.type() == Type::POINTER && v.get().p == itr)
    {
      v.swap(*itr);
      return;
    }
  }
  /* item has been deleted from target */
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
      throw ParseError(EXC_PARSE_OTHER_S, "Symbol of iterator required for FORALL.");
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
     * point to the target table when processing the clause of the statement */
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
    /* a progressing iterator cannot be reused in the body loop; also its type
     * must be safe; therefore enable the safety flag for the symbol */
    if (s->_var->symbol()->safety())
      throw ParseError(EXC_PARSE_OTHER_S, "Cannot use a protected symbol as iterator variable.");
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
    if (t->code != Parser::Separator)
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

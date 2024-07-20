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

#include "statement_if.h"
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

IFStatement::~IFStatement()
{
  for (auto& r : _rules)
  {
    if (r.first)
      delete r.first;
    if (r.second)
      delete r.second;
  }
}

const Statement * IFStatement::doit(Context& ctx) const
{
  for (auto& r : _rules)
  {
    if (r.first)
    {
      Value& val = r.first->value(ctx);
      if (val.isNull() || !(*val.boolean()))
        continue;
    }
    /* it should run with the given context */
    r.second->run(ctx, r.second->statements());
    break;
  }
  return _next;
}

void IFStatement::unparse(Context& ctx, FILE * out) const
{
  auto it = _rules.begin();
  while (it != _rules.end())
  {
    if (it->first != nullptr)
    {
      if (it == _rules.begin())
        fputs(KEYWORDS[STMT_IF], out);
      else
      {
        for (size_t i = 0; i < ctx.execLevel(); ++i) fputs(Parser::Indent, out);
        fputs(KEYWORDS[STMT_ELSIF], out);
      }
      fputc(' ', out);
      fputs(it->first->unparse(ctx).c_str(), out);
      fputc(' ', out);
      fputs(KEYWORDS[STMT_THEN], out);
    }
    else
    {
      for (size_t i = 0; i < ctx.execLevel(); ++i) fputs(Parser::Indent, out);
      fputs(KEYWORDS[STMT_ELSE], out);
    }
    fputc(Parser::NewLine, out);
    ctx.execBegin(this);
    it->second->unparse(out);
    ctx.execEnd();
    ++it;
  }
}

Executable * IFStatement::parse_clause(Parser& p, Context& ctx)
{
  std::list<const Statement*> statements;
  try
  {
    bool end = false;
    TokenPtr t;
    while (!end && (t = p.pop()))
    {
      if (t->code == Parser::Separator)
        continue;
      /* check for ending */
      if (t->code == TOKEN_KEYWORD &&
              (t->text == KEYWORDS[STMT_END] || t->text == KEYWORDS[STMT_ELSIF] ||
               t->text == KEYWORDS[STMT_ELSE]))
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
    for (auto ss : statements)
      delete ss;
    throw;
  }
  return new Executable(ctx, statements);
}

IFStatement * IFStatement::parse(Parser& p, Context& ctx)
{
  IFStatement * s = new IFStatement();
  try
  {
    for (;;)
    {
      Expression * exp = ParseExpression::expression(p, ctx);
      const Type& exp_type = exp->type(ctx);
      if (exp_type.level() > 0 ||
              (exp_type != Type::BOOLEAN && exp_type != Type::NO_TYPE))
      {
        delete exp;
        throw ParseError(EXC_PARSE_OTHER_S, "Boolean expression required for IF.");
      }
      TokenPtr t = p.pop();
      if (t->code == ')')
      {
        delete exp;
        throw ParseError(EXC_PARSE_MM_PARENTHESIS);
      }
      if (t->code != TOKEN_KEYWORD || t->text != KEYWORDS[STMT_THEN])
      {
        delete exp;
        throw ParseError(EXC_PARSE_OTHER_S, "Missing THEN keyword in IF statement.");
      }
      Executable * exec = parse_clause(p, ctx);
      s->_rules.push_back(std::make_pair(exp, exec));
      t = p.pop();
      if (t->text == KEYWORDS[STMT_ELSIF])
        continue; /* process next rule */
      if (t->text == KEYWORDS[STMT_ELSE])
      {
        exec = parse_clause(p, ctx);
        s->_rules.push_back(std::make_pair(nullptr, exec));
        t = p.pop();
      }
      if (t->text != KEYWORDS[STMT_END])
        throw ParseError(EXC_PARSE_OTHER_S, "Endless IF statement.");
      /* parse statement END */
      exec->statements().push_back(ENDStatement::parse(p, ctx, STMT_ENDIF));
      t = p.pop();
      if (t->code != Parser::Separator)
        throw ParseError(EXC_PARSE_STATEMENT_END_S, t->text.c_str());
      break;
    }
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

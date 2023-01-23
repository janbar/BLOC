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
    if (r.first == nullptr || r.first->boolean(ctx))
    {
      ctx.execBegin(this);
      try
      {
        r.second->run();
      }
      catch (...)
      {
        ctx.execEnd();
        throw;
      }
      ctx.execEnd();
      break;
    }
  }
  return _next;
}

void IFStatement::unparse(Context& ctx, FILE * out) const
{
  fputs(KEYWORDS[keyword()], out);
  for (auto& r : _rules)
  {
    if (r.first)
    {
      fputs(" ", out);
      fputs(r.first->unparse(ctx).c_str(), out);
      fputs(" ", out);
      fputs(KEYWORDS[STMT_THEN], out);
    }
    fputc(Parser::NEWLINE, out);
    ctx.execBegin(this);
    r.second->unparse(out);
    ctx.execEnd();
  }
}

Executable * IFStatement::parse_clause(Parser& p, Context& ctx, Statement * fi)
{
  ctx.execBegin(fi);
  std::list<const Statement*> statements;
  try
  {
    bool end = false;
    TokenPtr t;
    while (!end && (t = p.pop()))
    {
      if (t->code == Parser::SEPARATOR)
        continue;
      /* parse the statement */
      p.push(t);
      Statement * ss = ParseStatement::statement(p, ctx);
      statements.push_back(ss);
      /* check for ending */
      switch (ss->keyword())
      {
      case Statement::STMT_ELSIF:
      case Statement::STMT_ELSE:
      case Statement::STMT_ENDIF:
        end = true;
        break;
      default:
        break;
      }
    }
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

IFStatement * IFStatement::parse(Parser& p, Context& ctx)
{
  IFStatement * s = new IFStatement();
  try
  {
    for (;;)
    {
      Expression * exp = ParseExpression::expression(p, ctx);
      if (exp->type(ctx) != Type::BOOLEAN)
      {
        delete exp;
        throw ParseError(EXC_PARSE_MESSAGE_S, "Boolean expression required for IF.");
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
        throw ParseError(EXC_PARSE_MESSAGE_S, "Missing THEN keyword in IF statement.");
      }
      Executable * exec = parse_clause(p, ctx, s);
      s->_rules.push_back(std::make_pair(exp, exec));
      if (exec->statements().back()->keyword() == STMT_ENDIF)
        break; /* end of statement IF */
      else if (exec->statements().back()->keyword() == STMT_ELSIF)
        continue; /* process next rule */
      else if (exec->statements().back()->keyword() == STMT_ELSE)
      {
        exec = parse_clause(p, ctx, s);
        if (exec->statements().back()->keyword() == STMT_ENDIF)
        {
          s->_rules.push_back(std::make_pair(nullptr, exec));
          break;
        }
        delete exec;
      }
      throw ParseError(EXC_PARSE_MESSAGE_S, "Endless IF statement.");
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

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

#include "parse_statement.h"
#include "exception_parse.h"
#include "tokenizer.h"
#include "token.h"
#include "parser.h"
#include "debug.h"

#include "statement_print.h"
#include "statement_let.h"
#include "statement_import.h"
#include "statement_for.h"
#include "statement_break.h"
#include "statement_continue.h"
#include "statement_return.h"
#include "statement_begin.h"
#include "statement_end.h"
#include "statement_while.h"
#include "statement_if.h"
#include "statement_put.h"
#include "statement_do.h"
#include "statement_trace.h"
#include "statement_raise.h"
#include "statement_function.h"
#include "statement_forall.h"
#include "expression_item.h"

#include <forward_list>

namespace bloc
{

Statement * ParseStatement::statement(Parser& p, Context& ctx)
{
  ParseStatement ps(p, ctx);
  return ps.parse();
}

Statement * ParseStatement::parse()
{
  Statement::STATEMENT sc;
  Statement * s = nullptr;
  TokenPtr t = p.pop();
  if (!t)
    return nullptr;

  try
  {
    switch (t->code)
    {
    case TOKEN_KEYWORD:
      /* check for statement */
      sc = (Statement::STATEMENT)Statement::findKeyword(t->text);
      if (sc != Statement::unknown)
      {
        switch (sc)
        {
        case Statement::STMT_NOP:
          s = new Statement(Statement::STMT_NOP);
          return beyond_statement(s);

        case Statement::STMT_TRACE:
          s = TRACEStatement::parse(p, ctx);
          return beyond_statement(s);

        case Statement::STMT_RETURN:
          s = RETURNStatement::parse(p, ctx);
          return beyond_statement(s);

        case Statement::STMT_FUNCTION:
          s = FUNCTIONStatement::parse(p, ctx);
          return s;

        case Statement::STMT_IF:
          s = IFStatement::parse(p, ctx);
          return s;

        case Statement::STMT_WHILE:
          s = WHILEStatement::parse(p, ctx);
          return s;

        case Statement::STMT_FOR:
          s = FORStatement::parse(p, ctx);
          return s;

        case Statement::STMT_FORALL:
          s = FORALLStatement::parse(p, ctx);
          return s;

        case Statement::STMT_LET:
          s = LETStatement::parse(p, ctx);
          return chain_statement(s);

        case Statement::STMT_PUT:
          s = PUTStatement::parse(p, ctx);
          return beyond_statement(s);

        case Statement::STMT_BEGIN:
          s = BEGINStatement::parse(p, ctx);
          return s;

        case Statement::STMT_BREAK:
          s = BREAKStatement::parse(p, ctx);
          return beyond_statement(s);

        case Statement::STMT_CONTINUE:
          s = CONTINUEStatement::parse(p, ctx);
          return beyond_statement(s);

        case Statement::STMT_PRINT:
          s = PRINTStatement::parse(p, ctx);
          return beyond_statement(s);

        case Statement::STMT_DO:
          s = DOStatement::parse(p, ctx);
          return beyond_statement(s);

        case Statement::STMT_RAISE:
          s = RAISEStatement::parse(p, ctx);
          return beyond_statement(s);

        case Statement::STMT_IMPORT:
        {
          s = IMPORTStatement::parse(p, ctx);
          /* perform loading now */
          dynamic_cast<IMPORTStatement*>(beyond_statement(s))->loadModule(ctx);
          return s;
        }

        default:
          break;
        }
      }
      else
      {
        /* check for an assignation */
        if (test_assignation())
        {
          p.push(t);
          s = LETStatement::parse(p, ctx);
          return chain_statement(s);
        }
        /* else parse DO expression */
        p.push(t);
        s = DOStatement::parse(p, ctx);
        return beyond_statement(s);
      }
      break;
    case Parser::SEPARATOR:
      return nullptr;
    default:
      break;
    }
    throw ParseError(EXC_PARSE_NOT_A_STATEMENT);
  }
  catch (ParseError& pe)
  {
    DBG(DBG_DEBUG, "exception %p at %s line %d\n", &pe, __PRETTY_FUNCTION__, __LINE__);
    if (s) delete s;
    throw;
  }
  return nullptr;
}

/**
 * Parse chained statement (,) or valid the final separator (;)
 */
Statement * ParseStatement::beyond_statement(Statement * s)
{
  TokenPtr t = p.pop();
  if (t->code == ')')
    throw ParseError(EXC_PARSE_MM_PARENTHESIS);
  if (t->code != Parser::SEPARATOR)
    throw ParseError(EXC_PARSE_STATEMENT_END_S, t->text.c_str());
  return s;
}

Statement * ParseStatement::chain_statement(Statement * s)
{
  TokenPtr t = p.pop();
  /* parse next chained or finish */
  if (t->code == Parser::CHAIN)
  {
    s->setNext(statement(p, ctx));
    return s;
  }
  p.push(t);
  return beyond_statement(s);
}

bool ParseStatement::test_assignation()
{
  /* is var assignation */
  if (p.front()->code == '=' || p.front()->code == TOKEN_ASSIGN)
    return true;
  return false;
}

}

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

#include "statement_end.h"
#include "exception_parse.h"
#include "parser.h"
#include "context.h"

namespace bloc
{

ENDStatement * ENDStatement::parse(Parser& p, Context& ctx)
{
  if (ctx.execStatement() == nullptr)
    throw ParseError(EXC_PARSE_MESSAGE_S, "unexpected END keyword.");

  ENDStatement * s = new ENDStatement();
  try
  {
    TokenPtr t = p.pop();
    if (ctx.execLevel() > 0)
    {
      switch (ctx.execStatement()->keyword())
      {
      case STMT_IF:
        if (t->text != KEYWORDS[STMT_IF])
          throw ParseError(EXC_PARSE_MESSAGE_S, "unexpected END keyword. Waiting for END IF.");
        s->_keyword = STMT_ENDIF;
        return s;
      case STMT_FOR:
      case STMT_WHILE:
        if (t->text != KEYWORDS[STMT_LOOP])
           throw ParseError(EXC_PARSE_MESSAGE_S, "unexpected END keyword. Waiting for END LOOP.");
        s->_keyword = STMT_ENDLOOP;
        return s;
      case STMT_BEGIN:
        if (t->code != Parser::SEPARATOR)
          throw ParseError(EXC_PARSE_MESSAGE_S, "Extra input beyond END keyword.");
        p.push(t);
        return s;
      default:
        break;
      }
    }
    throw ParseError(EXC_PARSE_MESSAGE_S, "unexpected END keyword.");
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

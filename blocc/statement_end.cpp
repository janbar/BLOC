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
#include "debug.h"

namespace bloc
{

ENDStatement * ENDStatement::parse(Parser& p, Context& ctx, STATEMENT endof)
{
  ENDStatement * s = new ENDStatement(endof);
  try
  {
    TokenPtr t = p.pop();
    switch (endof)
    {
    case STMT_ENDIF:
      if (t->text != KEYWORDS[STMT_IF])
        throw ParseError(EXC_PARSE_OTHER_S, "unexpected END keyword. Waiting for END IF.", t);
      return s;
    case STMT_ENDLOOP:
      if (t->text != KEYWORDS[STMT_LOOP])
         throw ParseError(EXC_PARSE_OTHER_S, "unexpected END keyword. Waiting for END LOOP.", t);
      return s;
    case STMT_END:
      if (t->code != Parser::Separator)
        throw ParseError(EXC_PARSE_OTHER_S, "Extra input beyond END keyword.", t);
      p.push(t);
      return s;
    default:
      throw ParseError(EXC_PARSE_OTHER_S, "unexpected END keyword.", t);
    }
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

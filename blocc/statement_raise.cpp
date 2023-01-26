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

#include "statement_raise.h"
#include "exception_parse.h"
#include "exception_runtime.h"
#include "parse_expression.h"
#include "parser.h"
#include "context.h"
#include "debug.h"

#include <string>
#include <algorithm>

namespace bloc
{

const Statement * RAISEStatement::doit(Context& ctx) const
{
  EXC_RT er = RuntimeError::findThrowable(_name);
  if (er == EXC_RT_USER_S)
    throw RuntimeError(er, _name.c_str());
  throw RuntimeError(er);
  return nullptr;
}

void RAISEStatement::unparse(Context& ctx, FILE * out) const
{
  fputs(Statement::KEYWORDS[keyword()], out);
  fputc(' ', out);
  fputs(_name.c_str(), out);
}

RAISEStatement * RAISEStatement::parse(Parser& p, Context& ctx)
{
  try
  {
    TokenPtr t = p.pop();
    if (t->code != TOKEN_KEYWORD)
      throw ParseError(EXC_PARSE_UNEXPECTED_LEX_S, t->text.c_str());
    if (Parser::reservedKeyword(t->text))
      throw ParseError(EXC_PARSE_RESERVED_WORD_S, t->text.c_str());
    std::string name = t->text;
    std::transform(name.begin(), name.end(), name.begin(), ::toupper);
    return new RAISEStatement(name);
  }
  catch (ParseError& pe)
  {
    DBG(DBG_DEBUG, "exception %p at %s line %d\n", &pe, __PRETTY_FUNCTION__, __LINE__);
    throw;
  }
  return nullptr;
}

}

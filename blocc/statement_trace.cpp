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

#include "statement_trace.h"
#include "exception_parse.h"
#include "parse_expression.h"
#include "parser.h"
#include "context.h"
#include "debug.h"

#include <string>

namespace bloc
{

TRACEStatement::~TRACEStatement()
{
  if (_exp)
    delete _exp;
}

const Statement * TRACEStatement::doit(Context& ctx) const
{
  Value& val = _exp->value(ctx);
  if (!val.isNull())
    ctx.trace(*val.boolean());
  return _next;
}

void TRACEStatement::unparse(Context& ctx, FILE * out) const
{
  fputs(Statement::KEYWORDS[keyword()], out);
  if (_exp != nullptr)
  {
    fputs(" ", out);
    fputs(_exp->unparse(ctx).c_str(), out);
  }
}

TRACEStatement * TRACEStatement::parse(Parser& p, Context& ctx)
{
  TRACEStatement * s = new TRACEStatement();
  try
  {
    s->_exp = ParseExpression::expression(p, ctx);
    if (s->_exp->type(ctx) != Type::BOOLEAN)
      throw ParseError(EXC_PARSE_NOT_BOOLEAN);
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


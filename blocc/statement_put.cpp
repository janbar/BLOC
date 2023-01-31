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

#include "statement_put.h"
#include "exception_parse.h"
#include "parse_expression.h"
#include "parser.h"
#include "context.h"
#include "expression_boolean.h"
#include "expression_integer.h"
#include "expression_numeric.h"
#include "expression_tabchar.h"
#include "debug.h"

#include <cstdio>
#include <string>

namespace bloc
{

PUTStatement::~PUTStatement()
{
  for (auto exp : _args)
    delete exp;
}

const Statement * PUTStatement::doit(Context& ctx) const
{
  for (const Expression * exp : _args)
  {
    const Type& exp_type = exp->type(ctx);
    if (exp_type.level() == 0)
    {
      switch (exp_type.major())
      {
      case Type::NO_TYPE:
        break;
      case Type::LITERAL:
        fputs(exp->literal(ctx).c_str(), ctx.ctxout());
        break;
      case Type::BOOLEAN:
        fputs(BooleanExpression::readableBoolean(exp->boolean(ctx)).c_str(), ctx.ctxout());
        break;
      case Type::INTEGER:
        fputs(IntegerExpression::readableInteger(exp->integer(ctx)).c_str(), ctx.ctxout());
        break;
      case Type::NUMERIC:
        fputs(NumericExpression::readableNumeric(exp->numeric(ctx)).c_str(), ctx.ctxout());
        break;
      case Type::COMPLEX:
        exp->complex(ctx); /* execute expression */
        break;
      case Type::TABCHAR:
        exp->tabchar(ctx); /* execute expression */
        break;
      case Type::ROWTYPE:
        exp->tuple(ctx); /* execute expression */
        break;
      default:
        throw RuntimeError(EXC_RT_NOT_IMPLEMENTED);
      }
    }
    else
    {
      exp->collection(ctx); /* execute expression */
    }
  }
  fflush(ctx.ctxout());
  return _next;
}

void PUTStatement::unparse(Context& ctx, FILE * out) const
{
  fputs(Statement::KEYWORDS[keyword()], out);
  if (_args.empty())
    return;
  for (const Expression * exp : _args)
  {
    fputs(" ", out);
    fputs(exp->unparse(ctx).c_str(), out);
  }
}

PUTStatement * PUTStatement::parse(Parser& p, Context& ctx)
{
  PUTStatement * s = new PUTStatement();
  try
  {
    while (p.front()->code != Parser::SEPARATOR)
      s->_args.push_back(ParseExpression::expression(p, ctx));
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

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
#include "value.h"
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
    Value& val = exp->value(ctx);
    if (!val.isNull())
    {
      const Type& val_type = val.type();
      if (val_type.level() == 0)
      {
        switch (val_type.major())
        {
        case Type::LITERAL:
          fputs(val.literal()->c_str(), ctx.ctxout());
          break;
        case Type::BOOLEAN:
          fputs(Value::readableBoolean(*val.boolean()).c_str(), ctx.ctxout());
          break;
        case Type::INTEGER:
          fputs(Value::readableInteger(*val.integer()).c_str(), ctx.ctxout());
          break;
        case Type::NUMERIC:
          fputs(Value::readableNumeric(*val.numeric()).c_str(), ctx.ctxout());
          break;
        case Type::COMPLEX:
        case Type::TABCHAR:
        case Type::ROWTYPE:
          break;
        default:
          throw RuntimeError(EXC_RT_NOT_IMPLEMENTED);
        }
      }
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

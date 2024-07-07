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

#include "statement_print.h"
#include "exception_parse.h"
#include "parse_expression.h"
#include "collection.h"
#include "parser.h"
#include "context.h"
#include "debug.h"

#include <cstdio>
#include <string>

namespace bloc
{

PRINTStatement::~PRINTStatement()
{
  for (auto exp : _args)
    delete exp;
}

const Statement * PRINTStatement::doit(Context& ctx) const
{
  for (const Expression * exp : _args)
  {
    Value& val = exp->value(ctx);
    const Type& val_type = val.type();
    if (val_type.level() == 0)
    {
      //if (val_type == Type::NO_TYPE)
      //  throw RuntimeError(EXC_RT_OPAQUE_INLINE);
      if (val.isNull())
        fputs(Value::STR_NIL, ctx.ctxout());
      else
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
        case Type::IMAGINARY:
          fputs(Value::readableImaginary(*val.imaginary()).c_str(), ctx.ctxout());
          break;
        case Type::COMPLEX:
          fputs(Value::readableComplex(*val.complex()).c_str(), ctx.ctxout());
          break;
        case Type::TABCHAR:
          Value::outputTabchar(*val.tabchar(), ctx.ctxout());
          break;
        case Type::ROWTYPE:
          fputs(Value::readableTuple(*val.tuple()).c_str(), ctx.ctxout());
          break;
        default:
          throw RuntimeError(EXC_RT_NOT_IMPLEMENTED);
        }
    }
    else
    {
      fputs(val.typeName().c_str(), ctx.ctxout());
      if (!val.isNull())
      {
        size_t sz = val.collection()->size(); /* execute expression */
        fputc('[', ctx.ctxout());
        fputs(std::to_string(sz).c_str(), ctx.ctxout());
        fputc(']', ctx.ctxout());
      }
    }
  }
  fputc('\n', ctx.ctxout());
  fflush(ctx.ctxout());
  return _next;
}

void PRINTStatement::unparse(Context& ctx, FILE * out) const
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

PRINTStatement * PRINTStatement::parse(Parser& p, Context& ctx)
{
  PRINTStatement * s = new PRINTStatement();
  try
  {
    while (p.front()->code != Parser::Separator)
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

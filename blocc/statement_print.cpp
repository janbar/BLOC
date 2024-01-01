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
#include "parser.h"
#include "context.h"
#include "expression_boolean.h"
#include "expression_numeric.h"
#include "expression_integer.h"
#include "expression_complex.h"
#include "expression_tabchar.h"
#include "expression_collection.h"
#include "expression_tuple.h"
#include "plugin_manager.h"
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
    const Type& exp_type = exp->type(ctx);
    if (exp_type == Type::NO_TYPE)
    {
      /* nop */
    }
    else if (exp_type.level() == 0)
    {
      switch (exp_type.major())
      {
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
        fputs(ComplexExpression::readableComplex(exp->complex(ctx)).c_str(), ctx.ctxout());
        break;
      case Type::TABCHAR:
        TabcharExpression::outputTabchar(exp->tabchar(ctx), ctx.ctxout());
        break;
      case Type::ROWTYPE:
        fputs(TupleExpression::readableTuple(exp->tuple(ctx), ctx).c_str(), ctx.ctxout());
        break;
      default:
        throw RuntimeError(EXC_RT_NOT_IMPLEMENTED);
      }
    }
    else
    {
      fputs(exp->typeName(ctx).c_str(), ctx.ctxout());
      fputc('[', ctx.ctxout());
      fputs(std::to_string(exp->collection(ctx).size()).c_str(), ctx.ctxout());
      fputc(']', ctx.ctxout());
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

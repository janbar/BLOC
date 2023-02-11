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

#include "statement_return.h"
#include "exception_parse.h"
#include "parse_expression.h"
#include "parser.h"
#include "context.h"
#include "expression_boolean.h"
#include "expression_numeric.h"
#include "expression_integer.h"
#include "expression_literal.h"
#include "expression_tuple.h"
#include "debug.h"

#include <cstdio>
#include <string>

namespace bloc
{

RETURNStatement::~RETURNStatement()
{
  if (_exp)
    delete _exp;
}

const Statement * RETURNStatement::doit(Context& ctx) const
{
  if (_exp == nullptr)
  {
    ctx.saveReturned(nullptr);
  }
  else
  {
    const Type& exp_type = _exp->type(ctx);
    if (exp_type.level() == 0)
    {
      switch (exp_type.major())
      {
      case Type::NO_TYPE:
        ctx.saveReturned(nullptr);
        break;
      case Type::LITERAL:
        if (_exp->isRvalue())
          ctx.saveReturned(new LiteralExpression(std::move(_exp->literal(ctx))));
        else
          ctx.saveReturned(new LiteralExpression(_exp->literal(ctx)));
        break;
      case Type::BOOLEAN:
        ctx.saveReturned(new BooleanExpression(_exp->boolean(ctx)));
        break;
      case Type::INTEGER:
        ctx.saveReturned(new IntegerExpression(_exp->integer(ctx)));
        break;
      case Type::NUMERIC:
        ctx.saveReturned(new NumericExpression(_exp->numeric(ctx)));
        break;
      case Type::COMPLEX:
        _exp->complex(ctx);
        ctx.saveReturned(nullptr);
        break;
      case Type::TABCHAR:
        _exp->tabchar(ctx);
        ctx.saveReturned(nullptr);
        break;
      case Type::ROWTYPE:
        if (_exp->isRvalue())
          ctx.saveReturned(new TupleExpression(std::move(_exp->tuple(ctx))));
        else
          ctx.saveReturned(new TupleExpression(_exp->tuple(ctx)));
        break;
      default:
        throw RuntimeError(EXC_RT_NOT_IMPLEMENTED);
      }
    }
    else
    {
      _exp->collection(ctx);
      ctx.saveReturned(nullptr);
    }
  }
  ctx.returnCondition(true);
  return _next;
}

void RETURNStatement::unparse(Context& ctx, FILE * out) const
{
  fputs(Statement::KEYWORDS[keyword()], out);
  if (_exp != nullptr)
  {
    fputs(" ", out);
    fputs(_exp->unparse(ctx).c_str(), out);
  }
}

RETURNStatement * RETURNStatement::parse(Parser& p, Context& ctx)
{
  RETURNStatement * s = new RETURNStatement();
  TokenPtr t = p.front();
  if (t->code == Parser::SEPARATOR)
    return s;
  try
  {
    s->_exp = ParseExpression::expression(p, ctx);
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

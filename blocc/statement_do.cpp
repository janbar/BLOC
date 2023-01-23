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

#include "statement_do.h"
#include "exception_parse.h"
#include "parse_expression.h"
#include "parser.h"
#include "context.h"
#include "debug.h"

namespace bloc
{

DOStatement::~DOStatement()
{
  if (_exp)
    delete _exp;
}

const Statement * DOStatement::doit(Context& ctx) const
{
  if (_exp != nullptr)
  {
    const Type& exp_type = _exp->type(ctx);
    if (exp_type.level() == 0)
    {
      switch (exp_type.major())
      {
      case Type::NO_TYPE:
        break;
      case Type::BOOLEAN:
        _exp->boolean(ctx);
        break;
      case Type::INTEGER:
        _exp->integer(ctx);
        break;
      case Type::NUMERIC:
        _exp->numeric(ctx);
        break;
      case Type::LITERAL:
        _exp->literal(ctx);
        break;
      case Type::COMPLEX:
        _exp->complex(ctx);
        break;
      case Type::TABCHAR:
        _exp->tabchar(ctx);
        break;
      case Type::ROWTYPE:
        _exp->tuple(ctx);
        break;
      }
    }
    else
    {
      _exp->collection(ctx);
    }
  }
  return _next;
}

void DOStatement::unparse(Context& ctx, FILE * out) const
{
  if (_exp != nullptr)
    fputs(_exp->unparse(ctx).c_str(), out);
}

DOStatement * DOStatement::parse(Parser& p, Context& ctx)
{
  DOStatement * s = new DOStatement();
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

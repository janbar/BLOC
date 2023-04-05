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

#include "expression_item.h"
#include "expression_complex.h"
#include "parse_expression.h"
#include "exception_parse.h"
#include "context.h"
#include "parser.h"

namespace bloc
{

Type ItemExpression::_opaque = Type(Type::NO_TYPE);

ItemExpression::~ItemExpression()
{
  if (_exp)
    delete _exp;
}

const Type& ItemExpression::type(Context&ctx) const
{
  /* tuple is opaque */
  if (_exp->type(ctx).minor() == 0)
    return _opaque;
  if (_index < _exp->tuple_decl(ctx).size())
    return _exp->tuple_decl(ctx)[_index];
  return ComplexExpression::null;
}

bool ItemExpression::boolean(Context& ctx) const
{
  if (_index < _exp->tuple_decl(ctx).size())
    return _exp->tuple(ctx)[_index]->boolean(ctx);
  throw RuntimeError(EXC_RT_INDEX_RANGE_S, std::to_string(_index).c_str());
}

int64_t ItemExpression::integer(Context& ctx) const
{
  if (_index < _exp->tuple_decl(ctx).size())
    return _exp->tuple(ctx)[_index]->integer(ctx);
  throw RuntimeError(EXC_RT_INDEX_RANGE_S, std::to_string(_index).c_str());
}

double ItemExpression::numeric(Context& ctx) const
{
  if (_index < _exp->tuple_decl(ctx).size())
    return _exp->tuple(ctx)[_index]->numeric(ctx);
  throw RuntimeError(EXC_RT_INDEX_RANGE_S, std::to_string(_index).c_str());
}

std::string& ItemExpression::literal(Context& ctx) const
{
  if (_index < _exp->tuple_decl(ctx).size())
    return _exp->tuple(ctx)[_index]->literal(ctx);
  throw RuntimeError(EXC_RT_INDEX_RANGE_S, std::to_string(_index).c_str());
}

TabChar& ItemExpression::tabchar(Context& ctx) const
{
  if (_index < _exp->tuple_decl(ctx).size())
    return _exp->tuple(ctx)[_index]->tabchar(ctx);
  throw RuntimeError(EXC_RT_INDEX_RANGE_S, std::to_string(_index).c_str());
}

Complex& ItemExpression::complex(Context& ctx) const
{
  if (_index < _exp->tuple_decl(ctx).size())
    return _exp->tuple(ctx)[_index]->complex(ctx);
  throw RuntimeError(EXC_RT_INDEX_RANGE_S, std::to_string(_index).c_str());
}

std::string ItemExpression::unparse(Context& ctx) const
{
  return _exp->unparse(ctx)
      .append(1, OPERATOR).append(std::to_string(_index + 1));
}

ItemExpression * ItemExpression::parse(Parser& p, Context& ctx, Expression * exp)
{
  const Type& exp_type = exp->type(ctx);
  TokenPtr t = p.pop();
  if (t->code != TOKEN_INTEGER)
    throw ParseError(EXC_PARSE_INV_EXPRESSION);
  unsigned item_no = (unsigned)std::stoul(t->text, 0, 10);
  switch (exp_type.major())
  {
  case Type::NO_TYPE:
  case Type::ROWTYPE:
    if (exp_type.minor() == 0) /* opaque */
    {
      if (!exp->isStored())
        throw ParseError(EXC_PARSE_OPAQUE_INLINE);
    }
    else
    {
      if (item_no < 1 || item_no > exp->tuple_decl(ctx).size())
        throw ParseError(EXC_PARSE_OUT_OF_INDICE, std::to_string(item_no).c_str());
    }
    return new ItemExpression(exp, item_no - 1);
  default:
    throw ParseError(EXC_PARSE_NOT_ROWTYPE);
  }
  return nullptr;
}

}

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

#include "member_count.h"
#include <blocc/parse_expression.h>
#include <blocc/exception_parse.h>
#include <blocc/expression_integer.h>
#include <blocc/expression_literal.h>
#include <blocc/expression_tabchar.h>
#include <blocc/expression_collection.h>
#include <blocc/context.h>
#include <blocc/parser.h>

#include <cstring>
#include <cassert>

namespace bloc
{

int64_t MemberCOUNTExpression::integer(Context& ctx) const
{
  const Type& exp_type = _exp->type(ctx);
  if (exp_type.level() == 0)
  {
    switch (exp_type.major())
    {
    case Type::LITERAL:
      return (int64_t)_exp->literal(ctx).size();
    case Type::TABCHAR:
      return (int64_t)_exp->tabchar(ctx).size();
    default:
      throw RuntimeError(EXC_RT_MEMB_ARG_TYPE_S, KEYWORDS[_builtin]);
    }
  }
  else
  {
    return (int64_t)_exp->collection(ctx).size();
  }
}

MemberCOUNTExpression * MemberCOUNTExpression::parse(Parser& p, Context& ctx, Expression * exp)
{
  TokenPtr t = p.pop();

  if (t->code != '(')
    throw ParseError(EXC_PARSE_BAD_MEMB_CALL_S, KEYWORDS[BTM_COUNT]);
  try
  {
    const Type& exp_type = exp->type(ctx);
    /* supported type: collection, literal, tabchar */
    if (exp_type.level() == 0)
    {
      switch (exp_type.major())
      {
      case Type::LITERAL:
      case Type::TABCHAR:
        break;
      default:
        throw ParseError(EXC_PARSE_MEMB_NOT_IMPL_S, KEYWORDS[BTM_COUNT]);
      }
    }

    assertClosedMember(p, ctx, KEYWORDS[BTM_COUNT]);
    return new MemberCOUNTExpression(exp);
  }
  catch (ParseError& pe)
  {
    DBG(DBG_DEBUG, "exception %p at %s line %d\n", &pe, __PRETTY_FUNCTION__, __LINE__);
    throw;
  }
  return nullptr;
}

}

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
#include "blocc/tuple.h"
#include <blocc/parse_expression.h>
#include <blocc/exception_parse.h>
#include <blocc/collection.h>
#include <blocc/context.h>
#include <blocc/parser.h>
#include <blocc/debug.h>

#include <cstring>
#include <cassert>

namespace bloc
{

Value& MemberCOUNTExpression::value(Context& ctx) const
{
  Value& val = _exp->value(ctx);
  Value v = Value(Value::type_integer);

  if (!val.isNull())
  {
    const Type& exp_type = val.type();
    if (exp_type.level() == 0)
    {
      switch (exp_type.major())
      {
      case Type::LITERAL:
        v = Value(Integer(val.literal()->size()));
        break;
      case Type::TABCHAR:
        v = Value(Integer(val.tabchar()->size()));
        break;
      case Type::ROWTYPE:
        v = Value(Integer(val.tuple()->size()));
        break;
      default:
        throw RuntimeError(EXC_RT_MEMB_ARG_TYPE_S, KEYWORDS[_builtin]);
      }
    }
    else
    {
      v = Value(Integer(val.collection()->size()));
    }
  }
  if (val.lvalue())
    return ctx.allocate(std::move(v));
  val.swap(std::move(v));
  return val;
}

MemberCOUNTExpression * MemberCOUNTExpression::parse(Parser& p, Context& ctx, Expression * exp)
{
  TokenPtr t = p.pop();

  if (t->code != '(')
    throw ParseError(EXC_PARSE_BAD_MEMB_CALL_S, KEYWORDS[BTM_COUNT], t);
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
      case Type::ROWTYPE:
      case Type::NO_TYPE: /* opaque */
        break;
      default:
        throw ParseError(EXC_PARSE_MEMB_NOT_IMPL_S, KEYWORDS[BTM_COUNT], t);
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

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

#include "member_delete.h"
#include "blocc/expression.h"
#include "blocc/value.h"
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

Value& MemberDELETEExpression::value(Context& ctx) const
{
  Value& val = _exp->value(ctx);
  Value& a0 = _args[0]->value(ctx);
  if (val.isNull() || a0.isNull())
    throw RuntimeError(EXC_RT_INDEX_RANGE_S, a0.toString().c_str());

  if (val.type().level() > 0)
  {
    /* collection */
    Collection * rv = val.collection();
    Integer p = *a0.integer();
    if (p < 0 || p >= rv->size())
      throw RuntimeError(EXC_RT_INDEX_RANGE_S, a0.toString().c_str());
    rv->erase(rv->begin() + p);
    return val;
  }

  switch (val.type().major())
  {
    /* literal */
  case Type::LITERAL:
  {
    Literal * rv = val.literal();
    Integer p = *a0.integer();
    if (p < 0 || p >= rv->size())
      throw RuntimeError(EXC_RT_INDEX_RANGE_S, a0.toString().c_str());
    if (_exp->isConst())
    {
      Value v(new Literal(*rv));
      Literal * tmp = v.literal();
      tmp->erase(tmp->begin() + p);
      return ctx.allocate(std::move(v));
    }
    rv->erase(rv->begin() + p);
    return val;
  }
    /* tabchar */
  case Type::TABCHAR:
  {
    TabChar * rv = val.tabchar();
    Integer p = *a0.integer();
    if (p < 0 || p >= rv->size())
      throw RuntimeError(EXC_RT_INDEX_RANGE_S, a0.toString().c_str());
    rv->erase(rv->begin() + p);
    return val;
  }
  default:
    break;
  }
  throw RuntimeError(EXC_RT_MEMB_NOT_IMPL_S, KEYWORDS[BTM_DELETE]);
}

MemberDELETEExpression * MemberDELETEExpression::parse(Parser& p, Context& ctx, Expression * exp)
{
  std::vector<Expression*> args;
  TokenPtr t = p.pop();
  {
    const Symbol * s = exp->symbol();
    if (s && s->locked())
      throw ParseError(EXC_PARSE_CONST_VIOLATION_S, s->name().c_str(), t);
  }

  if (t->code != '(')
    throw ParseError(EXC_PARSE_BAD_MEMB_CALL_S, KEYWORDS[BTM_DELETE], t);
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
      case Type::NO_TYPE: /* opaque */
        break;
      default:
        throw ParseError(EXC_PARSE_MEMB_NOT_IMPL_S, KEYWORDS[BTM_DELETE], t);
      }
    }

    args.push_back(ParseExpression::expression(p, ctx));
    if (!ParseExpression::typeChecking(args.back(), Type::INTEGER, p, ctx))
      throw ParseError(EXC_PARSE_MEMB_ARG_TYPE_S, KEYWORDS[BTM_DELETE], t);
    assertClosedMember(p, ctx, KEYWORDS[BTM_DELETE]);
    return new MemberDELETEExpression(exp, std::move(args));
  }
  catch (ParseError& pe)
  {
    DBG(DBG_DEBUG, "exception %p at %s line %d\n", &pe, __PRETTY_FUNCTION__, __LINE__);
    for (Expression * e : args)
      delete e;
    throw;
  }
  return nullptr;
}

}

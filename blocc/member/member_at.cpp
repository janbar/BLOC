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

#include "member_at.h"
#include <blocc/parse_expression.h>
#include <blocc/exception_parse.h>
#include <blocc/collection.h>
#include <blocc/tuple.h>
#include <blocc/context.h>
#include <blocc/parser.h>
#include <blocc/debug.h>

#include <cstring>
#include <cassert>

namespace bloc
{

const Type& MemberATExpression::type(Context& ctx) const
{
  const Type& exp_type = _exp->type(ctx);
  if (exp_type.level() == 0)
  {
    switch (exp_type.major())
    {
    case Type::LITERAL:
    case Type::TABCHAR:
      return Value::type_integer;
    case Type::NO_TYPE: /* opaque */
      return exp_type;
    default:
      return Value::type_no_type;
    }
  }
  else
  {
    /* return lower level of volatile level */
    _type_volatile = exp_type.levelDown();
    return _type_volatile;
  }
}

Value& MemberATExpression::value(Context& ctx) const
{
  Value& val = _exp->value(ctx);
  Value& a0 = _args[0]->value(ctx);
  if (val.isNull())
    throw RuntimeError(EXC_RT_INDEX_RANGE_S, a0.toString().c_str());

  /* collection */
  if (val.type().level() > 0)
  {
    if (a0.isNull())
      return ctx.allocate(Value(val.type().levelDown()));
    Collection * rv = val.collection();
    Integer p = *a0.integer();
    if (p >= 0 && p < rv->size())
      return rv->at((unsigned)p).to_lvalue(val.lvalue());
    throw RuntimeError(EXC_RT_INDEX_RANGE_S, a0.toString().c_str());
  }

  if (a0.isNull())
    return ctx.allocate(Value(val.type()));
  switch (val.type().major())
  {
  case Type::LITERAL:
  {
    Literal * rv = val.literal();
    Integer p = *a0.integer();
    if (p >= 0 && p < rv->size())
      return ctx.allocate(Value(Integer(rv->at((unsigned) p))));
    throw RuntimeError(EXC_RT_INDEX_RANGE_S, a0.toString().c_str());
  }
  case Type::TABCHAR:
  {
    TabChar * rv = val.tabchar();
    Integer p = *a0.integer();
    if (p >= 0 && p < rv->size())
      return ctx.allocate(Value(Integer(rv->at((unsigned) p))));
    throw RuntimeError(EXC_RT_INDEX_RANGE_S, a0.toString().c_str());
  }
  default:
    break;
  }
  throw RuntimeError(EXC_RT_MEMB_ARG_TYPE_S, KEYWORDS[_builtin]);
}

const TupleDecl::Decl& MemberATExpression::tuple_decl(Context& ctx) const
{
  return _exp->tuple_decl(ctx);
}

MemberATExpression * MemberATExpression::parse(Parser& p, Context& ctx, Expression * exp)
{
  std::vector<Expression*> args;
  TokenPtr t = p.pop();

  if (t->code != '(')
    throw ParseError(EXC_PARSE_BAD_MEMB_CALL_S, KEYWORDS[BTM_AT]);
  try
  {
    args.push_back(ParseExpression::expression(p, ctx));
    if (!ParseExpression::typeChecking(args.back(), Type::INTEGER, p, ctx))
      throw ParseError(EXC_PARSE_MEMB_ARG_TYPE_S, KEYWORDS[BTM_AT]);

    const Type& exp_type = exp->type(ctx);
    /* supported type: collection, literal, tabchar */
    if (exp_type.level() == 0)
    {
      switch (exp_type.major())
      {
      case Type::NO_TYPE: /* opaque */
      case Type::LITERAL:
      case Type::TABCHAR:
        break;
      default:
        throw ParseError(EXC_PARSE_MEMB_NOT_IMPL_S, KEYWORDS[BTM_AT]);
      }
    }

    assertClosedMember(p, ctx, KEYWORDS[BTM_AT]);
    return new MemberATExpression(exp, std::move(args));
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


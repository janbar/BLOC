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
#include <blocc/parse_expression.h>
#include <blocc/exception_parse.h>
#include <blocc/expression_boolean.h>
#include <blocc/expression_integer.h>
#include <blocc/expression_numeric.h>
#include <blocc/expression_literal.h>
#include <blocc/expression_complex.h>
#include <blocc/expression_tabchar.h>
#include <blocc/expression_collection.h>
#include <blocc/context.h>
#include <blocc/parser.h>

#include <cstring>
#include <cassert>

namespace bloc
{

std::string& MemberDELETEExpression::literal(Context& ctx) const
{
  /* literal */
  std::string& rv = _exp->literal(ctx);
  int64_t p = _args[0]->integer(ctx);
  if (p < 0 || p >= rv.size())
    throw RuntimeError(EXC_RT_INDEX_RANGE_S, std::to_string(p).c_str());
  rv.erase(rv.begin() + p);
  return rv;
}

TabChar& MemberDELETEExpression::tabchar(Context& ctx) const
{
  /* tabchar */
  TabChar& rv = _exp->tabchar(ctx);
  int64_t p = _args[0]->integer(ctx);
  if (p < 0 || p >= rv.size())
    throw RuntimeError(EXC_RT_INDEX_RANGE_S, std::to_string(p).c_str());
  rv.erase(rv.begin() + p);
  return rv;
}

Collection& MemberDELETEExpression::collection(Context& ctx) const
{
  /* collection */
  Collection& rv = _exp->collection(ctx);
  int64_t p = _args[0]->integer(ctx);
  if (p < 0 || p >= rv.size())
    throw RuntimeError(EXC_RT_INDEX_RANGE_S, std::to_string(p).c_str());
  rv.erase(rv.begin() + p);
  return rv;
}

MemberDELETEExpression * MemberDELETEExpression::parse(Parser& p, Context& ctx, Expression * exp)
{
  std::vector<Expression*> args;
  TokenPtr t = p.pop();

  if (t->code != '(')
    throw ParseError(EXC_PARSE_BAD_MEMB_CALL_S, KEYWORDS[BTM_DELETE]);
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
        throw ParseError(EXC_PARSE_MEMB_NOT_IMPL_S, KEYWORDS[BTM_DELETE]);
      }
    }

    args.push_back(ParseExpression::expression(p, ctx));
    if (!ParseExpression::typeChecking(args.back(), Type::INTEGER, p, ctx))
      throw ParseError(EXC_PARSE_MEMB_ARG_TYPE_S, KEYWORDS[BTM_DELETE]);
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

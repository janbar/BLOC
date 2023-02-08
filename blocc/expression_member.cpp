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

#include "expression_member.h"
#include "member/member_at.h"
#include "member/member_concat.h"
#include "member/member_count.h"
#include "member/member_put.h"
#include "member/member_delete.h"
#include "member/member_insert.h"
#include "member/member_set.h"
#include "member/member_complex.h"

#include "exception_parse.h"
#include "expression_variable.h"
#include "expression_item.h"
#include "context.h"
#include "parser.h"
#include "debug.h"

#include <cstring>
#include <cassert>

namespace bloc
{

const char * MemberExpression::KEYWORDS[] = {
  "",         "concat",     "at",       "put",      "count",
  "delete",   "insert",     "set",
};

MemberExpression::~MemberExpression()
{
  if (_exp)
    delete _exp;
  for (Expression * e : _args)
    delete e;
  _args.clear();
}

std::string MemberExpression::unparse(Context& ctx) const
{
  std::string sb(_exp->unparse(ctx).append(1, OPERATOR));
  sb.append(KEYWORDS[_builtin]);
  sb.append("(");
  for (int i = 0; i < _args.size(); ++i)
  {
    if (i > 0)
      sb.append(1, Parser::CHAIN).append(1, ' ');
    sb.append(_args[i]->unparse(ctx));
  }
  sb.append(1, ')');
  return sb;
}

int MemberExpression::findBuiltinKeyword(const std::string& s)
{
  for (int i = 0; i < (sizeof (KEYWORDS) / sizeof (char*)); ++i)
  {
    if (strcmp(KEYWORDS[i], s.c_str()) == 0)
      return i;
  }
  return unknown;
}

void MemberExpression::assertClosedMember(Parser& p, Context& ctx, const char * member)
{
  TokenPtr t = p.pop();
  /* check for enclosure */
  if (t->code != ')')
    throw ParseError(EXC_PARSE_MEMB_ARG_NUM_S, member);
}

MemberExpression * MemberExpression::expression(Parser& p, Context& ctx, Expression * exp)
{
  try
  {
    const Type& exp_type = exp->type(ctx);
    if (exp_type.level() == 0)
    {
      switch (exp_type.major())
      {
        /* builtin member is available for those types */
      case Type::COMPLEX:
        return MemberMETHODExpression::parse(p, ctx, exp);
      case Type::LITERAL:
      case Type::TABCHAR:
      case Type::ROWTYPE:
      case Type::NO_TYPE: /* opaque */
        return parse_builtin(p, ctx, exp);
      default:
        throw ParseError(EXC_PARSE_INV_EXPRESSION);
      }
    }
    else
    {
      return parse_builtin(p, ctx, exp);
    }
  }
  catch (ParseError& pe)
  {
    DBG(DBG_DEBUG, "exception %p at %s line %d\n", &pe, __PRETTY_FUNCTION__, __LINE__);
    throw;
  }
  return nullptr;
}

MemberExpression * MemberExpression::parse_builtin(Parser& p, Context& ctx, Expression * exp)
{
  TokenPtr t = p.pop();

  /* check all types of keywords */
  BTMETHOD mc = (BTMETHOD)findBuiltinKeyword(t->text);
  if (mc == unknown)
    throw ParseError(EXC_PARSE_MEMB_NOT_IMPL_S, t->text.c_str());

  /* member expression could alter the content of the source,
   * so the targeted expression must be rvalue else an instance
   * of variable or member, but NOT a constant */
  switch (mc)
  {
    /* exceptions */
  case BTM_COUNT:
  case BTM_AT:
    break;
  default:
    if (!exp->isRvalue(ctx))
    {
      if (!dynamic_cast<VariableExpression*>(exp) &&
              !dynamic_cast<MemberExpression*>(exp) &&
              !dynamic_cast<ItemExpression*>(exp))
        throw ParseError(EXC_PARSE_MEMB_NOT_IMPL_S, t->text.c_str());
    }
  }

  switch (mc)
  {
  case BTM_CONCAT:
    return MemberCONCATExpression::parse(p, ctx, exp);
  case BTM_COUNT:
    return MemberCOUNTExpression::parse(p, ctx, exp);
  case BTM_AT:
    return MemberATExpression::parse(p, ctx, exp);
  case BTM_PUT:
    return MemberPUTExpression::parse(p, ctx, exp);
  case BTM_DELETE:
    return MemberDELETEExpression::parse(p, ctx, exp);
  case BTM_INSERT:
    return MemberINSERTExpression::parse(p, ctx, exp);
  case BTM_SET:
    return MemberSETExpression::parse(p, ctx, exp);
  default:
    throw ParseError(EXC_PARSE_MEMB_NOT_IMPL_S, KEYWORDS[mc]);
  }
  return nullptr;
}

}

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

#include "member_set.h"
#include <blocc/parse_expression.h>
#include <blocc/exception_parse.h>
#include <blocc/expression_boolean.h>
#include <blocc/expression_integer.h>
#include <blocc/expression_numeric.h>
#include <blocc/expression_literal.h>
#include <blocc/expression_complex.h>
#include <blocc/expression_tabchar.h>
#include <blocc/expression_collection.h>
#include "blocc/expression_item.h"
#include <blocc/context.h>
#include <blocc/parser.h>
#include <blocc/debug.h>

#include <cstring>
#include <cassert>

namespace bloc
{

Tuple& MemberSETExpression::tuple(Context& ctx) const
{
  /* tabchar */
  Tuple& rv = _exp->tuple(ctx);
  if (_index < rv.tuple_decl().size())
  {
    switch (rv.tuple_decl()[_index].major())
    {
    case Type::BOOLEAN:
      rv[_index]->refBoolean() = _args[0]->boolean(ctx);
      break;
    case Type::INTEGER:
      rv[_index]->refInteger() = _args[0]->integer(ctx);
      break;
    case Type::NUMERIC:
      rv[_index]->refNumeric() = _args[0]->numeric(ctx);
      break;
    case Type::LITERAL:
      if (_args[0]->isRvalue(ctx))
        rv[_index]->refLiteral().swap(_args[0]->literal(ctx));
      else
        rv[_index]->refLiteral() = _args[0]->literal(ctx);
      break;
    case Type::COMPLEX:
      if (_args[0]->isRvalue(ctx))
        rv[_index]->refComplex().swap(_args[0]->complex(ctx));
      else
        rv[_index]->refComplex() = _args[0]->complex(ctx);
      break;
    case Type::TABCHAR:
      if (_args[0]->isRvalue(ctx))
        rv[_index]->refTabchar().swap(_args[0]->tabchar(ctx));
      else
        rv[_index]->refTabchar() = _args[0]->tabchar(ctx);
      break;
    default:
      throw RuntimeError(EXC_RT_NOT_IMPLEMENTED);
    }
    return rv;
  }
  throw RuntimeError(EXC_RT_INDEX_RANGE_S, std::to_string(_index).c_str());
}

std::string MemberSETExpression::unparse(Context& ctx) const
{
  std::string sb(_exp->unparse(ctx).append(1, OPERATOR));
  sb.append(KEYWORDS[_builtin]);
  sb.append(1, ItemExpression::OPERATOR);
  sb.append(std::to_string(_index + 1));
  sb.append("(");
  for (Expression * e : _args)
    sb.append(e->unparse(ctx)).append(1, Parser::CHAIN);
  if (sb.back() == Parser::CHAIN)
    sb.back() = ')';
  else
    sb.push_back(')');
  return sb;
}

MemberSETExpression * MemberSETExpression::parse(Parser& p, Context& ctx, Expression * exp)
{
  if (exp->type(ctx) != Type::ROWTYPE)
    throw ParseError(EXC_PARSE_MEMB_NOT_IMPL_S, KEYWORDS[BTM_SET]);

  std::vector<Expression*> args;
  TokenPtr t = p.pop();

  if (t->code != ItemExpression::OPERATOR)
    throw ParseError(EXC_PARSE_BAD_MEMB_CALL_S, KEYWORDS[BTM_SET]);
  t = p.pop();
  /* item no MUST be constant */
  if (t->code != TOKEN_INTEGER)
    throw ParseError(EXC_PARSE_BAD_MEMB_CALL_S, KEYWORDS[BTM_SET]);
  unsigned item_no = (unsigned)std::stoul(t->text, 0, 10);
  if (item_no < 1 || item_no > exp->tuple_decl(ctx).size())
    throw ParseError(EXC_PARSE_OUT_OF_INDICE, std::to_string(item_no).c_str());
  t = p.pop();
  if (t->code != '(')
    throw ParseError(EXC_PARSE_BAD_MEMB_CALL_S, KEYWORDS[BTM_SET]);
  try
  {
    args.push_back(ParseExpression::expression(p, ctx));
    if (!ParseExpression::typeChecking(args.back(), exp->tuple_decl(ctx)[item_no - 1], p, ctx))
      throw ParseError(EXC_PARSE_MEMB_ARG_TYPE_S, KEYWORDS[BTM_SET]);
    assertClosedMember(p, ctx, KEYWORDS[BTM_SET]);
    return new MemberSETExpression(exp, std::move(args), item_no - 1);
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

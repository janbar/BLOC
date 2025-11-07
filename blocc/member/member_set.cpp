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
#include "blocc/expression_item.h"
#include <blocc/tuple.h>
#include <blocc/context.h>
#include <blocc/parser.h>
#include <blocc/debug.h>

#include <cstring>
#include <cassert>

namespace bloc
{

Value& MemberSETExpression::value(Context& ctx) const
{
  Value& val = _exp->value(ctx);
  if (val.isNull())
    throw RuntimeError(EXC_RT_INDEX_RANGE_S, std::to_string(_index).c_str());
  Value& a0 = _args[0]->value(ctx);

  if (val.type() == Type::ROWTYPE
          && val.type().level() == 0 && a0.type().level() == 0)
  {
    /* tuple */
    Tuple * rv = val.tuple();
    if (_index < rv->tuple_decl().size())
    {
      if (rv->tuple_decl()[_index] == a0.type())
      {
        if (a0.lvalue())
          rv->at(_index).swap(a0.clone());
        else
          rv->at(_index).swap(std::move(a0));
        return val;
      }
      /* type mixing */
      switch (rv->tuple_decl()[_index].major())
      {
      case Type::INTEGER:
        if (a0.type() == Type::NUMERIC)
        {
          rv->at(_index).swap(Value(Integer(*a0.numeric())));
          return val;
        }
        else if (a0.type() == Type::NO_TYPE)
        {
          rv->at(_index).swap(Value(Value::type_integer));
          return val;
        }
        break;
      case Type::NUMERIC:
        if (a0.type() == Type::INTEGER)
        {
          rv->at(_index).swap(Value(Numeric(*a0.integer())));
          return val;
        }
        else if (a0.type() == Type::NO_TYPE)
        {
          rv->at(_index).swap(Value(Value::type_numeric));
          return val;
        }
        break;
      default:
        if (a0.type() == Type::NO_TYPE)
        {
          rv->at(_index).swap(Value(rv->at(_index).type()));
          return val;
        }
        break;
      }
      throw RuntimeError(EXC_RT_TYPE_MISMATCH_S, rv->tuple_decl()[_index].typeName().c_str());
    }
    throw RuntimeError(EXC_RT_INDEX_RANGE_S, std::to_string(_index).c_str());
  }
  throw RuntimeError(EXC_RT_MEMB_NOT_IMPL_S, KEYWORDS[BTM_SET]);
}

std::string MemberSETExpression::unparse(Context& ctx) const
{
  std::string sb(_exp->unparse(ctx).append(1, OPERATOR));
  sb.append(KEYWORDS[_builtin]);
  sb.append(1, ItemExpression::OPERATOR);
  sb.append(std::to_string(_index + 1));
  sb.append("(");
  for (const Expression * e : _args)
    sb.append(e->unparse(ctx)).append(1, Parser::Chain);
  if (sb.back() == Parser::Chain)
    sb.back() = ')';
  else
    sb.push_back(')');
  return sb;
}

MemberSETExpression * MemberSETExpression::parse(Parser& p, Context& ctx, Expression * exp)
{
  std::vector<Expression*> args;
  TokenPtr t = p.pop();
  {
    const Symbol * s = exp->symbol();
    if (s && s->locked())
      throw ParseError(EXC_PARSE_CONST_VIOLATION_S, s->name().c_str(), t);
  }

  if (t->code != ItemExpression::OPERATOR)
    throw ParseError(EXC_PARSE_BAD_MEMB_CALL_S, KEYWORDS[BTM_SET], t);
  t = p.pop();
  /* item no MUST be constant */
  if (t->code != TOKEN_INTEGER)
    throw ParseError(EXC_PARSE_BAD_MEMB_CALL_S, KEYWORDS[BTM_SET], t);
  unsigned item_no = (unsigned)std::stoul(t->text, nullptr, 10);

  try
  {
    const Type& exp_type = exp->type(ctx);
    switch (exp_type.major())
    {
    case Type::NO_TYPE:
    case Type::ROWTYPE:
      break;
    default:
      throw ParseError(EXC_PARSE_NOT_ROWTYPE, t);
    }
    t = p.pop();
    if (t->code != '(')
      throw ParseError(EXC_PARSE_BAD_MEMB_CALL_S, KEYWORDS[BTM_SET], t);
    args.push_back(ParseExpression::expression(p, ctx));

    if (exp_type.minor() != 0) /* not opaque */
    {
      /* test item range */
      if (item_no < 1 || item_no > exp->tuple_decl(ctx).size())
        throw ParseError(EXC_PARSE_OUT_OF_INDICE, std::to_string(item_no).c_str(), t);
      /* test known types are compatible */
      if (!ParseExpression::typeChecking(args.back(), exp->tuple_decl(ctx)[item_no - 1], p, ctx))
        throw ParseError(EXC_PARSE_MEMB_ARG_TYPE_S, KEYWORDS[BTM_SET], t);
    }

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

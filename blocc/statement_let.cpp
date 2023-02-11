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

#include "statement_let.h"
#include "exception_parse.h"
#include "parse_expression.h"
#include "parser.h"
#include "context.h"
#include "expression_boolean.h"
#include "expression_integer.h"
#include "expression_numeric.h"
#include "expression_literal.h"
#include "expression_complex.h"
#include "expression_tabchar.h"
#include "expression_collection.h"
#include "expression_tuple.h"
#include "expression_item.h"
#include "debug.h"

#include <string>
#include <cassert>

namespace bloc
{

LETStatement::~LETStatement()
{
  if (_exp)
    delete _exp;
}

const Statement * LETStatement::doit(Context& ctx) const
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
        _var.store(ctx, BooleanExpression(_exp->boolean(ctx)));
        break;
      case Type::INTEGER:
        _var.store(ctx, IntegerExpression(_exp->integer(ctx)));
        break;
      case Type::NUMERIC:
        _var.store(ctx, NumericExpression(_exp->numeric(ctx)));
        break;
      case Type::COMPLEX:
      {
        if (_exp->isRvalue())
          _var.store(ctx, ComplexExpression(std::move(_exp->complex(ctx))));
        else
          _var.store(ctx, ComplexExpression(_exp->complex(ctx)));
        break;
      }
      case Type::LITERAL:
      {
        if (_exp->isRvalue())
          _var.store(ctx, LiteralExpression(std::move(_exp->literal(ctx))));
        else
          _var.store(ctx, LiteralExpression(_exp->literal(ctx)));
        break;
      }
      case Type::TABCHAR:
      {
        if (_exp->isRvalue())
          _var.store(ctx, TabcharExpression(std::move(_exp->tabchar(ctx))));
        else
          _var.store(ctx, TabcharExpression(_exp->tabchar(ctx)));
        break;
      }
      case Type::ROWTYPE:
      {
        if (_exp->isRvalue())
          _var.store(ctx, TupleExpression(std::move(_exp->tuple(ctx))));
        else
          _var.store(ctx, TupleExpression(_exp->tuple(ctx)));
        break;
      }
      }
    }
    else
    {
      /* first check for an rvalue, if not then make a copy */
      if (_exp->isRvalue())
        _var.store(ctx, CollectionExpression(std::move(_exp->collection(ctx))));
      else
        _var.store(ctx, CollectionExpression(_exp->collection(ctx)));
    }
  }
  return _next;
}

void LETStatement::unparse(Context&ctx, FILE * out) const
{
  fputs(_var.symbolName().c_str(), out);
  fputs(" = ", out);
  fputs(_exp->unparse(ctx).c_str(), out);
  unparse_next(ctx, out);
}

LETStatement * LETStatement::parse(Parser& p, Context& ctx)
{
  Expression * exp = nullptr;
  try
  {
    TokenPtr t = p.pop();
    if (t->code == TOKEN_KEYWORD)
    {
      /* check if keyword is reserved */
      if (Parser::reservedKeyword(t->text))
        throw ParseError(EXC_PARSE_RESERVED_WORD_S, t->text.c_str());
      std::string name = t->text;
      std::transform(name.begin(), name.end(), name.begin(), ::toupper);
      t = p.pop();
      if (t->code == '=' || t->code == TOKEN_ASSIGN)
      {
        exp = ParseExpression::expression(p, ctx);
        const Type& exp_type = exp->type(ctx);
        if (exp_type.major() == Type::ROWTYPE)
          /* register symbol of tuple */
          return new LETStatement(VariableExpression(ctx.registerSymbol(name, exp->tuple_decl(ctx), exp->type(ctx).level())), exp);
        else
          /* register symbol of intrinsic type */
          return new LETStatement(VariableExpression(ctx.registerSymbol(name, exp->type(ctx))), exp);
      }
    }
    throw ParseError(EXC_PARSE_NOT_A_STATEMENT);
  }
  catch (ParseError& pe)
  {
    DBG(DBG_DEBUG, "exception %p at %s line %d\n", &pe, __PRETTY_FUNCTION__, __LINE__);
    if (exp) delete exp;
    throw;
  }
  return nullptr;
}

}

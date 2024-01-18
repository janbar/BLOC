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
  /* assign variable */
  if (ctx.loadVariable(*_var.symbol()).type() != Type::POINTER)
  {
    /* ASSIGNMENT */
    _var.store(ctx, ctx, _exp);
    return _next;
  }
  else
  /* update reference */
  {
    Value * ptr = ctx.loadVariable(*_var.symbol()).value();
    /* the pointer CANNOT be null */
    if (ptr == nullptr)
      throw RuntimeError(EXC_RT_INTERNAL_ERROR_S, "Invalid pointer");
    Value& val = _exp->value(ctx); /* execute expression */
    /* values MUST be of the same type */
    if (val.type() != ptr->type())
      throw RuntimeError(EXC_RT_TYPE_MISMATCH_S, ptr->typeName().c_str());
    if (!val.lvalue())
      ptr->swap(val.to_lvalue(true));
    else if (ptr != &val)
      ptr->swap(val.clone().to_lvalue(true));
    return _next;
  }
}

void LETStatement::unparse(Context&ctx, FILE * out) const
{
  fputs(_var.symbol()->name().c_str(), out);
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

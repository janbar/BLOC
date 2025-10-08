/*
 *      Copyright (C) 2024 Jean-Luc Barriere
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

#include "builtin_input.h"
#include <blocc/parse_expression.h>
#include <blocc/exception_parse.h>
#include <blocc/context.h>
#include <blocc/parser.h>
#include <blocc/expression_literal.h>
#include <blocc/expression_variable.h>
#include <blocc/readstdin.h>
#include <blocc/debug.h>

#include <cstdlib>

namespace bloc
{

Value& INPUTExpression::value(Context & ctx) const
{
  if (_args[0]->symbol() == nullptr)
    throw RuntimeError(EXC_RT_FUNC_ARG_TYPE_S, KEYWORDS[oper]);
  Value& a0 = _args[0]->value(ctx);

  if (_args.size() > 1)
  {
    Value& a1 = _args[1]->value(ctx);
    if (!a1.isNull())
      fputs(a1.literal()->c_str(), stdout);
    fflush(stdout);
  }
  char buf[1024];
  int n = bloc_readstdin(buf, sizeof(buf));
  if (n > 0)
  {
    int l = n;
    /* discard nl */
    if (buf[l - 1] == '\n')
      --l;
    a0.swap(Value(new Literal(buf, l)).to_lvalue(a0.lvalue()));
  }
  return ctx.allocate(Value(Bool(n > 0 ? true : false)));
}

INPUTExpression * INPUTExpression::parse(Parser& p, Context& ctx)
{
  std::vector<Expression*> args;

  try
  {
    TokenPtr t = p.pop();
    if (t->code != '(')
      throw ParseError(EXC_PARSE_FUNC_ARG_NUM_S, KEYWORDS[FUNC_INPUT], t);
    args.push_back(ParseExpression::expression(p, ctx));
    VariableExpression * var = dynamic_cast<VariableExpression*>(args.back());
    if (var == nullptr || !ParseExpression::typeChecking(var, Type::LITERAL, p, ctx))
      throw ParseError(EXC_PARSE_FUNC_ARG_TYPE_S, KEYWORDS[FUNC_INPUT], t);
    if (p.front()->code == Parser::Chain)
    {
      t = p.pop();
      args.push_back(ParseExpression::expression(p, ctx));
      if (!ParseExpression::typeChecking(args.back(), Type::LITERAL, p, ctx))
        throw ParseError(EXC_PARSE_FUNC_ARG_TYPE_S, KEYWORDS[FUNC_INPUT], t);
    }
    assertClosedFunction(p, ctx, FUNC_INPUT);
    return new INPUTExpression(std::move(args));
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

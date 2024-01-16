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

#include "builtin_read.h"
#include "blocc/exception_runtime.h"
#include <blocc/parse_expression.h>
#include <blocc/exception_parse.h>
#include <blocc/context.h>
#include <blocc/parser.h>
#include <blocc/expression_variable.h>
#include <blocc/readstdin.h>
#include <blocc/debug.h>

#include <cstdlib>

namespace bloc
{

Value& READExpression::value(Context & ctx) const
{
  if (_args[0]->symbol() == nullptr)
    throw RuntimeError(EXC_RT_FUNC_ARG_TYPE_S, KEYWORDS[oper]);
  Value& a0 = _args[0]->value(ctx);

  int64_t n = 1024;
  if (_args.size() > 1)
  {
    Value& a1 = _args[1]->value(ctx);
    if (a1.isNull() || *a1.integer() < 0)
      throw RuntimeError(EXC_RT_INDEX_RANGE_S, std::to_string(n).c_str());
    n = *a1.integer();
  }

  a0.swap(Value(new Literal()));
  if (n > 32)
  {
    char * buf = new char[n];
    n = bloc_readstdin(buf, n);
    a0.literal()->assign(buf, n);
    delete [] buf;
  }
  else
  {
    char buf[32];
    n = bloc_readstdin(buf, n);
    a0.literal()->assign(buf, n);
  }
  return ctx.allocate(Value(Integer(n)));
}

READExpression * READExpression::parse(Parser& p, Context& ctx)
{
  std::vector<Expression*> args;

  try
  {
    TokenPtr t = p.pop();
    if (t->code != '(')
      throw ParseError(EXC_PARSE_FUNC_ARG_NUM_S, KEYWORDS[FUNC_READ]);
    args.push_back(ParseExpression::expression(p, ctx));
    VariableExpression * var = dynamic_cast<VariableExpression*>(args.back());
    if (var == nullptr || !ParseExpression::typeChecking(var, Type::LITERAL, p, ctx))
      throw ParseError(EXC_PARSE_FUNC_ARG_TYPE_S, KEYWORDS[FUNC_READ]);
    if (p.front()->code == Parser::CHAIN)
    {
      t = p.pop();
      args.push_back(ParseExpression::expression(p, ctx));
      if (!ParseExpression::typeChecking(args.back(), Type::INTEGER, p, ctx))
        throw ParseError(EXC_PARSE_FUNC_ARG_TYPE_S, KEYWORDS[FUNC_READ]);
    }
    assertClosedFunction(p, ctx, FUNC_READ);
    return new READExpression(std::move(args));

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

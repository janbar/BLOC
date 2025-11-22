/*
 *      Copyright (C) 2023 Jean-Luc Barriere
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

#include "expression_functor.h"
#include "parse_expression.h"
#include "exception_parse.h"
#include "parser.h"
#include "debug.h"
#include "exception_runtime.h"

#include <memory>

namespace bloc
{

FunctorExpression::~FunctorExpression()
{
  for (Expression * e : _args)
    delete e;
  _args.clear();
}

Value& FunctorExpression::value(Context& ctx) const
{
  auto env = (*_fentry)->createEnv(ctx, _args);
  (*_fentry)->body->doit(env.context());
  Value * ret = env.context().dropReturned();
  if (ret != nullptr)
  {
    Value& val = ctx.allocate(std::move(*ret));
    delete ret;
    return val;
  }
  /* do not throw EXC_RT_NO_RETURN_VALUE */
  return ctx.allocate(Value());
}

std::string FunctorExpression::unparse(Context& ctx) const
{
  std::string sb((*_fentry)->name);
  sb.append("(");
  for (const Expression * e : _args)
    sb.append(e->unparse(ctx)).append(1, Parser::Chain);
  if (sb.back() == Parser::Chain)
    sb.back() = ')';
  else
    sb.push_back(')');
  return sb;
}

FunctorExpression * FunctorExpression::parse(Parser& p, Context& ctx, TokenPtr& token)
{
  std::vector<Expression*> args;
  /* all names are declared in upper case, so now transform the keyword */
  std::string name(token->text);
  std::transform(name.begin(), name.end(), name.begin(), ::toupper);

  try
  {
    /* parse arguments list */
    TokenPtr t = p.pop();
    if (t->code != '(')
      throw ParseError(EXC_PARSE_INV_EXPRESSION, t);
    if (p.front()->code == ')')
    {
      p.pop();
    }
    else
    {
      for (;;)
      {
        args.push_back(ParseExpression::expression(p, ctx));
        t = p.pop();
        if (t->code == Parser::Chain)
          continue;
        if (t->code != ')')
          throw ParseError(EXC_PARSE_EXPRESSION_END_S, t->text.c_str(), t);
        break;
      }
    }

    FunctorManager::entry fe;
    if (ctx.functorManager().findDeclaration(name, args.size(), fe))
      return new FunctorExpression(fe, std::move(args));
    if (ctx.functorManager().exists(name))
      throw ParseError(EXC_PARSE_FUNC_ARG_NUM_S, name.c_str(), token);
    throw ParseError(EXC_PARSE_UNDEFINED_SYMBOL_S, name.c_str(), token);
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

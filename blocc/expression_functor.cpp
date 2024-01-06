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
  auto env = (*_functor)->createEnv(ctx, _args);
  (*_functor)->body->doit(env.context());
  Value * ret = env.context().dropReturned();
  if (ret == nullptr)
    throw RuntimeError(EXC_RT_NO_RETURN_VALUE);
  Value& val = ctx.allocate(std::move(*ret));
  delete ret;
  return val;
}

std::string FunctorExpression::unparse(Context& ctx) const
{
  std::string sb((*_functor)->name);
  sb.append("(");
  for (const Expression * e : _args)
    sb.append(e->unparse(ctx)).append(1, Parser::CHAIN);
  if (sb.back() == Parser::CHAIN)
    sb.back() = ')';
  else
    sb.push_back(')');
  return sb;
}

FunctorExpression * FunctorExpression::parse(Parser& p, Context& ctx, const std::string& name)
{
  std::vector<Expression*> args;

  try
  {
    /* parse arguments list */
    TokenPtr t = p.pop();
    if (t->code != '(')
      throw ParseError(EXC_PARSE_INV_EXPRESSION);
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
        if (t->code == Parser::CHAIN)
          continue;
        if (t->code != ')')
          throw ParseError(EXC_PARSE_EXPRESSION_END_S, t->text.c_str());
        break;
      }
    }

    const FunctorManager::entry& fe = FunctorManager::instance().findDeclaration(name, args.size());
    if (fe != FunctorManager::instance().npos())
      return new FunctorExpression(fe, std::move(args));
    if (FunctorManager::instance().exists(name))
      throw ParseError(EXC_PARSE_FUNC_ARG_NUM_S, name.c_str());
    throw ParseError(EXC_PARSE_UNDEFINED_SYMBOL_S, name.c_str());
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

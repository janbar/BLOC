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
#include "expression_boolean.h"
#include "expression_integer.h"
#include "expression_numeric.h"
#include "expression_literal.h"
#include "expression_complex.h"
#include "expression_tabchar.h"
#include "expression_tuple.h"
#include "expression_collection.h"

#include <memory>

namespace bloc
{

FunctorExpression::~FunctorExpression()
{
  for (Expression * e : _args)
    delete e;
  _args.clear();
}

bool FunctorExpression::boolean(Context& ctx) const
{
  auto env = (*_functor)->createEnv(ctx, _args);
  (*_functor)->body->doit(env.context());
  std::unique_ptr<Expression> ret(env.context().dropReturned());
  if (ret)
    return ret->boolean(env.context());
  throw RuntimeError(EXC_RT_NO_RETURN_VALUE);
}

int64_t FunctorExpression::integer(Context& ctx) const
{
  auto env = (*_functor)->createEnv(ctx, _args);
  (*_functor)->body->doit(env.context());
  std::unique_ptr<Expression> ret(env.context().dropReturned());
  if (ret)
    return ret->integer(env.context());
  throw RuntimeError(EXC_RT_NO_RETURN_VALUE);
}

double FunctorExpression::numeric(Context& ctx) const
{
  auto env = (*_functor)->createEnv(ctx, _args);
  (*_functor)->body->doit(env.context());
  std::unique_ptr<Expression> ret(env.context().dropReturned());
  if (ret)
    return ret->numeric(env.context());
  throw RuntimeError(EXC_RT_NO_RETURN_VALUE);
}

std::string& FunctorExpression::literal(Context& ctx) const
{
  auto env = (*_functor)->createEnv(ctx, _args);
  (*_functor)->body->doit(env.context());
  std::unique_ptr<Expression> ret(env.context().dropReturned());
  if (ret)
    return ctx.allocate(std::move(ret->literal(env.context())));
  throw RuntimeError(EXC_RT_NO_RETURN_VALUE);
}

TabChar& FunctorExpression::tabchar(Context& ctx) const
{
  auto env = (*_functor)->createEnv(ctx, _args);
  (*_functor)->body->doit(env.context());
  std::unique_ptr<Expression> ret(env.context().dropReturned());
  if (ret)
    return ctx.allocate(std::move(ret->tabchar(env.context())));
  throw RuntimeError(EXC_RT_NO_RETURN_VALUE);
}

Collection& FunctorExpression::collection(Context& ctx) const
{
  auto env = (*_functor)->createEnv(ctx, _args);
  (*_functor)->body->doit(env.context());
  std::unique_ptr<Expression> ret(env.context().dropReturned());
  if (ret)
    return ctx.allocate(std::move(ret->collection(env.context())));
  throw RuntimeError(EXC_RT_NO_RETURN_VALUE);
}

Tuple& FunctorExpression::tuple(Context& ctx) const
{
  auto env = (*_functor)->createEnv(ctx, _args);
  (*_functor)->body->doit(env.context());
  std::unique_ptr<Expression> ret(env.context().dropReturned());
  if (ret)
    return ctx.allocate(std::move(ret->tuple(env.context())));
  throw RuntimeError(EXC_RT_NO_RETURN_VALUE);
}

Complex& FunctorExpression::complex(Context& ctx) const
{
  auto env = (*_functor)->createEnv(ctx, _args);
  (*_functor)->body->doit(env.context());
  std::unique_ptr<Expression> ret(env.context().dropReturned());
  if (ret)
  {
    _tmp = ret->complex(env.context());
    return _tmp;
  }
  _tmp = Complex();
  return _tmp;
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

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

#include "functor_manager.h"
#include "context.h"
#include "statement.h"
#include "expression_variable.h"
#include "debug.h"

#include <cassert>

namespace bloc
{

Functor::~Functor()
{
  if (body)
    delete body;
  body = nullptr;
  if (ctx)
    delete ctx;
  ctx = nullptr;
  params.clear();
  name.clear();
}

unsigned FunctorManager::findDeclaration(const std::string& name, unsigned param_count)
{
  unsigned id = 0;
  for (const Entry& e : _declarations)
  {
    if (e.functor->name == name && e.functor->params.size() == param_count)
      return id;
    ++id;
  }
  return nid;
}

bool FunctorManager::nameExists(const std::string& name) const
{
  for (const Entry& e : _declarations)
  {
    if (e.functor->name == name)
      return true;
  }
  return false;
}

FunctorManager::Entry& FunctorManager::createOrReplace(const std::string& name, const std::vector<Symbol>& params)
{
  _backed.reset();
  for (Entry& e : _declarations)
  {
    if (e.functor->name == name && e.functor->params.size() == params.size())
    {
      /* back up current declaration */
      _backed.swap(e.functor);
      return e;
    }
  }
  _declarations.emplace_back(Entry(FunctorPtr(new Functor())));
  return _declarations.back();
}

void FunctorManager::rollback()
{
  if (_declarations.empty())
    return;
  if (_backed)
  {
    /* revert last change, restoring the backed up */
    if (_declarations.back().functor->name == _backed->name &&
            _declarations.back().functor->params.size() == _backed->params.size())
    {
      _declarations.back().functor.swap(_backed);
      return;
    }
  }
  else
  {
    /* remove last created */
    _declarations.pop_back();
  }
}

FunctorManager::Env FunctorManager::createEnv(Context& caller, unsigned id, const std::vector<Expression*>& pvals)
{
  uint8_t r = caller.recursion();
  if (r == RECURSION_LIMIT)
    throw RuntimeError(EXC_RT_RECURSION_LIMIT);

  Entry& entry = getDeclaration(id);

  Context * _ctx;
  if (entry.ctx_cache.empty())
  {
    _ctx = entry.functor->createChildRuntime(r + 1);
    _ctx->trace(caller.trace());
  }
  else
  {
    _ctx = entry.ctx_cache.front();
    entry.ctx_cache.pop_front();
    _ctx->recursion(r + 1);
    _ctx->trace(caller.trace());
    _ctx->returnCondition(false);
  }

  assert(entry.functor->params.size() == pvals.size());

  /* bind parameter values ​​to variables for all symbols */
  unsigned i = 0;
  for (const Symbol& symbol : entry.functor->params)
    VariableExpression(symbol).store(*_ctx, caller, pvals[i++]);

  return Env(entry, _ctx);
}

}

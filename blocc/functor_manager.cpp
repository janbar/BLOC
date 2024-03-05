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

namespace bloc
{

Functor::~Functor()
{
  while (!ctx_cache.empty())
  {
    delete ctx_cache.front();
    ctx_cache.pop_front();
  }
  if (body)
    delete body;
  body = nullptr;
  if (ctx)
    delete ctx;
  ctx = nullptr;
  params.clear();
  name.clear();
}

Functor::Env Functor::createEnv(Context& parent, const std::vector<Expression*>& pvals) const
{
  uint8_t r = parent.recursion();
  if (r == RECURSION_LIMIT)
    throw RuntimeError(EXC_RT_RECURSION_LIMIT);

  Context * _ctx;
  if (ctx_cache.empty())
    _ctx = new Context(*(ctx), r + 1, parent.trace());
  else
  {
    _ctx = ctx_cache.front();
    ctx_cache.pop_front();
    _ctx->recursion(r + 1);
    _ctx->trace(parent.trace());
    _ctx->returnCondition(false);
  }
  /* assign args with the given values */
  for (int i = 0; i < params.size(); ++i)
    VariableExpression(params[i]).store(*_ctx, parent, pvals[i]);

  return Env(*this, _ctx);
}

const FunctorManager::entry FunctorManager::findDeclaration(const std::string& name, unsigned param_count)
{
  for (entry e = _declarations.begin(); e != _declarations.end(); ++e)
  {
    if ((*e)->name == name && (*e)->params.size() == param_count)
      return e;
  }
  return _declarations.end();
}

bool FunctorManager::exists(const std::string& name) const
{
  for (const FunctorPtr& fp : _declarations)
  {
    if (fp->name == name)
      return true;
  }
  return false;
}

std::vector<FunctorPtr> FunctorManager::reportDeclarations() const
{
  std::vector<FunctorPtr> v;
  v.assign(_declarations.begin(), _declarations.end());
  return v;
}

FunctorManager::entry FunctorManager::createOrReplace(const std::string& name, const std::vector<Symbol>& params)
{
  _backed.reset();
  for (entry e = _declarations.begin(); e != _declarations.end(); ++e)
  {
    if ((*e)->name == name && (*e)->params.size() == params.size())
    {
      /* backup current declaration */
      _backed.swap((*e));
      return e;
    }
  }
  _declarations.push_front(FunctorPtr(new Functor(name, params)));
  return _declarations.begin();
}

void FunctorManager::rollback()
{
  if (_backed)
  {
    /* revert last change, restoring the backed up */
    for (FunctorPtr& fp : _declarations)
    {
      if (fp->name == _backed->name && fp->params.size() == _backed->params.size())
      {
        fp.swap(_backed);
        return;
      }
    }
  }
  else
  {
    /* remove last created */
    _declarations.pop_front();
  }
}

}

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

#ifndef FUNCTOR_MANAGER_H_
#define FUNCTOR_MANAGER_H_

#include "expression.h"

#include <string>
#include <memory>
#include <vector>
#include <forward_list>

#define RECURSION_LIMIT 0xff

namespace bloc
{

class Statement;
class Context;

struct Functor
{
  std::string name;                   /* functor name */
  std::vector<Symbol> params;         /* ordered list of parameter */
  Type returns;                       /* Type returned */
  Context * ctx = nullptr;            /* required parse context */
  const Statement * body = nullptr;   /* statement (begin ... end) */

  Functor() { };
  explicit Functor(const std::string& name, const std::vector<Symbol>& params)
  : name(name), params(params) { }
  ~Functor();

  /* A runtime context is cached after use. It will be reused for the
   * next call, thus avoiding a new cloning of the prestine context.
   * The env factory reuses cached contexts or creates a new one.
   * The env destructor returns its context to the cache.
   */
  mutable std::forward_list<Context*> ctx_cache;

  class Env
  {
    friend struct Functor;
  public:
    Context& context() { return *ctx; }
    ~Env() { if (ctx) functor.ctx_cache.push_front(ctx); }
    Env(Env&& e) noexcept : functor(e.functor), ctx(e.ctx) { e.ctx = nullptr; }

    Env(const Env& e) = delete;
    Env& operator=(const Env& e) = delete;

  private:
    Env(const Functor& functor, Context * ctx) : functor(functor), ctx(ctx) { }
    const Functor& functor;
    Context * ctx;
  };

  /**
   * Env/Context factory for body runtime.
   * Note that size of `pvals` must be equal to the size of `params`.
   * The recursion depth will be incremented in the returned Env/Context.
   * @param caller the context in which the call is made
   * @param pvals the list of expressions passed as parameters
   * @return the `Env` for body runtime
   */
  Env createEnv(Context& caller, const std::vector<Expression*>& pvals) const;
};

typedef std::shared_ptr<Functor> FunctorPtr;

class FunctorManager
{

public:
  FunctorManager() = default;
  ~FunctorManager() = default;

  typedef std::forward_list<FunctorPtr> container;
  typedef container::iterator entry;

  /**
   * Find entry for the given declaration.
   * @param name The name of the declaration
   * @param param_count The parameters count of the declaration
   * @param found_entry out parameter to receive the found entry
   * @return true if entry is found, else false
   */
  bool findDeclaration(const std::string& name, unsigned param_count, entry& found_entry);

  /**
   * Returns true if a declaration with the given name exists.
   * @param name The name of the declaration
   * @return true if the given name exists, else false
   */
  bool exists(const std::string& name) const;

  container reportDeclarations() const;

  entry createOrReplace(const std::string& name, const std::vector<Symbol>& params);

  void rollback();

private:
  container _declarations;
  container::value_type _backed;
};

}

#endif /* FUNCTOR_MANAGER_H_ */

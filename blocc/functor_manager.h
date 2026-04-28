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
#include "context.h"

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

  Functor() { }
  ~Functor();
};

typedef std::shared_ptr<Functor> FunctorPtr;

class FunctorManager
{

public:

  static constexpr unsigned nid = (unsigned)(-1);

  FunctorManager() = default;
  ~FunctorManager() = default;

  explicit FunctorManager(const FunctorManager& fm)
  {
    // don't copy the cache of context
    for (const Entry& e : fm._declarations)
      _declarations.emplace_back(Entry(e.functor));
  }

  FunctorManager& operator=(const FunctorManager& fm)
  {
    // don't copy the cache of context
    for (const Entry& e : fm._declarations)
      _declarations.emplace_back(Entry(e.functor));
    return *this;
  }

  struct Entry
  {
    /* the shareable functor */
    FunctorPtr functor;
    /* the context cache */
    std::forward_list<Context*> ctx_cache;

    explicit Entry(const FunctorPtr& _functor) : functor(_functor) { }

    explicit Entry(const Entry&) = delete;
    Entry& operator =(const Entry&) = delete;

    ~Entry() { clearCache(); }

    Entry(Entry&& e) noexcept
    {
      functor.swap(e.functor);
      ctx_cache.swap(e.ctx_cache);
    }

    void clearCache()
    {
      for (Context*c : ctx_cache)
        delete c;
      ctx_cache.clear();
    }
  };

  /**
   * Find id for the given declaration.
   * @param name The name of the declaration
   * @param param_count The parameters count of the declaration
   * @return the found id, else nid
   */
  unsigned findDeclaration(const std::string& name, unsigned param_count);

  /**
   * Returns true if a declaration with the given name exists.
   * @param name The name of the declaration
   * @return true if the given name exists, else false
   */
  bool nameExists(const std::string& name) const;

  typedef std::vector<Entry> container;

  const container& declarations() const
  {
    return _declarations;
  }

  Entry& createOrReplace(const std::string& name, const std::vector<Symbol>& params);

  void rollback();

  Entry& getDeclaration(unsigned id)
  {
    return _declarations[id];
  }

  /* A runtime context is cached after use. It will be reused for the
   * next call, thus avoiding a new cloning of the prestine context.
   * The env factory reuses cached contexts or creates a new one.
   * The env destructor returns its context to the cache.
   */
  class Env
  {
    FunctorManager::Entry& _entry;
    Context * _ctx;

  public:

    Env(FunctorManager::Entry& entry, Context * ctx)
    : _entry(entry), _ctx(ctx) { }

    Env(const Env& e) = delete;
    Env& operator=(const Env& e) = delete;

    ~Env()
    {
      // recycle the context
      if (_ctx)
        _entry.ctx_cache.push_front(_ctx);
    }

    Env(Env&& e) noexcept : _entry(e._entry), _ctx(e._ctx) { e._ctx = nullptr; }

    Functor& functor() { return *_entry.functor; }
    Context& context() { return *_ctx; }
  };

  /**
   * Context factory for body runtime.
   * The size of `pvals` must be equal to the size of functor `params`.
   * The recursion depth will be incremented in the returned Env/Context.
   * @param caller the context in which the call is made
   * @param id the entry id
   * @param pvals the list of expressions passed as parameters
   * @return the `Env` for body runtime
   */
  Env createEnv(Context& caller, unsigned id, const std::vector<Expression*>& pvals);

  void setRoot(Context * ctx) { _root = ctx; }
  Context * getRoot() { return _root; }

  Context * createChildContext(Context& ctx)
  {
    return ctx.createChild(this);
  }

private:
  Context * _root = nullptr;
  container _declarations;
  FunctorPtr _backed;
};

}

#endif /* FUNCTOR_MANAGER_H_ */

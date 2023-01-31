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

#ifndef CONTEXT_H_
#define CONTEXT_H_

#include "symbol.h"
#include "template_stack.h"
#include "template_temporary.h"
#include "expression_static.h"

#include <string>
#include <forward_list>
#include <map>
#include <algorithm>
#include <chrono>

namespace bloc
{

class Statement;

class Context
{

public:
  virtual ~Context();
  Context();
  explicit Context(int fd_out, int fd_err);

  /**
   * Purge the context including all symbols and storage pool.
   * Any executables previously built with the context will no longer work.
   */
  void purge();

  /**************************************************************************/
  /* Symbol and pointer                                                     */
  /**************************************************************************/

  Symbol * findSymbol(const std::string& name)
  {
    auto it = _symbols.find(name);
    return (it != _symbols.end() ? it->second : nullptr);
  }

  /**
   * Register symbol of intrinsic type
   */
  Symbol& registerSymbol(const std::string& name, const Type& type);

  /**
   * Register symbol of tuple type
   */
  Symbol& registerSymbol(const std::string& name, const Tuple::Decl& decl, Type::TypeLevel level);

  StaticExpression * loadVariable(const std::string& symbolName)
  {
    const Symbol * symbol = findSymbol(symbolName);
    if (symbol)
      return loadVariable(*symbol);
    return nullptr;
  }

  StaticExpression * loadVariable(const Symbol& symbol)
  {
    if (symbol.id < _storage.size())
      return _storage[symbol.id];
    return nullptr;
  }

  StaticExpression& storeVariable(const Symbol& symbol, StaticExpression&& e);

  void describeSymbol(const std::string& name);
  void describeSymbol(const std::pair<std::string, Symbol*>& entry);

  void dumpVariables();

  /**************************************************************************/
  /* Control and stack                                                      */
  /**************************************************************************/

  void stackControl(const Statement * s)
  {
    _controlstack.stack(s);
  }

  const Statement * topControl()
  {
    return _controlstack.empty() ? nullptr : _controlstack.top();
  }

  void unstackControl()
  {
    _controlstack.unstack();
  }

  size_t execLevel() const
  {
    return _execstack.size();
  }

  const Statement * execStatement() const
  {
    return _execstack.top();
  }

  void execBegin(const Statement * s)
  {
    _execstack.stack(s);
  }

  void execEnd()
  {
    _execstack.unstack();
  }

  bool stopCondition() const
  {
    return (_breakCondition || _continueCondition || _returnCondition);
  }

  void breakCondition(bool b)
  {
    _breakCondition = b;
  }

  bool breakCondition() const
  {
    return _breakCondition;
  }

  void continueCondition(bool b)
  {
    _continueCondition = b;
  }

  bool continueCondition() const
  {
    return _continueCondition;
  }

  void returnCondition(bool b)
  {
    _returnCondition = b;
  }

  bool returnCondition() const
  {
    return _returnCondition;
  }

  void saveReturned(Expression * ret);
  Expression * dropReturned();

  /**************************************************************************/
  /* Events                                                                 */
  /**************************************************************************/

  void onStatementEnd(const Statement * s)
  {
    purgeWorkingMemory();
  }

  void onRuntimeError();

  /**************************************************************************/
  /* Trace                                                                  */
  /**************************************************************************/

  double timestamp() const
  {
    auto ts = std::chrono::system_clock::now();
    std::chrono::duration<double> diff = ts - _ts_init;
    return diff.count();
  }

  double elapsed(double from) const
  {
    auto ts = std::chrono::system_clock::now();
    std::chrono::duration<double> diff = ts - _ts_init;
    return (diff.count() - from);
  }

  void trace(bool b)
  {
    _trace = b;
  }

  bool trace() const
  {
    return _trace;
  }

  /**************************************************************************/
  /* Temporary management                                                   */
  /**************************************************************************/

  std::string& allocate(std::string&& v)
  {
    Temporary<std::string> * tmp = new Temporary<std::string>(std::move(v));
    _temporary_storage.push_back(tmp);
    return tmp->handle;
  }

  TabChar& allocate(TabChar&& v)
  {
    Temporary<TabChar> * tmp = new Temporary<TabChar>(std::move(v));
    _temporary_storage.push_back(tmp);
    return tmp->handle;
  }

  Collection& allocate(Collection&& v)
  {
    Temporary<Collection> * tmp = new Temporary<Collection>(std::move(v));
    _temporary_storage.push_back(tmp);
    return tmp->handle;
  }

  Tuple& allocate(Tuple&& v)
  {
    Temporary<Tuple> * tmp = new Temporary<Tuple>(std::move(v));
    _temporary_storage.push_back(tmp);
    return tmp->handle;
  }

  void statWorkingMemory(size_t * count, size_t * size) const;

  /**
   * Purge temporary storage allocated by a standalone expression or statement.
   * Unlike running Executable where purging is performed after completion or
   * failure, a manual purge of working allocations is required:
   *
   * Processing an Expression:
   * It is obvious that the working memory is needed as int64_t as the payload has
   * not been processed. Also the memory is not freed on exception. So in any
   * case we have to perform the purge, either after the payload has been
   * processed, or catching exception RuntimeError.
   * Examples:
   *   try {
   *     Expression * e = Parser.parseExpression();
   *     result = e->literal(ctx);
   *     delete e;
   *     ctx.purgeWorkingMemory();  --> run the purge after copying result
   *   } catch (RuntimeError& re) {
   *     ctx.purgeWorkingMemory();  --> run the purge on exception
   *     ...
   *   }
   *
   * Processing a Statement:
   * Upon successful completion, purging is performed automatically. It is only
   * in case of failure that we have to manually execute the purge.
   * Examples:
   *   try {
   *     Statement * s = Parser.parseStatement();
   *     Statement * s_nxt = s;
   *     while (s_nxt) {
   *       s_nxt = s_nxt->doit(ctx);
   *       ...
   *     }
   *   } catch (RuntimeError& re) {
   *     ctx.purgeWorkingMemory();  --> run the purge on exception
   *     ...
   *   }
   */
  void purgeWorkingMemory()
  {
    for (auto tmp : _temporary_storage)
      delete tmp;
    _temporary_storage.clear();
  }

  /**************************************************************************/
  /* Environment                                                            */
  /**************************************************************************/

  FILE * ctxout() const { return _sout; }
  FILE * ctxerr() const { return _serr; }

  void error(const RuntimeError& error) { _last_error = error; }
  const RuntimeError& error() { return _last_error; }

  static const char * version();

  static const char * versionHeader();

  static int compatible();

  static const char * country();

  static const char * language();

  static double random(double max);

private:
  std::chrono::system_clock::time_point _ts_init = std::chrono::system_clock::now();

  /* memory pool and references */
  std::map<std::string, Symbol*> _symbols;
  std::vector<StaticExpression*> _storage;

  /* stack of looping statement */
  Stack<const Statement*> _controlstack;

  /* stack of block statement */
  Stack<const Statement*> _execstack;

  /* temporary pool */
  std::vector<TemporaryBase*> _temporary_storage;

  bool _trace = false;
  bool _breakCondition = false;
  bool _continueCondition = false;
  bool _returnCondition = false;
  Expression * _returned = nullptr;

  FILE * _sout = nullptr; // stream for output
  FILE * _serr = nullptr; // stream for errors

  RuntimeError _last_error;
};

}

#endif /* CONTEXT_H_ */

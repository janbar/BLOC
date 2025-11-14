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
#include "exception_runtime.h"
#include "value.h"

#include <string>
#include <forward_list>
#include <map>
#include <algorithm>
#include <chrono>

namespace bloc
{

class Statement;
class FunctorManager;

class Context
{

public:
  virtual ~Context();
  Context();
  Context& operator=(const Context&) = delete;
  Context(int fd_out, int fd_err);

  /**
   * Purge the context including all symbols and storage pool.
   * Any executables previously built with the context will no longer work.
   */
  void purge();

  /*========================================================================*/
  /* Symbol and pointer                                                     */
  /*========================================================================*/

  /**
   * Returns the pointer to the symbol with the given name, or null.
   * Note that the symbol name must be in UPPERCASE.
   * @param name        the name
   * @return            the pointer to the found symbol
   */
  Symbol * findSymbol(const std::string& name)
  {
    for (MemorySlot& e : _storage_pool)
    {
      if (e.symbol->name() == name)
        return e.symbol;
    }
    return nullptr;
  }

  Symbol * getSymbol(unsigned id)
  {
    return _storage_pool[id].symbol;
  }

  void parsingBegin();

  void parsingEnd();

  bool parsing() const
  {
    return _parsing;
  }

  /**
   * Register symbol of intrinsic type
   */
  Symbol& registerSymbol(const std::string& name, const Type& type);

  /**
   * Register symbol of tuple type
   */
  Symbol& registerSymbol(const std::string& name, const TupleDecl::Decl& decl, Type::TypeLevel level);

  /**
   * Returns the pointer to the value bound to the given name, or null.
   * Note that the symbol name must be in UPPERCASE.
   * @param name        the name
   * @return            the pointer to the value
   */
  Value * loadVariable(const std::string& symbolName)
  {
    const Symbol * symbol = findSymbol(symbolName);
    if (symbol)
      return &loadVariable(*symbol);
    return nullptr;
  }

  /**
   * Returns the value bound to the given symbol.
   * @param symbol      the symbol
   * @return            the value
   */
  Value& loadVariable(const Symbol& symbol)
  {
    return _storage_pool[symbol.id()].value;
  }

  /**
   * Clear the value bound to the given symbol.
   * @param symbol      the symbol
   */
  void clearVariable(const Symbol& symbol)
  {
    _storage_pool[symbol.id()].value = Value();
  }

  /**
   * Bind the given value to the given symbol.
   * Note that values are swapped, therefore the passed value will contain the
   * payload of the old bound value.
   * @param symbol      the symbol
   * @param value       the value
   * @return            the bound value
   */
  Value& storeVariable(const Symbol& symbol, Value&& e);

  void describeSymbol(const std::string& name);
  void describeSymbol(const Symbol& symbol);

  void dumpVariables();


  /*========================================================================*/
  /* Functors                                                               */
  /*========================================================================*/

  /**
   * Returns the root manager of this
   */
  FunctorManager& functorManager();

  /**
   * Create a child context from this
   */
  Context * createChild();

  void dumpFunctors();

  /*========================================================================*/
  /* Control and stack                                                      */
  /*========================================================================*/

  void stackControl(const Statement * s) { _controlstack.stack(s); }

  const Statement * topControl()
  {
    return _controlstack.empty() ? nullptr : _controlstack.top();
  }

  void unstackControl() { _controlstack.unstack(); }

  size_t execLevel() const { return _execstack.size(); }

  const Statement * execStatement() const { return _execstack.top(); }

  void execBegin(const Statement * s) { _execstack.stack(s); }

  void execEnd() { _execstack.unstack(); }

  bool stopCondition() const
  {
    return (_breakCondition || _continueCondition || _returnCondition
            || _root->_returnCondition); /* handle program exit */
  }

  void breakCondition(bool b) { _breakCondition = b; }

  bool breakCondition() const { return _breakCondition; }

  void continueCondition(bool b) { _continueCondition = b; }

  bool continueCondition() const { return _continueCondition; }

  void returnCondition(bool b) { _returnCondition = b; }

  bool returnCondition() const { return _returnCondition; }

  void saveReturned(Value& ret);

  /**
   * Returns a pointer to returned value by the last RUN. The returned pointer
   * must be freed by the caller.
   * @return            a value, or null
   */
  Value * dropReturned();

  /*========================================================================*/
  /* Events                                                                 */
  /*========================================================================*/

  void onStatementEnd(const Statement * s) { _temporary_storage.clear(); }

  void onRuntimeError();

  /*========================================================================*/
  /* Trace                                                                  */
  /*========================================================================*/

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

  void trace(bool b) { _trace = b; }

  bool trace() const { return _trace; }

  void recursion(uint8_t r) { _recursion = r; }

  uint8_t recursion() const { return _recursion; }

  /*========================================================================*/
  /* Temporary storage management                                           */
  /*========================================================================*/

  Value& allocate(Value&& v) { return _temporary_storage.keep(std::move(v)); }

  size_t allocationCount() const { return _temporary_storage.count(); }

  /**
   * Purge temporary storage allocated by a standalone expression or statement.
   * Unlike running Executable where purging is performed after completion or
   * failure, a manual purge of working allocations is required:
   *
   * Processing an Expression:
   * It is obvious that the working memory is needed as long as the payload has
   * not been processed. Also the memory is not freed on exception. So in any
   * case we have to perform the purge, either after the payload has been
   * processed, or catching exception RuntimeError.
   * Examples:
   *   try {
   *     Expression * e = Parser.parseExpression();
   *     result = e->value(ctx).clone();
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
  void purgeWorkingMemory() { _temporary_storage.purge(); }

  /*========================================================================*/
  /* Environment                                                            */
  /*========================================================================*/

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

  /**
   * Set context as trusted to allow use of restricted plugins
   * @param b enable or disable use of restricted plugins
   */
  void trusted(bool b);

  bool trusted() { return (_flags & FLAG_TRUSTED) != 0; }

private:
  Context * _root;
  FunctorManager * _fctm = nullptr;
  std::chrono::system_clock::time_point _ts_init;

  /* memory slots */
  struct MemorySlot
  {
    Symbol * symbol;
    Value value;
    ~MemorySlot() { delete symbol; }
    explicit MemorySlot(const Symbol& s) : symbol(new Symbol(s)), value(s) { }
    explicit MemorySlot(Symbol&& s) : symbol(new Symbol(std::move(s))), value(s) { }
    MemorySlot(const MemorySlot& m) = delete;
    MemorySlot& operator=(MemorySlot& m) = delete;
    MemorySlot(MemorySlot&& m) noexcept
    : symbol(m.symbol), value(std::move(m.value)) { m.symbol = nullptr; }
    MemorySlot& operator=(MemorySlot&& m) noexcept
    {
      if (this == &m)
        return *this;
      symbol = m.symbol;
      m.symbol = nullptr;
      value = std::move(m.value);
      return *this;
    }
  };

  std::vector<MemorySlot> _storage_pool;

  /* stack of looping statement */
  Stack<const Statement*> _controlstack;

  /* stack of block statement */
  Stack<const Statement*> _execstack;

  /* temporary pool */
  class Pool
  {
    unsigned wm = 0;
    std::vector<Value*> pool;
  public:
    Pool() { }
    Pool(const Pool&) = delete;
    Pool& operator=(const Pool&) = delete;
    ~Pool()
    {
      for (Value * v : pool)
        delete v;
    }
    Value& keep(Value&& v)
    {
      if (wm < pool.size())
        pool[wm]->swap(std::move(v));
      else
        pool.push_back(new Value(std::move(v)));
      return *pool[wm++];
    }
    void clear()
    {
      wm = 0;
    }
    void purge()
    {
      wm = 0;
      for (Value * v : pool)
        v->swap(Value());
    }
    size_t count() const { return wm; }
    size_t reserved() const { return pool.size(); }
  };

  Pool _temporary_storage;

  std::vector<Symbol> _backed_symbols;

  RuntimeError _last_error;

  bool _parsing = false;
  bool _breakCondition = false;
  bool _continueCondition = false;
  bool _returnCondition = false;

  Value * _returned = nullptr;

  FILE * _sout = nullptr; // stream for output
  FILE * _serr = nullptr; // stream for errors

  bool _trace = false;

  enum Flag { FLAG_TRUSTED = 0x01 };
  uint8_t _flags = 0;

  uint8_t _recursion = 0;
  friend class Functor;
  explicit Context(const Context& ctx);
  Context(const Context& ctx, uint8_t recursion);
};

}

#endif /* CONTEXT_H_ */

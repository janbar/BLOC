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

#include "context.h"
#include "statement.h"
#include "functor_manager.h"
#include "plugin_manager.h"
#include "exception_parse.h"
#include "collection.h"
#include "tuple.h"
#include "debug.h"

#include <cstdio>
#include <cstring>
#include <random>
#include <cassert>

#if defined(LIBBLOC_MSWIN)
#include <process.h>  /* for getpid */
#include <io.h>       /* for dup */
#define STDOUT_FILENO _fileno(stdout)
#else
#include <unistd.h>
#endif

#define COMPATIBLE  0x020800
#ifndef LIBVERSION
#define LIBVERSION  "[undefined]"
#endif
#define LIBHEADER   LIBTAG " version " LIBVERSION " compiled on " __DATE__ " at " __TIME__

namespace bloc
{

Context::Context()
: _root(this)
, _ts_init(std::chrono::system_clock::now())
{
  int d = ::dup(STDOUT_FILENO);
  if (d >= 0)
  {
    _sout = ::fdopen(d, "w");
    _serr = _sout;
  }
}

Context::Context(int fd_out, int fd_err)
: _root(this)
, _ts_init(std::chrono::system_clock::now())
{
  int d = ::dup(fd_out);
  if (d >= 0)
    _sout = ::fdopen(d, "w");
  if (fd_err == fd_out)
    _serr = _sout;
  else
  {
    d = ::dup(fd_err);
    if (d >= 0)
      _serr = ::fdopen(d, "w");
  }
}

Context::~Context()
{
  purge();

  /* only the root context own file descriptors,
   * therefore a child should not close any of them */
  if (_root == this)
  {
    if (_serr && _serr != _sout)
      ::fclose(_serr);
    if (_sout)
      ::fclose(_sout);
  }
}

Context * Context::clone()
{
  Context * other = new Context(::fileno(_sout), ::fileno(_serr));
  /* copy flags */
  other->_flags = _flags;
  /* clone table of symbols */
  other->_storage_pool.reserve(_storage_pool.size());
  for (const MemorySlot& e : _storage_pool)
    other->_storage_pool.push_back(e);
  /* clone functor manager */
  other->_fctm = new FunctorManager(*_fctm);
  other->_fctm->setRoot(other);
  return other;
}

Context * Context::clone(int fd_out, int fd_err)
{
  Context * other = new Context(fd_out, fd_err);
  /* copy flags */
  other->_flags = _flags;
  /* clone table of symbols */
  other->_storage_pool.reserve(_storage_pool.size());
  for (const MemorySlot& e : _storage_pool)
    other->_storage_pool.push_back(e);
  /* clone functor manager */
  other->_fctm = new FunctorManager(*_fctm);
  other->_fctm->setRoot(other);
  return other;
}

void Context::purge()
{
  returnCondition(false);
  if (_returned)
    delete _returned;
  _returned = nullptr;

  if (_fctm && _fctm->getRoot() == this)
    delete _fctm;
  _fctm = nullptr;

  /* clear temporary pool */
  _temporary_storage.purge();

  /* clear storage pool */
  _storage_pool.clear();

  /* reset trace mode */
  _trace = false;

  /* reset parsing state */
  _parsing = false;
  _backed_symbols.clear();
}

/**************************************************************************/
/* Symbol and pointer                                                     */
/**************************************************************************/

void Context::parsingBegin()
{
  _parsing = true;
}

void Context::parsingEnd()
{
  /* restore all upgraded symbols during parsing */
  std::vector<Symbol>::const_iterator it = _backed_symbols.cend();
  while (_backed_symbols.begin() != it)
  {
    --it;
    if (it->major() == Type::ROWTYPE)
      _storage_pool[it->id()].symbol->upgrade(it->tuple_decl(), it->level());
    else
      _storage_pool[it->id()].symbol->upgrade(*it);
  }
  _backed_symbols.clear();
  _parsing = false;
}

Symbol& Context::registerSymbol(const std::string& name, const Type& type)
{
  Symbol * s = findSymbol(name);
  if (s == nullptr)
  {
    /* allocate new */
    unsigned nxt_id = _storage_pool.size();
    _storage_pool.push_back(MemorySlot(Symbol(nxt_id, name, type)));
    Symbol * sym = _storage_pool.back().symbol;

    /* implement safety qualifier */
    if (name.front() == Symbol::SAFETY_QUALIFIER)
      sym->safety(true);

    return *sym;
  }
  if (s->locked())
    throw ParseError(EXC_PARSE_CONST_VIOLATION_S, s->name().c_str());
  if (type == *s)
    return *s;
  if (s->safety())
  {
    switch (s->check_safety(type))
    {
    case Symbol::SAFE_KO:
      if (s->major() == Type::ROWTYPE)
        throw ParseError(EXC_PARSE_TYPE_MISMATCH_S, s->tuple_decl().tupleName().c_str());
      throw ParseError(EXC_PARSE_TYPE_MISMATCH_S, s->typeName().c_str());
    case Symbol::SAFE_EQU:
      return *s;
    case Symbol::SAFE_UPG:
      break;
    }
  }
  /* back up old symbol */
  _backed_symbols.push_back(*s);
  s->upgrade(type);
  return *s;
}

Symbol& Context::registerSymbol(const std::string& name, const TupleDecl::Decl& decl, Type::TypeLevel level)
{
  Symbol * s = findSymbol(name);
  if (s == nullptr)
  {
    /* allocate new */
    unsigned nxt_id = _storage_pool.size();
    _storage_pool.push_back(MemorySlot(Symbol(nxt_id, name, decl, level)));
    Symbol * sym = _storage_pool.back().symbol;

    /* implement safety qualifier */
    if (name.front() == Symbol::SAFETY_QUALIFIER)
      sym->safety(true);

    return *sym;
  }
  if (s->locked())
    throw ParseError(EXC_PARSE_CONST_VIOLATION_S, s->name().c_str());
  Type type = decl.make_type(level);
  if (type == *s)
    return *s;
  if (s->safety())
  {
    switch (s->check_safety(type))
    {
    case Symbol::SAFE_KO:
      if (s->major() == Type::ROWTYPE)
        throw ParseError(EXC_PARSE_TYPE_MISMATCH_S, s->tuple_decl().tupleName().c_str());
      throw ParseError(EXC_PARSE_TYPE_MISMATCH_S, s->typeName().c_str());
    case Symbol::SAFE_EQU:
      return *s;
    case Symbol::SAFE_UPG:
      break;
    }
  }
  /* back up old symbol */
  _backed_symbols.push_back(*s);
  s->upgrade(decl, level);
  return *s;
}

Value& Context::storeVariable(unsigned id, Value&& e)
{
  MemorySlot& slot = _storage_pool[id];
  if (slot.symbol->locked())
    throw RuntimeError(EXC_RT_CONST_VIOLATION_S, slot.symbol->name().c_str());
  if (slot.value.type() == e.type())
  {
    /* move new as lvalue */
    if (!e.lvalue())
      slot.value.swap(std::move(e.to_lvalue(true)));
    /* if not the same instance then copy */
    else if (&e != &slot.value)
      slot.value.swap(e.clone().to_lvalue(true));
  }
  else
  {
    /* safety flag forbids any change of qualified type */
    if (slot.symbol->safety())
    {
      if (slot.symbol->check_safety(e.type()) == Symbol::SAFE_KO)
        throw RuntimeError(EXC_RT_TYPE_MISMATCH_S, slot.value.typeName().c_str());
    }
    /* upgrade the symbol registered in the context for next parsing */
    if (e.type() != Type::ROWTYPE || e.isNull())
      slot.symbol->upgrade(e.type());
    else
    {
      /* ROWTYPE not null */
      if (e.type().level() > 0)
        slot.symbol->upgrade(e.collection()->table_decl(), e.type().level());
      else
        slot.symbol->upgrade(e.tuple()->tuple_decl(), e.type().level());
    }
    /* move new as lvalue */
    if (e.lvalue())
      slot.value.swap(e.clone().to_lvalue(true));
    else
      slot.value.swap(std::move(e.to_lvalue(true)));
  }
  /* forward safety of symbol */
  return slot.value;
}

void Context::describeSymbol(const std::string& name)
{
  std::string _name(name);
  std::transform(_name.begin(), _name.end(), _name.begin(), ::toupper);
  Symbol * s = findSymbol(_name);
  if (s != nullptr)
    describeSymbol(s->id());
  else if (_sout)
  {
    fprintf(_sout, "%s is undefined.\n", _name.c_str());
    fflush(_sout);
  }
}

void Context::describeSymbol(unsigned id)
{
  if (!_sout)
    return;
  Value& var = loadVariable(id);

  fprintf(_sout, "%04x: %s is ", id, getSymbol(id).name().c_str());
  if (var.type() == Type::NO_TYPE)
    fprintf(_sout, "%s\n", getSymbol(id).typeName().c_str());
  else
  {
    fputs(var.toString().c_str(), _sout);
    fputc('\n', _sout);
    if (!var.isNull() && var.type().level() == 0)
    {
      /* print 1 line of content */
      switch (var.type().major())
      {
      case Type::LITERAL:
      {
        char buf[80];
        // coverity[fun_call_w_exception]
        std::string tmp = var.literal()->substr(0, sizeof (buf));
        snprintf(buf, sizeof (buf) - 4, "%s",
                 Value::readableLiteral(tmp).c_str());
        fputs(buf, _sout);
        if (buf[strlen(buf) - 1] != '"')
          fputs("...", _sout);
        fputc('\n', _sout);
        break;
      }
      case Type::TABCHAR:
        // coverity[fun_call_w_exception]
        Value::outputTabchar(*var.tabchar(), _sout, 1);
        break;
      default:
        break;
      }
    }
  }
  fflush(_sout);
}

void Context::dumpVariables()
{
  for (MemorySlot& e : _storage_pool)
    describeSymbol(e.symbol->id());
}

FunctorManager& Context::functorManager()
{
  /* only a root context owns the functor manager */
  if (_fctm == nullptr)
  {
    _fctm = new FunctorManager();
    _fctm->setRoot(this);
  }
  return *_fctm;
}

void Context::dumpFunctors()
{
  if (!_fctm || !_sout)
    return;
  for (const FunctorManager::Entry& e : _fctm->declarations())
  {
    fprintf(_sout, "%s (", e.functor->name.c_str());
    bool n = false;
    for (const bloc::Symbol& p : e.functor->params)
    {
      if (n)
        fputc(',', _sout);
      fputs(p.name().c_str(), _sout);
      /* type declaration */
      if (p.level() > 0)
        fputs(":table", _sout);
      else if (p.major() == Type::COMPLEX && p.minor() != 0)
      {
        fputs(":", _sout);
        fputs(PluginManager::instance().plugged(p.minor()).interface.name, _sout);
      }
      else if (p.major() != Type::NO_TYPE)
      {
        fputs(":", _sout);
        fputs(p.typeName().c_str(), _sout);
      }
      n = true;
    }
    fputs(") returns ", _sout);
    if (e.functor->returns.level() > 0)
      fputs("table", _sout);
    else
    {
      if (e.functor->returns.major() == Type::COMPLEX && e.functor->returns.minor() != 0)
        fputs(PluginManager::instance().plugged(e.functor->returns.minor()).interface.name, _sout);
      else
        fputs(e.functor->returns.typeName().c_str(), _sout);
    }
    fputc('\n', _sout);
    fflush(_sout);
  }
}

/**************************************************************************/
/* Control and stack                                                      */
/**************************************************************************/

void Context::unstackControl()
{
  const Control& c = _controlstack.top();
  c.stmt->finalizeControl(*this, c.data);
  _controlstack.unstack();
}

void Context::saveReturned(Value& ret)
{
  if (_returned)
    delete _returned;
  if (ret.lvalue())
    _returned = new Value(ret.clone());
  else
  {
    _returned = new Value();
    _returned->swap(ret);
  }
}

Value * Context::dropReturned()
{
  Value * ret = _returned;
  _returned = nullptr;
  return ret;
}

/**************************************************************************/
/* Events                                                                 */
/**************************************************************************/

void Context::onRuntimeError()
{
  /* purge control stack */
  while (!_controlstack.empty() && _controlstack.top().stmt->level() >= execLevel())
    unstackControl();
  /* purge temporary allocations */
  purgeWorkingMemory();
}

/**************************************************************************/
/* Environment                                                            */
/**************************************************************************/

const char * Context::version()
{
  return LIBVERSION;
}

const char * Context::versionHeader()
{
  return LIBHEADER;
}

int Context::compatible()
{
  return COMPATIBLE;
}

const char * Context::country()
{
  static const char * COUNTRY = "US";
  return COUNTRY;
}

const char * Context::language()
{
  static const char * LANGUAGE = "en";
  return LANGUAGE;
}

double Context::random(double max)
{
  static std::minstd_rand r;
  static bool seeded = false;
  if (!seeded)
  {
    seeded = true;
    r.seed((unsigned) ::getpid());
    (void)r();
  }
  return (double)r() / std::minstd_rand::max() * max;
}

void Context::trusted(bool b)
{
  if (b)
    _flags |= FLAG_TRUSTED;
  else
    _flags &= ~(FLAG_TRUSTED);
}

/**
 * Make an empty shell of context.
 * @param ctx the parent context
 */
Context::Context(const Context& ctx)
: _root(ctx._root)
, _ts_init(ctx._ts_init)
, _sout(ctx._sout)
, _serr(ctx._serr)
, _flags(ctx._flags)
{
}

/**
 * Make a child context from this.
 * @param fm          functor manager
 */
Context * Context::createChild(FunctorManager * fm) const
{
  Context * child = new Context(*this);
  child->_fctm = fm;
  return child;
}

/**
 * Make a runtime context from this with the given recursion level.
 * The table of symbol is duplicated with empty values.
 * @param fm          functor manager
 * @param recursion   level of recursion (>=1)
 */
Context * Context::createChildRuntime(FunctorManager * fm, uint8_t recursion) const
{
  assert(recursion > 0);
  Context * runtime = new Context(*this);
  runtime->_fctm = fm;
  runtime->_recursion = recursion;
  /* copy table of symbols with new empty values */
  runtime->_storage_pool.reserve(_storage_pool.size());
  for (const MemorySlot& e : _storage_pool)
    runtime->_storage_pool.push_back(MemorySlot(*(e.symbol)));
  return runtime;
}

}

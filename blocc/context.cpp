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

#ifndef LIBVERSION
#define LIBVERSION  "[undefined]"
#endif

#include "debug.h"
#define LIBHEADER   LIBTAG " version " LIBVERSION " compiled on " __DATE__ " at " __TIME__

#include "context.h"
#include "statement.h"
#include "exception_parse.h"
#include "collection.h"
#include "tuple.h"

#include <cstdio>
#include <cstring>
#include <random>
#include <cassert>

#if (defined(_WIN32) || defined(_WIN64))
#include <process.h>  /* for getpid */
#include <io.h>       /* for dup */
#define STDOUT_FILENO _fileno(stdout)
#else
#include <unistd.h>
#endif

namespace bloc
{

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
  return 5;
}

Context::Context()
{
  _sout = ::fdopen(::dup(STDOUT_FILENO), "w");
  _serr = _sout;
}

Context::Context(int fd_out, int fd_err)
{
  _sout = ::fdopen(::dup(fd_out), "w");
  if (fd_err == fd_out)
    _serr = _sout;
  else
    _serr = ::fdopen(::dup(fd_err), "w");
}

Context::~Context()
{
  purge();
  if (_serr != _sout)
    ::fclose(_serr);
  ::fclose(_sout);
}

Context::Context(const Context& ctx, uint8_t recursion, bool trace)
: _trace(trace), _recursion(recursion)
{
  /* duplicate file descriptors */
  _sout = ::fdopen(::dup(::fileno(ctx._sout)), "w");
  if (ctx._serr == ctx._sout)
    _serr = _sout;
  else
    _serr = ::fdopen(::dup(::fileno(ctx._serr)), "w");
  /* copy table of symbols with new empty values */
  _storage_pool.reserve(ctx._storage_pool.size());
  for (const MemorySlot& e : ctx._storage_pool)
    _storage_pool.push_back(MemorySlot(*(e.symbol)));
}

void Context::purge()
{
  returnCondition(false);
  if (_returned)
    delete _returned;
  _returned = nullptr;
  /* clear temporary pool */
  _temporary_storage.purge();
  /* clear storage pool */
  _storage_pool.clear();
  /* reset trace mode */
  _trace = false;
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
  while (_backed_symbols.begin() != it--)
  {
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
  if (*s == type)
    return *s;
  if (s->safety())
  {
    if (s->major() == Type::ROWTYPE)
      throw ParseError(EXC_PARSE_TYPE_MISMATCH_S, s->tuple_decl().tupleName().c_str());
    throw ParseError(EXC_PARSE_TYPE_MISMATCH_S, s->typeName().c_str());
  }
  /* stacking old symbol */
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
  if (*s == decl.make_type(level))
    return *s;
  if (s->safety())
    throw ParseError(EXC_PARSE_TYPE_MISMATCH_S, s->tuple_decl().tupleName().c_str());
  /* stacking old symbol */
  _backed_symbols.push_back(*s);
  s->upgrade(decl, level);
  return *s;
}

Value& Context::storeVariable(const Symbol& symbol, Value&& e)
{
  MemorySlot& slot = _storage_pool[symbol.id()];
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
    /* safety flag forbids any change of type */
    if (slot.value.safety())
      throw RuntimeError(EXC_RT_TYPE_MISMATCH_S, slot.value.typeName().c_str());
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
  return slot.value.to_safety(slot.symbol->safety());
}

void Context::describeSymbol(const std::string& name)
{
  std::string _name(name);
  std::transform(_name.begin(), _name.end(), _name.begin(), ::toupper);
  Symbol * s = findSymbol(_name);
  if (s != nullptr)
    describeSymbol(*s);
  else
  {
    fprintf(_sout, "%s is undefined.\n", _name.c_str());
    fflush(_sout);
  }
}

void Context::describeSymbol(const Symbol& symbol)
{
  Value& var = loadVariable(symbol);

  fprintf(_sout, "[%04x] %s is ", symbol.id(), symbol.name().c_str());
  if (var.type() == Type::NO_TYPE)
    fprintf(_sout, "%s\n", symbol.typeName().c_str());
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
    describeSymbol(*e.symbol);
}

/**************************************************************************/
/* Control and stack                                                      */
/**************************************************************************/

void Context::saveReturned(Value& ret)
{
  if (_returned)
    delete _returned;
  if (ret.lvalue())
    _returned = new Value(ret.clone());
  else
    _returned = new Value(std::move(ret));
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
  while (!_controlstack.empty() && _controlstack.top()->level() >= execLevel())
    _controlstack.unstack();
  /* purge temporary allocations */
  purgeWorkingMemory();
}

/**************************************************************************/
/* Environment                                                            */
/**************************************************************************/

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

}

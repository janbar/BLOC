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
#include "expression_literal.h"
#include "expression_tabchar.h"

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
  return 2;
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
  /* copy table of symbols */
  _symbols.reserve(ctx._symbols.size());
  for (auto e : ctx._symbols)
    _symbols.push_back(new Symbol(*e));
  _storage.insert(_storage.begin(), ctx._storage.size(), nullptr);
}

void Context::purge()
{
  returnCondition(false);
  if (_returned)
    delete _returned;
  _returned = nullptr;
  /* clear variables allocation */
  for (auto e : _storage)
    if (e != nullptr) delete e;
  _storage.clear();
  /* clear symbol map */
  for (auto e : _symbols)
    delete e;
  _symbols.clear();
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
      _symbols[it->id]->upgrade(it->tuple_decl(), it->level());
    else
      _symbols[it->id]->upgrade(*it);
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
    unsigned nxt_id = _symbols.size();
    _storage.push_back(nullptr);
    _symbols.push_back(new Symbol(nxt_id, name, type));
    Symbol& sym = *_symbols.back();

    /* implement safety qualifier */
    if (name.front() == Symbol::SAFETY_QUALIFIER)
      sym.safety(true);

    return sym;
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

Symbol& Context::registerSymbol(const std::string& name, const Tuple::Decl& decl, Type::TypeLevel level)
{
  Symbol * s = findSymbol(name);
  if (s == nullptr)
  {
    /* allocate new */
    unsigned nxt_id = _symbols.size();
    _storage.push_back(nullptr);
    _symbols.push_back(new Symbol(nxt_id, name, decl, level));
    Symbol& sym = *_symbols.back();

    /* implement safety qualifier */
    if (name.front() == Symbol::SAFETY_QUALIFIER)
      sym.safety(true);

    return sym;
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

StaticExpression& Context::storeVariable(const Symbol& symbol, StaticExpression&& e)
{
  std::vector<StaticExpression*>::iterator it = _storage.begin() + symbol.id;
  if (*it == nullptr)
  {
    /* store new and forward the safety flag */
    *it = e.swapNew();
    (*it)->safety(symbol.safety());
    /* upgrade the symbol registered in this context for this name */
    const Type& new_type = (*it)->refType();
    if (new_type == Type::ROWTYPE)
      getSymbol(symbol.id)->upgrade((*it)->tuple_decl(*this), new_type.level());
    else
      getSymbol(symbol.id)->upgrade(new_type);
  }
  else if ((*it)->refType() != e.refType())
  {
    /* safety flag forbids any change of type */
    if ((*it)->safety())
      throw RuntimeError(EXC_RT_TYPE_MISMATCH_S, (*it)->typeName(*this).c_str());
    /* delete old, and store new */
    delete *it;
    *it = e.swapNew();
    /* upgrade the symbol registered in this context for this name */
    const Type& new_type = (*it)->refType();
    if (new_type == Type::ROWTYPE)
      getSymbol(symbol.id)->upgrade((*it)->tuple_decl(*this), new_type.level());
    else
      getSymbol(symbol.id)->upgrade(new_type);
  }
  else
  {
    /* swap */
    (*it)->_swap(e);
  }
  return **it;
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
  StaticExpression * var = loadVariable(symbol);

  fprintf(_sout, "[%04x] %s is ", symbol.id, symbol.name.c_str());
  if (!var)
    fprintf(_sout, "%s\n", symbol.typeName().c_str());
  else
  {
    fputs(var->toString(*this).c_str(), _sout);
    fputc('\n', _sout);
    if (var->refType().level() == 0)
    {
      /* print 1 line of content */
      switch (var->refType().major())
      {
      case Type::LITERAL:
      {
        char buf[80];
        snprintf(buf, sizeof (buf) - 4, "%s",
                 LiteralExpression::readableLiteral(var->refLiteral().substr(0, sizeof (buf))).c_str());
        fputs(buf, _sout);
        if (buf[strlen(buf) - 1] != '"')
          fputs("...", _sout);
        fputc('\n', _sout);
        break;
      }
      case Type::TABCHAR:
        TabcharExpression::outputTabchar(var->refTabchar(), _sout, 1);
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
  for (const auto e : _symbols)
    describeSymbol(*e);
}

/**************************************************************************/
/* Control and stack                                                      */
/**************************************************************************/

void Context::saveReturned(Expression* ret)
{
  if (_returned)
    delete _returned;
  _returned = ret;
}

Expression* Context::dropReturned()
{
  Expression * r = _returned;
  _returned = nullptr;
  return r;
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

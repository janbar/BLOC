/*
 *      Copyright (C) 2024 Jean-Luc Barriere
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

#include "plugin_sys.h"
#include <blocc/collection.h>
#include <blocc/debug.h>
#include <cstdlib>
#include <cassert>
#include <cstring>
#include <cstdio>
#include <list>
#include <chrono>
#include <thread>

#include <signal.h>

/*
 * Create the module SYS
 */
PLUGINCREATOR(SYSPlugin)

namespace bloc
{
namespace plugin
{
namespace sys
{

/*
 * The module interface
 */

enum Method
{
  Exec0     = 0,
  Exec1     = 1,
  CmdStatus = 2,
  Setenv    = 3,
  Sleep     = 4,
};

/**********************************************************************/
/*  Method arguments                                                  */
/*  mode:         type: decl,ndim                                     */
/**********************************************************************/
static PLUGIN_ARG exec0_args[]  = {
  { PLUGIN_IN,    { "L", 0 } }, // command
};

static PLUGIN_ARG exec1_args[]  = {
  { PLUGIN_IN,    { "L", 0 } }, // command
  { PLUGIN_INOUT, { "X", 0 } }, // output
  { PLUGIN_IN,    { "I", 0 } }, // maxsize
};

static PLUGIN_ARG setenv_args[]  = {
  { PLUGIN_IN,    { "L", 0 } }, // name
  { PLUGIN_IN,    { "L", 0 } }, // value
};

static PLUGIN_ARG sleep_args[]  = {
  { PLUGIN_IN,    { "I", 0 } }, // duration ms
};

/**********************************************************************/
/*  Methods list                                                      */
/*  id:       name:         ret: decl,ndim  args_count,args:          */
/**********************************************************************/
static PLUGIN_METHOD methods[] =
{
  { Exec0,        "exec",             { "B", 0 },     1, exec0_args,
          "Execute a system command or a block of commands." },
  { Exec1,        "exec",             { "B", 0 },     3, exec1_args,
          "Execute a system command or a block of commands, and tail the output into"
          "\nthe specified variable as a byte array with the given max size."
          "\nNote that max size will be tuncated to 2GB."},
  { CmdStatus,    "status",           { "I", 0 },     0, nullptr,
          "Read the return status of the last system command." },
  { Setenv,       "setenv",           { "L", 0 },     2, setenv_args,
          "Sets the environment variable to the given value and returns the old value."
          "\nPassing a null value will unset the variable." },
  { Sleep,        "sleep",            { "I", 0 },     1, sleep_args,
          "Delay for a specified amount of time in seconds." },
};

/**
 * The state of file handle
 */
struct Handle
{
  int _status = 0;
  ~Handle() { }
  Handle() { }
  bool exec(const std::string& cmd);
  bool execout(const std::string& cmd, bloc::TabChar& out, size_t maxsize);
  const char * getvar(const std::string& name);
  void setvar(const std::string& name, const std::string& value);
  void unsetvar(const std::string& name);
  void delayms(unsigned millisec);
};

} /* namespace sys */

void SYSPlugin::declareInterface(PLUGIN_INTERFACE * interface)
{
  interface->name = "sys";
  interface->method_count = sizeof(sys::methods) / sizeof(PLUGIN_METHOD);
  interface->methods = sys::methods;
  interface->ctors_count = 0;
  interface->ctors = nullptr;
}

void * SYSPlugin::createObject(int ctor_id, bloc::Context& ctx, const std::vector<bloc::Expression*>& args)
{
  sys::Handle * h = nullptr;
  try
  {
    h = new sys::Handle();
  }
  catch (...)
  {
    if (h)
      delete h;
    throw;
  }
  return h;
}

void SYSPlugin::destroyObject(void * object)
{
  sys::Handle * h = static_cast<sys::Handle*>(object);
  delete h;
}

bloc::Value * SYSPlugin::executeMethod(
          bloc::Complex& object_this,
          int method_id,
          bloc::Context& ctx,
          const std::vector<bloc::Expression*>& args
          )
{
  sys::Handle * h = static_cast<sys::Handle*>(object_this.instance());
  switch (method_id)
  {
  case sys::Exec0:
  {
    bloc::Value& a0 = args[0]->value(ctx);
    if (a0.isNull())
      return new bloc::Value(bloc::Value::type_boolean);
    return new bloc::Value(bloc::Bool(h->exec(*a0.literal())));
  }

  case sys::Exec1:
  {
    bloc::Value& a0 = args[0]->value(ctx);
    bloc::Value& a1 = args[1]->value(ctx);
    bloc::Value& a2 = args[2]->value(ctx);
    if (a0.isNull() || a2.isNull())
      return new bloc::Value(bloc::Value::type_boolean);
    if (!args[1]->isVarName())
      throw RuntimeError(EXC_RT_OTHER_S, "Invalid arguments.");
    bloc::Value out(new bloc::TabChar());
    bool success = bloc::Bool(h->execout(*a0.literal(), *out.tabchar(), *a2.integer()));
    ctx.storeVariable(*args[1]->symbol(), std::move(out));
    return new bloc::Value(success);
  }

  case sys::CmdStatus:
  {
    return new bloc::Value(bloc::Integer(h->_status));
  }

  case sys::Setenv:
  {
    bloc::Value& a0 = args[0]->value(ctx);
    bloc::Value& a1 = args[1]->value(ctx);
    if (a0.isNull())
      return new bloc::Value(bloc::Value::type_literal);
    bloc::Value old;
    const char * buf = h->getvar(*a0.literal());
    if (buf != nullptr)
      old.swap(bloc::Value(new bloc::Literal(buf)));
    if (a1.isNull())
      h->unsetvar(*a0.literal());
    else
      h->setvar(*a0.literal(), *a1.literal());
    return new bloc::Value(std::move(old));
  }

  case sys::Sleep:
  {
    bloc::Value& a0 = args[0]->value(ctx);
    if (!a0.isNull())
    {
      double cur = ctx.timestamp();
      double brk = cur + *a0.integer();
      int tick = 0;
      while (!ctx.stopCondition() && cur < brk)
      {
        // yield for 500 ms
        h->delayms(500);
        // resync the clock every 1 minutes
        if (++tick == 120)
        {
          // resync the clock
          cur = ctx.timestamp();
          tick = 0;
        }
        else
        {
          cur += 0.5;
        }
      }
      return new bloc::Value(bloc::Integer(brk - cur));
    }
    return new bloc::Value(bloc::Value::type_integer);
  }

  default:
    break;
  }
  return nullptr;
}

bool sys::Handle::exec(const std::string& cmd)
{
#if defined(LIBBLOC_MSWIN)
#else
  void (*_handler)(int) = signal(SIGCHLD, SIG_DFL);
#endif

  // always prefer to use standard call: i.e system(),
  // it implements posix_spawn() on unix as possible,
  // it works on windows, and provides the same signature
  _status = ::system(cmd.c_str());

#if defined(LIBBLOC_MSWIN)
#else
  if (_handler != SIG_ERR)
    signal(SIGCHLD, _handler);
#endif
  return (_status == 0);
}

#define CHUNKLEN    4096
#define MAXOUTLEN   INT32_MAX

bool sys::Handle::execout(const std::string& cmd, bloc::TabChar& out, size_t maxsize)
{
  std::list<char*> buf;
  /* cache the total size of the buffer */
  size_t buflen = 0;
  /* nb bytes to discard from head of the buffer */
  size_t head = 0;
  /* nb bytes to discard from end of the buffer */
  size_t tail = 0;

  /* notes:
   * the dried size of data is: buflen - head - tail.
   * head size will never grow above two CHUNK: at each loop the chunk in front
   * is removed if necessary so that the head size remains less than CHUNKLEN.
   * tail cannot grow above CHUNKLEN */

  _status = 0; /* reset command status */

#if defined(LIBBLOC_MSWIN)
  FILE * pout = _popen(cmd.c_str(), "r");
#else
  void (*_handler)(int) = signal(SIGCHLD, SIG_DFL);
  FILE * pout = ::popen(cmd.c_str(), "r");
#endif
  if (pout)
  {
    /* a recycle chunk */
    char * bin = nullptr;
    /* allocate the read buffer */
    char * tmp = new char[CHUNKLEN];

    /* buflen should not overflow, so the output must be truncated to
     * a reasonable size */
    if (maxsize > MAXOUTLEN)
      maxsize = MAXOUTLEN;

    /* initialize the buffer with 1 chunk */
    buf.push_back(new char[CHUNKLEN]);
    buflen = CHUNKLEN;
    tail = CHUNKLEN;

    while (!::feof(pout))
    {
      size_t n = ::fread(tmp, 1, CHUNKLEN, pout);
      if ( n > 0)
      {
        if (tail >= n)
        {
          /* the tail of last chunk is large enough to hold the read data */
          ::memcpy(buf.back() + CHUNKLEN - tail, tmp, n);
          tail -= n;
        }
        else
        {
          /* first, fill the tail of last chunk */
          if (tail)
          {
            ::memcpy(buf.back() + CHUNKLEN - tail, tmp, tail);
            n -= tail;
          }
          /* reuse the bin, or allocate a new chunk for the rest (n) */
          if (!bin)
            buf.push_back(new char[CHUNKLEN]);
          else
          {
            buf.push_back(bin);
            bin = nullptr;
          }
          buflen += CHUNKLEN;
          /* fill the new chunk with the rest (n) */
          ::memcpy(buf.back(), tmp + tail, n);
          /* and reset tail of the last chunk */
          tail = CHUNKLEN - n;

          /* set head position */
          if (buflen > (maxsize + tail))
          {
            // coverity[overflow_const]
            head = buflen - (maxsize + tail);
            /* detach the head of buffer which may be unnecessary */
            if (head >= CHUNKLEN)
            {
              /* reuse head chunk as bin */
              bin = buf.front();
              buf.pop_front();
              buflen -= CHUNKLEN;
              head -= CHUNKLEN;
            }
          }
        }
      }
      else
      {
        int e = ::ferror(pout);
        if (e)
        {
          bloc::DBG(DBG_ERROR, "broken pipe (%d)\n", e);
          _status = e;
          break;
        }
      }
    }
    delete [] tmp;
    if (bin)
      delete [] bin;
    tmp = nullptr;
    bin = nullptr;

#if defined(LIBBLOC_MSWIN)
    int r = _pclose(pout);
#else
    int r = pclose(pout);
    if (_handler != SIG_ERR)
      signal(SIGCHLD, _handler);
#endif
    if (r && _status == 0)
      _status = r; /* push error */

    /* fill output buffer */
    out.clear();
    out.reserve(buflen - head - tail);
    /* if len is greater than CHUNKLEN, then buffer contains linked chunks */
    if (buflen > CHUNKLEN)
    {
      /* copy the first chunk from position at head */
      out.insert(out.end(), buf.front() + head, buf.front() + CHUNKLEN);
      delete [] buf.front();
      buf.pop_front();
      buflen -= CHUNKLEN;
      while (buflen > CHUNKLEN)
      {
        /* copy next chunk */
        out.insert(out.end(), buf.front(), buf.front() + CHUNKLEN);
        delete [] buf.front();
        buf.pop_front();
        buflen -= CHUNKLEN;
      }
      /* copy the last chunk until tail front */
      out.insert(out.end(), buf.front(), buf.front() + CHUNKLEN - tail);
      delete [] buf.front();
      buf.pop_front();
      buflen -= CHUNKLEN;
    }
    else
    {
      /* copy the chunk from position at head and until tail front */
      out.insert(out.end(), buf.front() + head, buf.front() + CHUNKLEN - tail);
      delete [] buf.front();
      buf.pop_front();
    }
    assert(buf.size() == 0);
    return (_status == 0);
  }
  _status = -1;
  return false;
}

const char * sys::Handle::getvar(const std::string& name)
{
  return ::getenv(name.c_str());
}

void sys::Handle::setvar(const std::string& name, const std::string& value)
{
  if (name.length() == 0)
    return;
#if defined(LIBBLOC_MSWIN)
  std::string buf;
  buf.append(name).append("=").append(value);
  _putenv(buf.c_str());
#else
  ::setenv(name.c_str(), value.c_str(), 1);
#endif
}

void sys::Handle::unsetvar(const std::string& name)
{
  if (name.length() == 0)
    return;
#if defined(LIBBLOC_MSWIN)
  std::string buf;
  buf.append(name).append("=");
  _putenv(buf.c_str());
#else
  ::unsetenv(name.c_str());
#endif
}

void sys::Handle::delayms(unsigned millisec)
{
  std::this_thread::sleep_for(std::chrono::milliseconds(millisec));
}

} /* namespace plugin */
} /* namespace bloc */

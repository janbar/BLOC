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

/*
 * Create the module CSVImport
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
  Setenv    = 2,
  Sleep     = 3,
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
          "\nthe specified variable as a byte array with the given max size." },
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
  sys::Handle * sys = nullptr;
  try
  {
    sys = new sys::Handle();
  }
  catch (...)
  {
    if (sys)
      delete sys;
    throw;
  }
  return sys;
}

void SYSPlugin::destroyObject(void * object)
{
  sys::Handle * sys = static_cast<sys::Handle*>(object);
  delete sys;
}

bloc::Value * SYSPlugin::executeMethod(
          bloc::Complex& object_this,
          int method_id,
          bloc::Context& ctx,
          const std::vector<bloc::Expression*>& args
          )
{
  sys::Handle * sys = static_cast<sys::Handle*>(object_this.instance());
  switch (method_id)
  {
  case sys::Exec0:
  {
    bloc::Value& a0 = args[0]->value(ctx);
    if (a0.isNull())
      return new bloc::Value(bloc::Value::type_integer);
    return new bloc::Value(bloc::Bool(sys->exec(*a0.literal())));
  }

  case sys::Exec1:
  {
    bloc::Value& a0 = args[0]->value(ctx);
    bloc::Value& a1 = args[1]->value(ctx);
    bloc::Value& a2 = args[2]->value(ctx);
    if (a0.isNull() || a2.isNull())
      return new bloc::Value(bloc::Value::type_integer);
    if (args[1]->symbol() == nullptr)
      throw RuntimeError(EXC_RT_OTHER_S, "Invalid arguments.");
    bloc::Value out(new bloc::TabChar());
    bool success = bloc::Bool(sys->execout(*a0.literal(), *out.tabchar(), *a2.integer()));
    ctx.storeVariable(*args[1]->symbol(), std::move(out));
    return new bloc::Value(success);
  }

  case sys::Setenv:
  {
    bloc::Value& a0 = args[0]->value(ctx);
    bloc::Value& a1 = args[1]->value(ctx);
    if (a0.isNull())
      return new bloc::Value(bloc::Value::type_literal);
    bloc::Value old;
    const char * buf = sys->getvar(*a0.literal());
    if (buf != nullptr)
      old.swap(bloc::Value(new bloc::Literal(buf)));
    if (a1.isNull())
      sys->unsetvar(*a0.literal());
    else
      sys->setvar(*a0.literal(), *a1.literal());
    return new bloc::Value(std::move(old));
  }

  case sys::Sleep:
  {
    bloc::Value& a0 = args[0]->value(ctx);
    if (!a0.isNull())
    {
      unsigned tenth = 0;
      uint32_t rest = (uint32_t)(*a0.integer());
      while (!ctx.stopCondition() && rest != 0)
      {
        sys->delayms(100);
        if (++tenth >= 10)
        {
          rest -= 1;
          tenth = 0;
        }
      }
      return new bloc::Value(bloc::Integer(rest));
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
  // always prefer to use standard call: i.e system(),
  // it implements posix_spawn() on unix as possible,
  // it works on windows, and provides the same signature
  int r = ::system(cmd.c_str());
  return (r == 0);
}

bool sys::Handle::execout(const std::string& cmd, bloc::TabChar& out, size_t maxsize)
{
  static const size_t chunk = 4096;
  int err = 0;
  std::list<char*> buf;
  buf.push_back(new char[chunk]);
  size_t len = chunk;   /* total allocated size of the buffer */
  size_t head = 0;      /* head cut */
  size_t tail = chunk;  /* tail cut */

#if (defined(_WIN32) || defined(_WIN64))
  FILE * pout = _popen(cmd.c_str(), "r");
#else
  FILE * pout = ::popen(cmd.c_str(), "r");
#endif
  if (pout)
  {
    char * bin = nullptr;
    char * tmp = new char[chunk];
    while (!::feof(pout))
    {
      size_t n = ::fread(tmp, 1, sizeof(chunk), pout);
      if ( n > 0)
      {
        /* free space */
        if (tail >= n)
        {
          ::memcpy(buf.back() + chunk - tail, tmp, n);
          tail -= n;
        }
        else
        {
          if (tail)
          {
            ::memcpy(buf.back() + chunk - tail, tmp, tail);
            n -= tail;
          }
          if (!bin)
            buf.push_back(new char[chunk]);
          else
          {
            /* reuse the chunk in bin */
            buf.push_back(bin);
            bin = nullptr;
          }
          len += chunk;
          ::memcpy(buf.back(), tmp + tail, n);
          tail = chunk - n;
        }

       /* free space */
        size_t sz = len - head - tail;
        if (sz > maxsize)
        {
          head += sz - maxsize;
          while (head >= chunk)
          {
            if (bin)
              delete [] bin;
            bin = buf.front();
            buf.pop_front();
            len -= chunk;
            head -= chunk;
          }
        }
        /* end free space */
      }
      else
      {
        int e = ::ferror(pout);
        if (e)
        {
          bloc::DBG(DBG_ERROR, "broken pipe (%d)\n", e);
          err = e;
          break;
        }
      }
    }
    delete [] tmp;
    if (bin)
      delete [] bin;
    tmp = nullptr;
    bin = nullptr;

#if (defined(_WIN32) || defined(_WIN64))
    int r = _pclose(pout);
#else
    int r = pclose(pout);
#endif
    if (!err && r)
      err = r; /* push error */

    /* fill output buffer */
    out.clear();
    out.reserve(len - head - tail);
    /* if len is greater than chunk, then buffer contains linked chunks */
    if (len > chunk)
    {
      /* insert the first chunk from position at head */
      out.insert(out.end(), buf.front() + head, buf.front() + chunk);
      delete [] buf.front();
      buf.pop_front();
      len -= chunk;
      while (len > chunk)
      {
        /* insert next chunk */
        out.insert(out.end(), buf.front(), buf.front() + chunk);
        delete [] buf.front();
        buf.pop_front();
        len -= chunk;
      }
      /* insert the last chunk until tail */
      out.insert(out.end(), buf.front(), buf.front() + chunk - tail);
      delete [] buf.front();
      buf.pop_front();
      len -= chunk;
    }
    else
    {
      /* insert the chunk from position at head and until tail */
      out.insert(out.end(), buf.front() + head, buf.front() + chunk - tail);
      delete [] buf.front();
      buf.pop_front();
    }
    assert(buf.size() == 0);
  }
  return (err == 0);
}

const char * sys::Handle::getvar(const std::string& name)
{
  return ::getenv(name.c_str());
}

void sys::Handle::setvar(const std::string& name, const std::string& value)
{
  if (name.length() == 0)
    return;
#if (defined(_WIN32) || defined(_WIN64))
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
#if (defined(_WIN32) || defined(_WIN64))
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

} /* namespace import */
} /* namespace bloc */

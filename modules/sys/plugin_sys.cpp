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
#include "spawn_process.h"

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
#if defined(LIBBLOC_UNIX)
extern "C" { extern char **environ; }
#endif

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
  Exec2,
  CmdStatus,
  Setenv,
  Sleep,
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

static PLUGIN_ARG exec2_args[]  = {
  { PLUGIN_IN,    { "L", 0 } }, // command
  { PLUGIN_IN,    { "L", 0 } }, // input
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
          "\nNote that output size will be tuncated to 2GB."
          "\n1  : command"
          "\n2  : variable to store output"
          "\n3  : tail max size" },
  { Exec2,        "exec",             { "B", 0 },     4, exec2_args,
          "Execute a system command, send string as input, and tail the output into"
          "\nthe specified variable as a byte array with the given max size."
          "\nNote that output size will be tuncated to 2GB."
          "\n1  : command"
          "\n2  : string to send as input"
          "\n3  : variable to store output"
          "\n4  : tail max size" },
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
  bool exec(const std::string& cmd, FILE* out);
  bool execinout(const std::string& cmd, const char * input,
                 bloc::TabChar& out, size_t maxsize);
  const char * getvar(const std::string& name);
  void setvar(const std::string& name, const std::string& value);
  void unsetvar(const std::string& name);
  void delayms(unsigned millisec);
};

class ExecBuffer
{
  const size_t CHUNKLEN = 4096;
  size_t _outsize;
  size_t _bufsize   = 0;  ///< cache the total size of the buffer
  size_t _head      = 0;  ///< nb bytes to discard from head of the buffer
  size_t _tail      = 0;  ///< nb bytes to discard from end of the buffer
  char * _spare     = nullptr;  ///< chunk for spare
  std::list<char*> _chunks;     ///< data chunks
  const char * _inputdata = nullptr;  ///< data for read
public:
  ExecBuffer(size_t outsize, const char * inputdata)
  {
    /* buflen should not overflow, so the output must be truncated to
     * a reasonable size */
    _outsize = (outsize > INT32_MAX ? INT32_MAX : outsize);
    /* initialize the buffer with 1 chunk */
    _chunks.push_back(new char[CHUNKLEN]);
    _bufsize = CHUNKLEN;
    _tail = CHUNKLEN;
    /* set input for read */
    _inputdata = inputdata;
  }
  ~ExecBuffer()
  {
    for (char* e : _chunks) { if (e) delete [] e; }
    if (_spare) delete [] _spare;
  }
  int write(const char * data, int len);
  int read(char * c);
  void result(bloc::TabChar& out);
};

static int writeout(void * out, const char * data, int len);
static int writebuf(void * hdl, const char * data, int len);
static int readchar(void * hdl, char * c);

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
    return new bloc::Value(bloc::Bool(h->exec(*a0.literal(), ctx.ctxout())));
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
    bool success = bloc::Bool(h->execinout(*a0.literal(), nullptr, *out.tabchar(), *a2.integer()));
    ctx.storeVariable(*args[1]->symbol(), std::move(out));
    return new bloc::Value(success);
  }

  case sys::Exec2:
  {
    bloc::Value& a0 = args[0]->value(ctx);
    bloc::Value& a1 = args[1]->value(ctx);
    bloc::Value& a2 = args[2]->value(ctx);
    bloc::Value& a3 = args[3]->value(ctx);
    if (a0.isNull() || a3.isNull())
      return new bloc::Value(bloc::Value::type_boolean);
    if (!args[2]->isVarName())
      throw RuntimeError(EXC_RT_OTHER_S, "Invalid arguments.");
    const char * input = (a1.isNull() ? nullptr : a1.literal()->c_str());
    bloc::Value out(new bloc::TabChar());
    bool success = bloc::Bool(h->execinout(*a0.literal(), input, *out.tabchar(), *a3.integer()));
    ctx.storeVariable(*args[2]->symbol(), std::move(out));
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

int sys::writeout(void* out, const char* data, int len)
{
  FILE* file = static_cast<FILE*>(out);
  ::fwrite(data, 1, len, file);
  ::fflush(file);
  return len;
}

bool sys::Handle::exec(const std::string& cmd, FILE* out)
{
  std::vector<char*> argv;
  int exitcode = 0;
  int ret;

#if defined(LIBBLOC_MSWIN)
  argv.push_back(strncpy(new char[8], "CMD.EXE", 8));
  argv.push_back(strncpy(new char[3], "/C", 3));
#else
  void (*_handler)(int) = signal(SIGCHLD, SIG_DFL);
  argv.push_back(strncpy(new char[3], "sh", 3));
  argv.push_back(strncpy(new char[3], "-c", 3));
#endif
  argv.push_back(strncpy(new char[cmd.size()+1], cmd.c_str(), cmd.size()+1));
  argv.push_back(nullptr);

  ret = spawn_process(argv.data(), environ, out, writeout, nullptr, &exitcode);

#if defined(LIBBLOC_UNIX)
  if (_handler != SIG_ERR)
    signal(SIGCHLD, _handler);
#endif

  for (char* v : argv) { if (v) delete [] v; }

  /* push error */
  if (ret == SPAWN_SUCCESS)
  {
    _status = exitcode;
    return (exitcode == 0);
  }
  _status = -1;
  return false;
}

int sys::ExecBuffer::write(const char * data, int len)
{
  /* notes:
   * the dried size of data is: buflen - head - tail.
   * head size will never grow above two CHUNK: at each loop the chunk in front
   * is removed if necessary so that the head size remains less than CHUNKLEN.
   * tail cannot grow above CHUNKLEN */
  int rest = len;

  while (rest > 0)
  {
    int n = (rest > CHUNKLEN ? CHUNKLEN : rest);
    rest -= n;

    if (_tail >= n)
    {
      /* the tail of last chunk is large enough to hold the read data */
      ::memcpy(_chunks.back() + CHUNKLEN - _tail, data, n);
      _tail -= n;
    }
    else
    {
      /* first, fill the tail of last chunk */
      if (_tail)
      {
        ::memcpy(_chunks.back() + CHUNKLEN - _tail, data, _tail);
        n -= _tail;
      }
      /* reuse the spare, or allocate a new chunk for the rest (n) */
      if (!_spare)
        _chunks.push_back(new char[CHUNKLEN]);
      else
      {
        _chunks.push_back(_spare);
        _spare = nullptr;
      }
      _bufsize += CHUNKLEN;
      /* fill the new chunk with the rest (n) */
      ::memcpy(_chunks.back(), data + _tail, n);
      /* and reset tail of the last chunk */
      _tail = CHUNKLEN - n;
    }

    /* move head position */
    if (_bufsize > (_outsize + _tail))
    {
      // coverity[overflow_const]
      _head = _bufsize - (_outsize + _tail);
      /* detach the head of buffer which may be unnecessary */
      if (_head >= CHUNKLEN)
      {
        /* reuse head chunk as spare */
        _spare = _chunks.front();
        _chunks.pop_front();
        _bufsize -= CHUNKLEN;
        _head -= CHUNKLEN;
      }
    }
  }
  return len;
}

int sys::ExecBuffer::read(char * c)
{
  if (_inputdata == nullptr || *_inputdata == '\0')
    return 0;
  *c = *_inputdata;
  _inputdata += 1;
  return 1;
}

void sys::ExecBuffer::result(bloc::TabChar& out)
{
  /* fill in the truncated output */
  out.clear();
  out.reserve(_bufsize - _head - _tail);
  /* if len is greater than CHUNKLEN, then buffer contains linked chunks */
  if (_bufsize > CHUNKLEN)
  {
    /* copy the first chunk from position at head */
    out.insert(out.end(), _chunks.front() + _head, _chunks.front() + CHUNKLEN);
    delete [] _chunks.front();
    _chunks.pop_front();
    _bufsize -= CHUNKLEN;
    while (_bufsize > CHUNKLEN)
    {
      /* copy next chunk */
      out.insert(out.end(), _chunks.front(), _chunks.front() + CHUNKLEN);
      delete [] _chunks.front();
      _chunks.pop_front();
      _bufsize -= CHUNKLEN;
    }
    /* copy the last chunk until tail front */
    out.insert(out.end(), _chunks.front(), _chunks.front() + CHUNKLEN - _tail);
    delete [] _chunks.front();
    _chunks.pop_front();
    _bufsize -= CHUNKLEN;
  }
  else
  {
    /* copy the chunk from position at head and until tail front */
    out.insert(out.end(), _chunks.front() + _head, _chunks.front() + CHUNKLEN - _tail);
    delete [] _chunks.front();
    _chunks.pop_front();
  }
  assert(_chunks.size() == 0);
}

int sys::writebuf(void* hdl, const char* data, int len)
{
  ExecBuffer* buf = reinterpret_cast<ExecBuffer*>(hdl);
  return buf->write(data, len);
}

int sys::readchar(void * hdl, char * c)
{
  ExecBuffer* buf = reinterpret_cast<ExecBuffer*>(hdl);
  return buf->read(c);
}

bool sys::Handle::execinout(const std::string& cmd, const char * input,
                            bloc::TabChar& out, size_t maxsize)
{
  std::vector<char*> argv;
  int exitcode = 0;
  int ret;

  ExecBuffer buf(maxsize, input);

#if defined(LIBBLOC_MSWIN)
  argv.push_back(strncpy(new char[8], "CMD.EXE", 8));
  argv.push_back(strncpy(new char[3], "/C", 3));
#else
  void (*_handler)(int) = signal(SIGCHLD, SIG_DFL);
  argv.push_back(strncpy(new char[3], "sh", 3));
  argv.push_back(strncpy(new char[3], "-c", 3));
#endif
  argv.push_back(strncpy(new char[cmd.size()+1], cmd.c_str(), cmd.size()+1));
  argv.push_back(nullptr);

  if (!input)
    ret = spawn_process(argv.data(), environ, &buf, writebuf, nullptr, &exitcode);
  else
    ret = spawn_process(argv.data(), environ, &buf, writebuf, readchar, &exitcode);

#if defined(LIBBLOC_UNIX)
  if (_handler != SIG_ERR)
    signal(SIGCHLD, _handler);
#endif

  for (char* v : argv) { if (v) delete [] v; }
  buf.result(out);

  /* push error */
  if (ret == SPAWN_SUCCESS)
  {
    _status = exitcode;
    return (exitcode == 0);
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

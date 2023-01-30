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

#include "import_file.h"

#include <cstdio>
#include <cerrno>
#if (defined(_WIN32) || defined(_WIN64))
#include "win32/dirent.h"
#else
#include <dirent.h>
#endif
#include <sys/stat.h>

#if (defined(_WIN32) || defined(_WIN64))
#define __WINDOWS__
#endif

#ifdef __WINDOWS__
#define FILE_SEPARATOR '\\'
#else
#define FILE_SEPARATOR '/'
#endif

/*
 * Create the module FileImport
 */
PLUGINCREATOR(FilePlugin)

namespace bloc
{
namespace plugin
{
namespace file
{

/*
 * The module interface
 */

/**********************************************************************/
/*  Constructors                                                      */
/**********************************************************************/
static PLUGIN_TYPE ctor_0_args[]  = {
  { "L", 0 }, // filename
  { "L", 0 }, // open flags
};

static PLUGIN_CTOR ctors[] =
{
  { 0,      2,  ctor_0_args },  /* new file( filename, open flags ) */
};

enum Method
{
  Close = 0, Open, Write_S, Write_B, Read_S, Read_B, Flush,
  SeekSet, SeekCur, SeekEnd, Position, Filename, IsOpen, Mode,
  Stat, Dir, Readln,
};

/**********************************************************************/
/*  Method arguments                                                  */
/*  mode:         type: decl,ndim                                     */
/**********************************************************************/
static PLUGIN_ARG stat_args[]  = {
  { PLUGIN_IN,    { "L", 0 } }, // filename
};

static PLUGIN_ARG dir_args[]  = {
  { PLUGIN_IN,    { "L", 0 } }, // filename
};

static PLUGIN_ARG open_args[]     = {
  { PLUGIN_IN,    { "L", 0 } }, // filename
  { PLUGIN_IN,    { "L", 0 } }, // open flags
};

static PLUGIN_ARG write_s_args[]  = {
  { PLUGIN_IN,    { "L", 0 } }, // write string
};

static PLUGIN_ARG write_b_args[]  = {
  { PLUGIN_IN,    { "X", 0 } }, // write raw
};

static PLUGIN_ARG read_s_args[]   = {
  { PLUGIN_INOUT, { "L", 0 } }, // string read
  { PLUGIN_IN,    { "I", 0 } }, // read size
};

static PLUGIN_ARG read_b_args[]   = {
  { PLUGIN_INOUT, { "X", 0 } }, // raw read
  { PLUGIN_IN,    { "I", 0 } }, // read size
};

static PLUGIN_ARG seek_args[]     = {
  { PLUGIN_IN,    { "I", 0 } }, // seek offset
};

/**********************************************************************/
/*  Methods list                                                      */
/*  id:       name:         ret: decl,ndim  args_count,args:          */
/**********************************************************************/
static PLUGIN_METHOD methods[] =
{
  { Close,    "close",      { "B", 0 },     0, nullptr, },

  { Open,     "open",       { "I", 0 },     2, open_args, },

  { Write_S,  "write",      { "I", 0 },     1, write_s_args, },

  { Read_S,   "read",       { "I", 0 },     2, read_s_args, },

  { SeekSet,  "seekset",    { "I", 0 },     1, seek_args, },

  { SeekCur,  "seekcur",    { "I", 0 },     1, seek_args, },

  { SeekEnd,  "seekend",    { "I", 0 },     1, seek_args, },

  { Position, "position",   { "I", 0 },     0, nullptr, },

  { Flush,    "flush",      { "B", 0 },     0, nullptr, },

  { Filename, "filename",   { "L", 0 },     0, nullptr, },

  { IsOpen,   "isopen",     { "B", 0 },     0, nullptr, },

  { Write_B,  "write",      { "I", 0 },     1, write_b_args, },

  { Read_B,   "read",       { "I", 0 },     2, read_b_args, },

  { Mode,     "mode",       { "L", 0 },     0, nullptr, },

  { Stat,     "stat",       { "I", 0 },     1, stat_args, },

  { Dir,      "dir",        { "IL", 1 },    1, dir_args, },

  { Readln,   "readln",     { "L", 0 },     0, nullptr, },
};

/**
 * The state of file handle
 */
struct Handle {
  FILE * _file = nullptr;
  std::string _path;
  std::string _mode;
  bool _r = false;
  bool _w = false;

  ~Handle() { if (_file) ::fclose(_file); }
  Handle() { }
  int open(const std::string& path, const std::string& flags);
  int close();
  int64_t write(const char * buf, unsigned n);
  int64_t read(char * buf, unsigned n);
  int seek_cur(int64_t s);
  int seek_set(int64_t s);
  int seek_end(int64_t s);
  int64_t position();
  int flush();
  int status(const std::string& path);
  unsigned readln(char * buf, unsigned n);
};

/**
 * stat file
 */
static int _stat(const std::string& path)
{
  struct stat buf;
  int r = ::stat(path.c_str(), &buf);
  if (r == 0)
  {
    if (S_ISREG(buf.st_mode))
      return 1;
    if (S_ISDIR(buf.st_mode))
      return 2;
    return 3;
  }
  r = errno;
  if (r == ENOENT)
    return 0;
  return (-r);
}

/**
 * dir list
 */
static int _dir(const std::string& path, bloc::Collection& out)
{
  DIR *dirp;
  struct dirent *dp;
  if ((dirp = ::opendir(path.c_str())) == nullptr)
    return (-1);
  while ((dp = ::readdir(dirp)) != nullptr)
  {
    /* create the row */
    bloc::Tuple::container_t row;
    row.push_back(new bloc::IntegerExpression(
        (int64_t) _stat(std::string(path).append(1, FILE_SEPARATOR).append(dp->d_name))));
    row.push_back(new bloc::LiteralExpression(dp->d_name));
    /* fill collection with the new tuple */
    out.push_back(new TupleExpression(Tuple(std::move(row))));
  }
  closedir(dirp);
  return errno;
}

} /* namespace file */

void FilePlugin::declareInterface(PLUGIN_INTERFACE * interface)
{
  interface->name = "file";
  interface->method_count = sizeof(file::methods) / sizeof(PLUGIN_METHOD);
  interface->methods = file::methods;
  /* file do not declare any specialized ctor */
  interface->ctors_count = sizeof(file::ctors) / sizeof(PLUGIN_CTOR);
  interface->ctors = file::ctors;
}

void * FilePlugin::createObject(int ctor_id, bloc::Context& ctx, const std::vector<bloc::Expression*>& args)
{
  file::Handle * file = new file::Handle();
  try
  {
    switch (ctor_id)
    {
    case 0: /* file( filename, open flags ) */
      if (file->open(args[0]->literal(ctx), args[1]->literal(ctx)) != 0)
        throw RuntimeError(EXC_RT_OTHER_S, "Failed to open file.");
      break;

    default: /* default ctor */
      break;
    }
  }
  catch (...)
  {
    delete file;
    throw;
  }
  return file;
}

void FilePlugin::destroyObject(void * object)
{
  file::Handle * file = static_cast<file::Handle*>(object);
  delete file;
}

bloc::Expression * FilePlugin::executeMethod(
          bloc::Complex object_this,
          int method_id,
          bloc::Context& ctx,
          const std::vector<bloc::Expression*>& args
          )
{
  file::Handle * file = static_cast<file::Handle*>(object_this.instance());
  switch (method_id)
  {
  case file::Close:
    return new bloc::BooleanExpression((file->close() == 0 ? true : false));

  case file::Open:
    return new bloc::IntegerExpression(file->open(args[0]->literal(ctx), args[1]->literal(ctx)));

  case file::Write_S:
  {
    if (!file->_w)
      throw bloc::RuntimeError(bloc::EXC_RT_OTHER_S, "file not opened for write operation.");
    std::string& str = args[0]->literal(ctx);
    return new bloc::IntegerExpression((int64_t)file->write(str.c_str(), str.size()));
  }

  case file::Read_S:
  {
    if (!file->_r)
      throw bloc::RuntimeError(bloc::EXC_RT_OTHER_S, "file not opened for read operation.");
    int64_t n = args[1]->integer(ctx);
    char * buf = new char[n];
    n = file->read(buf, n);
    /* INOUT */
    bloc::VariableExpression * vout = dynamic_cast<bloc::VariableExpression*>(args[0]);
    vout->store(ctx, bloc::LiteralExpression(std::string(buf, n)));
    delete [] buf;
    return new bloc::IntegerExpression(n);
  }

  case file::SeekSet:
    if (!file->_file)
      throw bloc::RuntimeError(bloc::EXC_RT_OTHER_S, "file not opened.");
    return new bloc::IntegerExpression((int64_t)file->seek_set(args[0]->integer(ctx)));

  case file::SeekCur:
    if (!file->_file)
      throw bloc::RuntimeError(bloc::EXC_RT_OTHER_S, "file not opened.");
    return new bloc::IntegerExpression((int64_t)file->seek_cur(args[0]->integer(ctx)));

  case file::SeekEnd:
    if (!file->_file)
      throw bloc::RuntimeError(bloc::EXC_RT_OTHER_S, "file not opened.");
    return new bloc::IntegerExpression((int64_t)file->seek_end(args[0]->integer(ctx)));

  case file::Position:
    if (!file->_file)
      throw bloc::RuntimeError(bloc::EXC_RT_OTHER_S, "file not opened.");
    return new bloc::IntegerExpression(file->position());

  case file::Flush:
    return new bloc::BooleanExpression((file->flush() == 0 ? true : false));

  case file::Filename:
    return new bloc::LiteralExpression(file->_path);

  case file::Mode:
    return new bloc::LiteralExpression(file->_mode);

  case file::IsOpen:
    return new bloc::BooleanExpression((file->_file ? true : false));

  case file::Write_B:
  {
    if (!file->_w)
      throw bloc::RuntimeError(bloc::EXC_RT_OTHER_S, "file not opened for write operation.");
    bloc::TabChar& buf = args[0]->tabchar(ctx);
    return new bloc::IntegerExpression((int64_t)file->write(buf.data(), buf.size()));
  }

  case file::Read_B:
  {
    if (!file->_r)
      throw bloc::RuntimeError(bloc::EXC_RT_OTHER_S, "file not opened for read operation.");
    int64_t n = args[1]->integer(ctx);
    char * buf = new char[n];
    n = file->read(buf, n);
    /* INOUT */
    bloc::VariableExpression * vout = dynamic_cast<bloc::VariableExpression*>(args[0]);
    vout->store(ctx, bloc::TabcharExpression(bloc::TabChar(buf, buf + n)));
    delete [] buf;
    return new bloc::IntegerExpression(n);
  }

  case file::Stat:
    return new bloc::IntegerExpression((int64_t) file::_stat(args[0]->literal(ctx)));

  case file::Dir:
  {
    Collection ret(bloc::plugin::make_decl("IL", object_this.typeId()), 1);
    file::_dir(args[0]->literal(ctx), ret);
    return new CollectionExpression(std::move(ret));
  }

  case file::Readln:
  {
    if (!file->_r)
      throw bloc::RuntimeError(bloc::EXC_RT_OTHER_S, "file not opened for read operation.");
    char * buf = new char[1024]; /* max length */
    unsigned n = file->readln(buf, 1024);
    bloc::LiteralExpression * out = new bloc::LiteralExpression(std::string(buf, n));
    delete [] buf;
    return out;
  }

  }
  return nullptr;
}

int file::Handle::open(const std::string& path, const std::string& mode)
{
  if (_file)
    close();
  if (!(_file = ::fopen(path.c_str(), mode.c_str())))
    return errno;
  _path = path;
  _mode = mode;
  _w = (mode.find('w') != std::string::npos) || (mode.find('+') != std::string::npos) || (mode.find('a') != std::string::npos);
  _r = (mode.find('r') != std::string::npos) || (mode.find('+') != std::string::npos);
  return 0;
}

int file::Handle::close()
{
  if (!_file)
    return 0;
  int r = ::fclose(_file);
  _file = nullptr;
  _path.clear();
  _mode.clear();
  _w = false;
  _r = false;
  return r;
}

int64_t file::Handle::write(const char * buf, unsigned n)
{
  return (int64_t)::fwrite(buf, 1, n, _file);
}

int64_t file::Handle::read(char * buf, unsigned n)
{
  return (int64_t)::fread(buf, 1, n, _file);
}

int file::Handle::seek_cur(int64_t s)
{
  if (::fseek(_file, s, SEEK_CUR) != 0)
    return errno;
  return 0;
}

int file::Handle::seek_set(int64_t s)
{
  if (::fseek(_file, s, SEEK_SET) != 0)
    return errno;
  return 0;
}

int file::Handle::seek_end(int64_t s)
{
  if (::fseek(_file, s, SEEK_END) != 0)
    return errno;
  return 0;
}

int64_t file::Handle::position()
{
  return ::ftell(_file);
}

int file::Handle::flush()
{
  if (!_file)
    return (-1);
  return ::fflush(_file);
}

unsigned file::Handle::readln(char * buf, unsigned n)
{
  unsigned r = 0;
  int c;
  while (r < n && (c = ::fgetc(_file)) > 0)
  {
    *buf = (char)c;
    ++buf;
    ++r;
    if (c == '\n')
      break;
  }
  return r;
}

} /* namespace import */
} /* namespace bloc */

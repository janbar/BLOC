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

#include "plugin_file.h"
#include <blocc/collection.h>
#include <blocc/tuple.h>

#include <cstdio>
#include <cerrno>
#if defined(LIBBLOC_MSWIN)
#include "win32/dirent.h"
#else
#include <dirent.h>
#include <limits.h>
#endif
#include <sys/stat.h>

#if defined(LIBBLOC_MSWIN)
#define FILE_SEPARATOR '\\'
#ifndef PATH_MAX
#define PATH_MAX 4096
#endif
#else
#define FILE_SEPARATOR '/'
#endif

#define BLOC_FILE_BUFSZ 4096

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
  { 0,      2,  ctor_0_args,
          "Open file for the given filename, and flags:"
          "\nr or rb    Open file for reading."
          "\nw or wb    Truncate to zero length or create file for writing"
          "\na or ab    Append, open or create file for writing at end-of-file"
          "\nr+ or rb+  Open file for update (reading and writing)"
          "\nw+ or wb+  Truncate to zero length or create file for update"
          "\na+ or ab+  Append, open or create file for update, writing at end-of-file"
   },
};

enum Method
{
  Close = 0, Open, Write_S, Write_B, Read_S, Read_B, Readln, Flush,
  SeekSet, SeekCur, SeekEnd, Position, IsOpen, Mode, Filename,
  FDirname, FBasename, FStat,
  Stat, Dir, Separator, Dirname, Basename,
};

/**********************************************************************/
/*  Method arguments                                                  */
/*  mode:         type: decl,ndim                                     */
/**********************************************************************/
static PLUGIN_ARG string1_args[]  = {
  { PLUGIN_IN,    { "L", 0 } }, // string
};

static PLUGIN_ARG string2_args[]     = {
  { PLUGIN_IN,    { "L", 0 } }, // string
  { PLUGIN_IN,    { "L", 0 } }, // string
};

static PLUGIN_ARG raw_args[]  = {
  { PLUGIN_IN,    { "X", 0 } }, // raw
};

static PLUGIN_ARG int_args[]     = {
  { PLUGIN_IN,    { "I", 0 } }, // integer
};

static PLUGIN_ARG read_s_args[]   = {
  { PLUGIN_INOUT, { "L", 0 } }, // string read
  { PLUGIN_IN,    { "I", 0 } }, // read size
};

static PLUGIN_ARG readln_args[]   = {
  { PLUGIN_INOUT, { "L", 0 } }, // line read
};

static PLUGIN_ARG read_b_args[]   = {
  { PLUGIN_INOUT, { "X", 0 } }, // raw read
  { PLUGIN_IN,    { "I", 0 } }, // read size
};

/**********************************************************************/
/*  Methods list                                                      */
/*  id:       name:         ret: decl,ndim  args_count,args:          */
/**********************************************************************/
static PLUGIN_METHOD methods[] =
{
  { Close,    "close",      { "B", 0 },     0, nullptr,
          "Close the file handle." },
  { Open,     "open",       { "I", 0 },     2, string2_args,
          "Opens the file indicated by path and flags, and returns the system error"
          "\nnumber or 0 for success." },
  { Write_S,  "write",      { "I", 0 },     1, string1_args,
          "Writes string, and returns the number of character written successfully." },
  { Write_B,  "write",      { "I", 0 },     1, raw_args,
          "Writes bytes, and returns the number of byte written successfully." },
  { Flush,    "flush",      { "B", 0 },     0, nullptr,
          "Synchronizes output stream with the file." },
  { Readln,   "readln",     { "B", 0 },     1, readln_args,
          "Reads a chunk of characters into the variable, ended by LF or limited by"
          "\nthe size of the internal buffer. At EOF it returns FALSE else TRUE.", },
  { Read_S,   "read",       { "I", 0 },     2, read_s_args,
          "Reads up to count characters into the variable, and returns the number of"
          "\ncharacter read successfully." },
  { Read_B,   "read",       { "I", 0 },     2, read_b_args,
          "Reads up to count bytes into the variable, and returns the number of byte"
          "\nread successfully." },
  { SeekSet,  "seekset",    { "I", 0 },     1, int_args,
          "Moves the file position indicator to an absolute location in a file." },
  { SeekCur,  "seekcur",    { "I", 0 },     1, int_args,
          "Moves the file position indicator from the current file position." },
  { SeekEnd,  "seekend",    { "I", 0 },     1, int_args,
          "Moves the file position indicator from the end of the file." },
  { Position, "position",   { "I", 0 },     0, nullptr,
          "Returns the file position indicator." },
  { IsOpen,   "isopen",     { "B", 0 },     0, nullptr,
          "Checks whether the file is open." },
  { Mode,     "mode",       { "L", 0 },     0, nullptr,
          "Returns open flags of the file." },
  { Filename, "filename",   { "L", 0 },     0, nullptr,
          "Returns filename of the file." },
  { FDirname, "dirname",    { "L", 0 },     0, nullptr,
          "Returns the dirname of the file." },
  { FBasename,  "basename",  { "L", 0 },    0, nullptr,
          "Returns the basename of the file." },
  { FStat,    "stat",       { "IIL", 0 },   0, nullptr,
          "Returns a tuple containing basic informations about the file:"
          "\n{ Type, Size, Absolute Path }"
          "\nType: 0=Invalid, 1=Regular, 2=Directory, 3=Other"
          "\nSize: file size in byte" },
  //
  // global methods
  //
  { Stat,     "stat",       { "IIL", 0 },   1, string1_args,
          "Returns a tuple containing basic informations about the given path:"
          "\n{ Type, Size, Absolute Path }"
          "\nType: 0=Invalid, 1=Regular, 2=Directory, 3=Other"
          "\nSize: file size in byte" },
  { Dir,      "dir",        { "IIL", 1 },    1, string1_args,
          "Returns the entry list of a directory: [{ Type, Size, Name }]"
          "\nType: 1=Regular, 2=Directory, 3=Other"
          "\nSize: file size in byte" },
  { Separator, "separator", { "L", 0 },      0, nullptr,
          "Returns the file separator character." },
  { Dirname, "dirname",     { "L", 0 },      1, string1_args,
          "Returns the dirname of the given path." },
  { Basename,  "basename",  { "L", 0 },      1, string1_args,
          "Returns the basename of the given path." },
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
  size_t write(const char * buf, unsigned n);
  size_t read(char * buf, unsigned n);
  int seek_cur(int64_t s);
  int seek_set(int64_t s);
  int seek_end(int64_t s);
  int64_t position();
  int flush();
  int readln(char * buf, unsigned n);
};

/**
 * stat file
 */
static int _stat(const std::string& path, int * fmode, size_t * fsize)
{
  struct stat buf;
  int r = ::stat(path.c_str(), &buf);
  if (r == 0)
  {
    if (S_ISREG(buf.st_mode))
    {
      *fmode = 1;
      *fsize = (size_t) buf.st_size;
    }
    else if (S_ISDIR(buf.st_mode))
    {
      *fmode = 2;
      *fsize = 0;
    }
    else
    {
      *fmode = 3;
      *fsize = 0;
    }
    return 0;
  }
  r = errno;
  return (-r);
}

/**
 * basename
 */
std::string _baseName(const std::string& path)
{
  size_t p = path.find_last_of(FILE_SEPARATOR);
  if (p == std::string::npos)
    return path;
  p += 1;
  if (p == path.length())
    return _baseName(path.substr(0, p - 1));
  return path.substr(p);
}

/**
 * dirname
 */
std::string _dirName(const std::string& path)
{
  size_t p = path.find_last_of(FILE_SEPARATOR);
  if (p == std::string::npos)
    return std::string(".");
  if (p > 0 && (p + 1) == path.length())
    return _dirName(path.substr(0, p - 1));
  return path.substr(0, p);
}

/**
 * absolute path name
 */
static std::string _absolutePath(const std::string& path)
{
  char buf[PATH_MAX];
  *buf = '\0';
#if defined(LIBBLOC_MSWIN)
  _fullpath(buf, path.c_str(), PATH_MAX);
#else
  if (realpath(path.c_str(), buf) == nullptr)
    return std::string();
#endif
  return std::string(buf);
}

/**
 * dir list
 */
static int _dir(const std::string& path, bloc::Collection ** out)
{
  DIR *dirp;
  struct dirent *dp;
  if ((dirp = ::opendir(path.c_str())) == nullptr)
    return (-1);

  *out = new bloc::Collection(bloc::plugin::make_decl("IIL", 0), 1);
  while ((dp = ::readdir(dirp)) != nullptr)
  {
    int fmode = 0;
    size_t fsize = 0;
    _stat(std::string(path).append(1, FILE_SEPARATOR).append(dp->d_name), &fmode, &fsize);
    /* create the row */
    bloc::Tuple::container_t row;
    row.push_back(bloc::Value(bloc::Integer(fmode)));
    if (fmode == 1)
      row.push_back(bloc::Value(bloc::Integer(fsize)));
    else
      row.push_back(bloc::Value(bloc::Value::type_integer));
    row.push_back(bloc::Value(new bloc::Literal(dp->d_name)));
    /* fill collection with the new tuple */
    (*out)->push_back(Value(new bloc::Tuple(std::move(row))));
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
    {
      bloc::Value& a0 = args[0]->value(ctx);
      bloc::Value& a1 = args[1]->value(ctx);
      if (a0.isNull() || a1.isNull())
        throw RuntimeError(EXC_RT_OTHER_S, "Invalid arguments.");
      if (file->open(*a0.literal(), *a1.literal()) != 0)
        throw RuntimeError(EXC_RT_OTHER_S, "Failed to open file.");
      break;
    }

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

bloc::Value * FilePlugin::executeMethod(
          bloc::Complex& object_this,
          int method_id,
          bloc::Context& ctx,
          const std::vector<bloc::Expression*>& args
          )
{
  file::Handle * file = static_cast<file::Handle*>(object_this.instance());
  switch (method_id)
  {
  case file::Close:
    return new Value(bloc::Bool(file->close() == 0 ? true : false));

  case file::Open:
  {
    bloc::Value& a0 = args[0]->value(ctx);
    bloc::Value& a1 = args[1]->value(ctx);
    if (a0.isNull() || a1.isNull())
      throw RuntimeError(EXC_RT_OTHER_S, "Invalid arguments.");
    return new bloc::Value(bloc::Integer(file->open(*a0.literal(), *a1.literal())));
  }

  case file::Write_S:
  {
    if (!file->_w)
      throw bloc::RuntimeError(bloc::EXC_RT_OTHER_S, "file not opened for write operation.");
    bloc::Value& a0 = args[0]->value(ctx);
    if (!a0.isNull())
      return new bloc::Value(bloc::Integer(file->write(a0.literal()->c_str(), a0.literal()->size())));
    return new bloc::Value(bloc::Integer(0));
  }

  case file::Read_S:
  {
    if (!file->_r)
      throw bloc::RuntimeError(bloc::EXC_RT_OTHER_S, "file not opened for read operation.");
    bloc::Value& a1 = args[1]->value(ctx);
    if (!args[0]->isVarName() || a1.isNull())
      throw RuntimeError(EXC_RT_OTHER_S, "Invalid arguments.");

    bloc::Literal * str = new bloc::Literal();
    bloc::Integer r = 0;
    bloc::Integer l = *a1.integer();
    if (l > 0)
    {
      bloc::Integer n = l;
      str->reserve(n);
      char buf[BLOC_FILE_BUFSZ];
      while (n > 0)
      {
        size_t c = file->read(buf, (n > BLOC_FILE_BUFSZ ? BLOC_FILE_BUFSZ : n));
        str->append(buf, c);
        r += c;
        if (c < BLOC_FILE_BUFSZ)
          break;
        n -= c;
      }
    }
    /* INOUT */
    ctx.storeVariable(*args[0]->symbol(), bloc::Value(str));
    return new bloc::Value(bloc::Integer(r));
  }

  case file::Readln:
  {
    if (!file->_r)
      throw bloc::RuntimeError(bloc::EXC_RT_OTHER_S, "file not opened for read operation.");
    if (!args[0]->isVarName())
      throw RuntimeError(EXC_RT_OTHER_S, "Invalid arguments.");

    char buf[BLOC_FILE_BUFSZ]; /* max length */
    int n = file->readln(buf, sizeof(buf));
    if (n >= 0)
    {
      /* INOUT */
      ctx.storeVariable(*args[0]->symbol(), bloc::Value(new bloc::Literal(buf, n)));
    }
    return new bloc::Value(bloc::Bool(n < 0 ? false : true));
  }

  case file::SeekSet:
  {
    if (!file->_file)
      throw bloc::RuntimeError(bloc::EXC_RT_OTHER_S, "file not opened.");
    bloc::Value& a0 = args[0]->value(ctx);
    if (a0.isNull())
      throw RuntimeError(EXC_RT_OTHER_S, "Invalid arguments.");
    return new bloc::Value(bloc::Integer(file->seek_set(*a0.integer())));
  }

  case file::SeekCur:
  {
    if (!file->_file)
      throw bloc::RuntimeError(bloc::EXC_RT_OTHER_S, "file not opened.");
    bloc::Value& a0 = args[0]->value(ctx);
    if (a0.isNull())
      throw RuntimeError(EXC_RT_OTHER_S, "Invalid arguments.");
    return new bloc::Value(bloc::Integer(file->seek_cur(*a0.integer())));
  }

  case file::SeekEnd:
  {
    if (!file->_file)
      throw bloc::RuntimeError(bloc::EXC_RT_OTHER_S, "file not opened.");
    bloc::Value& a0 = args[0]->value(ctx);
    if (a0.isNull())
      throw RuntimeError(EXC_RT_OTHER_S, "Invalid arguments.");
    return new bloc::Value(bloc::Integer(file->seek_end(*a0.integer())));
  }

  case file::Position:
    if (!file->_file)
      throw bloc::RuntimeError(bloc::EXC_RT_OTHER_S, "file not opened.");
    return new bloc::Value(bloc::Integer(file->position()));

  case file::Flush:
    if (!file->_file)
      throw bloc::RuntimeError(bloc::EXC_RT_OTHER_S, "file not opened.");
    return new bloc::Value(bloc::Bool(file->flush() == 0 ? true : false));

  case file::Filename:
    if (!file->_file)
      throw bloc::RuntimeError(bloc::EXC_RT_OTHER_S, "file not opened.");
    return new bloc::Value(new bloc::Literal(file->_path));

  case file::FDirname:
    if (!file->_file)
      throw bloc::RuntimeError(bloc::EXC_RT_OTHER_S, "file not opened.");
    return new bloc::Value(new bloc::Literal(file::_dirName(file->_path)));

  case file::FBasename:
    if (!file->_file)
      throw bloc::RuntimeError(bloc::EXC_RT_OTHER_S, "file not opened.");
    return new bloc::Value(new bloc::Literal(file::_baseName(file->_path)));

  case file::Mode:
    return new bloc::Value(new bloc::Literal(file->_mode));

  case file::IsOpen:
    return new bloc::Value(bloc::Bool(file->_file ? true : false));

  case file::Write_B:
  {
    if (!file->_w)
      throw bloc::RuntimeError(bloc::EXC_RT_OTHER_S, "file not opened for write operation.");
    bloc::Value& a0 = args[0]->value(ctx);
    if (!a0.isNull())
      return new bloc::Value(bloc::Integer(file->write(a0.tabchar()->data(), a0.tabchar()->size())));
    return new bloc::Value(bloc::Integer(0));
  }

  case file::Read_B:
  {
    if (!file->_r)
      throw bloc::RuntimeError(bloc::EXC_RT_OTHER_S, "file not opened for read operation.");
    bloc::Value& a1 = args[1]->value(ctx);
    if (!args[0]->isVarName() || a1.isNull())
      throw RuntimeError(EXC_RT_OTHER_S, "Invalid arguments.");

    bloc::TabChar * raw = new bloc::TabChar();
    bloc::Integer r = 0;
    bloc::Integer l = *a1.integer();
    if (l > 0)
    {
      bloc::Integer n = l;
      raw->reserve(n);
      char buf[BLOC_FILE_BUFSZ];
      while (n > 0)
      {
        size_t c = file->read(buf, (n > BLOC_FILE_BUFSZ ? BLOC_FILE_BUFSZ : n));
        raw->insert(raw->end(), buf, buf + c);
        r += c;
        if (c < BLOC_FILE_BUFSZ)
          break;
        n -= c;
      }
    }
    /* INOUT */
    ctx.storeVariable(*args[0]->symbol(), bloc::Value(raw));
    return new bloc::Value(bloc::Integer(r));
  }

  case file::FStat:
  {
    if (!file->_file)
      throw bloc::RuntimeError(bloc::EXC_RT_OTHER_S, "file not opened.");
    int fmode = 0;
    size_t fsize = 0;
    /* initialize the row */
    bloc::Tuple::container_t row;
    row.push_back(bloc::Value(bloc::Value::type_integer));
    row.push_back(bloc::Value(bloc::Value::type_integer));
    row.push_back(bloc::Value(bloc::Value::type_literal));
    if (file::_stat(file->_path, &fmode, &fsize) == 0)
    {
      row[0].swap(bloc::Value(bloc::Integer(fmode)));
      if (fmode == 1)
        row[1].swap(bloc::Value(bloc::Integer(fsize)));
      row[2].swap(bloc::Value(new bloc::Literal(file::_absolutePath(file->_path))));
    }
    else
      row[0].swap(bloc::Value(bloc::Integer(0)));
    return new bloc::Value(new bloc::Tuple(std::move(row)));
  }

  case file::Stat:
  {
    bloc::Value& a0 = args[0]->value(ctx);
    if (a0.isNull())
      throw RuntimeError(EXC_RT_OTHER_S, "Invalid arguments.");
    int fmode = 0;
    size_t fsize = 0;
    const std::string& path = *a0.literal();
    /* initialize the row */
    bloc::Tuple::container_t row;
    row.push_back(bloc::Value(bloc::Value::type_integer));
    row.push_back(bloc::Value(bloc::Value::type_integer));
    row.push_back(bloc::Value(bloc::Value::type_literal));
    if (file::_stat(path, &fmode, &fsize) == 0)
    {
      row[0].swap(bloc::Value(bloc::Integer(fmode)));
      if (fmode == 1)
        row[1].swap(bloc::Value(bloc::Integer(fsize)));
      row[2].swap(bloc::Value(new bloc::Literal(file::_absolutePath(path))));
    }
    else
      row[0].swap(bloc::Value(bloc::Integer(0)));
    return new bloc::Value(new bloc::Tuple(std::move(row)));
  }

  case file::Dir:
  {
    bloc::Value& a0 = args[0]->value(ctx);
    if (a0.isNull())
      throw RuntimeError(EXC_RT_OTHER_S, "Invalid arguments.");
    Collection * c = nullptr;
    file::_dir(*a0.literal(), &c);
    return new Value(c);
  }

  case file::Separator:
  {
    return new Value(new bloc::Literal(1, FILE_SEPARATOR));
  }

  case file::Dirname:
  {
    bloc::Value& a0 = args[0]->value(ctx);
    if (a0.isNull())
      throw RuntimeError(EXC_RT_OTHER_S, "Invalid arguments.");
    return new Value(new bloc::Literal(file::_dirName(*a0.literal())));
  }

  case file::Basename:
  {
    bloc::Value& a0 = args[0]->value(ctx);
    if (a0.isNull())
      throw RuntimeError(EXC_RT_OTHER_S, "Invalid arguments.");
    return new Value(new bloc::Literal(file::_baseName(*a0.literal())));
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

size_t file::Handle::write(const char * buf, unsigned n)
{
  return ::fwrite(buf, 1, n, _file);
}

size_t file::Handle::read(char * buf, unsigned n)
{
  return ::fread(buf, 1, n, _file);
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

int file::Handle::readln(char * buf, unsigned n)
{
  int c = 0, r = 0;
  while (r < n &&  c != '\n')
  {
    if ((c = ::fgetc(_file)) <= 0)
      break;
    *buf = (char)c;
    ++buf;
    ++r;
  }
  if (r > 0 && (c >= 0 || c == EOF))
    return r;
  return (c < 0 ? c : r);
}

} /* namespace import */
} /* namespace bloc */

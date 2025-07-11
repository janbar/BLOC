/*
 *      Copyright (C) 2025 Jean-Luc Barriere
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

#include "cli_msgdb.h"
#include <blocc/declspec.h>

#include <string>
#include <cstdlib>
#include <cstring>

#if defined(LIBBLOC_MSWIN)
#include "win32/dlfcn.h"
#define FILE_SEPARATOR '\\'
#define LIBPREFIX "bloc_"
#define LIBSUFFIX ".dll"
#else
#include <dlfcn.h>
#include <unistd.h>
#define FILE_SEPARATOR '/'

#if defined(__APPLE__)
#define LIBPREFIX "libbloc_"
#define LIBSUFFIX "." LIBVERSION ".dylib"

#elif defined(__CYGWIN__)
#define LIBPREFIX "cygbloc_"
#define LIBSUFFIX "-" LIBVERSION ".dll"

#else
#define LIBPREFIX "libbloc_"
#define LIBSUFFIX ".so." LIBVERSION
#endif
#endif

typedef const char* (*DLSYM_msgdb_setup)(const char *);
typedef const MsgDB::msgdb_lang* (*DLSYM_msgdb_get_lang)(const char *);
typedef const MsgDB::msgdb_section* (*DLSYM_msgdb_get_section)(const char*);
typedef const MsgDB::msgdb_keyword* (*DLSYM_msgdb_get_text)(const char*, const char*);

struct MsgDB::Interface
{
  void * dlhandle;
  DLSYM_msgdb_setup setup;
  DLSYM_msgdb_get_lang get_lang;
  DLSYM_msgdb_get_section get_section;
  DLSYM_msgdb_get_text get_text;
};

MsgDB::~MsgDB()
{
  if (_msgdb)
  {
    dlclose(_msgdb->dlhandle);
    delete _msgdb;
    _msgdb = nullptr;
  }
}

bool MsgDB::initialize()
{
  if (_msgdb)
    return true;

  void* dlhandle = nullptr;
  std::string libs;
  libs.assign(LIBPREFIX).append("msgdb").append(LIBSUFFIX);
  dlhandle = dlopen(libs.c_str(), RTLD_LAZY);

  if (!dlhandle)
    return false;

  Interface msgdb;
  msgdb.dlhandle = dlhandle;
  msgdb.setup = (DLSYM_msgdb_setup)dlsym(dlhandle, "msgdb_setup");
  msgdb.get_lang = (DLSYM_msgdb_get_lang)dlsym(dlhandle, "msgdb_get_lang");
  msgdb.get_section = (DLSYM_msgdb_get_section)dlsym(dlhandle, "msgdb_get_section");
  msgdb.get_text = (DLSYM_msgdb_get_text)dlsym(dlhandle, "msgdb_get_text");

  if (msgdb.setup != nullptr && msgdb.get_lang != nullptr &&
          msgdb.get_section != nullptr && msgdb.get_text != nullptr)
  {
    _msgdb = new Interface();
    *_msgdb = msgdb;
    return true;
  }

  dlclose(dlhandle);
  return false;
}

void MsgDB::setup(const std::string& lang)
{
  if (_msgdb)
    _msgdb->setup(lang.c_str());
}

const MsgDB::msgdb_keyword * MsgDB::getText(const std::string& section, const std::string& keyword)
{
  if (_msgdb)
    return _msgdb->get_text(section.c_str(), keyword.c_str());
  return nullptr;
}

const MsgDB::msgdb_section * MsgDB::getSection(const std::string& section)
{
  if (_msgdb)
    return _msgdb->get_section(section.c_str());
  return nullptr;
}

const MsgDB::msgdb_lang * MsgDB::getLang(const std::string& lang)
{
  if (_msgdb)
    return _msgdb->get_lang(lang.c_str());
  return nullptr;
}

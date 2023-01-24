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

#include "import_manager.h"
#include "debug.h"

#include <cstdlib>
#include <cstring>

#if (defined(_WIN32) || defined(_WIN64))
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
#define LIBSUFFIX "." LIBSOVERSION ".dylib"

#elif defined(__CYGWIN__)
#define LIBPREFIX "cygbloc_"
#define LIBSUFFIX "-" LIBSOVERSION ".dll"

#else
#define LIBPREFIX "libbloc_"
#define LIBSUFFIX ".so." LIBSOVERSION
#endif
#endif

namespace bloc
{

ImportManager * ImportManager::_instance = nullptr;

IMPORT_INTERFACE ImportManager::_internal = { "null", 0, nullptr, 0, nullptr };

ImportManager& ImportManager::instance()
{
  if (_instance)
    return *_instance;
  return *(_instance = new ImportManager());
}

void ImportManager::destroy()
{
  if (_instance)
    delete _instance;
  _instance = nullptr;
}

unsigned ImportManager::findModuleTypeId(const std::string & name) const
{
  for (unsigned i = _modules.size() - 1; i > 0; --i)
  {
    if (name == _modules[i].interface.name)
      return i;
  }
  return 0;
}

ImportManager::ImportManager()
{
  IMPORT_MODULE module;
  module.interface = _internal;
  module.instance = nullptr;
  module.dlhandle = nullptr;
  _modules.emplace_back(module);
}

ImportManager::~ImportManager()
{
  for (IMPORT_MODULE& m : _modules)
  {
    if (m.instance)
      delete m.instance;
    if (m.dlhandle)
      dlclose(m.dlhandle);
  }
  _modules.clear();
}

std::vector<const IMPORT_INTERFACE*> ImportManager::reportInterfaces() const
{
  std::vector<const IMPORT_INTERFACE*> v;
  v.reserve(_modules.size() - 1);
  for (unsigned i = 1; i < _modules.size(); ++i)
    v.emplace_back(&_modules[i].interface);
  return v;
}

unsigned ImportManager::importTypeByName(const std::string & name)
{
  void* dlhandle = nullptr;
  std::string libs;
  libs.assign(LIBPREFIX).append(name).append(LIBSUFFIX);
  dlhandle = dlopen(libs.c_str(), RTLD_LAZY);

  if (!dlhandle)
  {
    DBG(DBG_ERROR, "%s: %s\n", __FUNCTION__, dlerror());
    return 0;
  }
  return registerModule(dlhandle);
}

unsigned ImportManager::importTypeByPath(const char * libpath)
{
  void* dlhandle = nullptr;
  dlhandle = dlopen(libpath, RTLD_LAZY);
  if (!dlhandle)
  {
    DBG(DBG_ERROR, "%s: %s\n", __FUNCTION__, dlerror());
    return 0;
  }
  return registerModule(dlhandle);
}

unsigned ImportManager::registerModule(void* dlhandle)
{
  /* check for already registered dlhandle */
  for (int id = 0; id < _modules.size(); ++id)
  {
    if (_modules[id].dlhandle == dlhandle)
    {
      DBG(DBG_WARN, "%s: module '%s' already imported.\n", __FUNCTION__, _modules[id].interface.name);
      return id;
    }
  }
  DLSYM_VERSION version = (DLSYM_VERSION)dlsym(dlhandle, "IMPORT_version");
  if (!version)
  {
    DBG(DBG_ERROR, "%s: %s\n", __FUNCTION__, dlerror());
    return 0;
  }
  if (version() != IMPORT_VERSION)
  {
    DBG(DBG_ERROR, "%s: version mismatch: found %d, requires %d\n", __FUNCTION__, version(), IMPORT_VERSION);
    return 0;
  }
  DLSYM_CREATE create = (DLSYM_CREATE)dlsym(dlhandle, "IMPORT_create");
  if (!create)
  {
    DBG(DBG_ERROR, "%s: %s\n", __FUNCTION__, dlerror());
    return 0;
  }

  bloc::import::ImportBase * instance = static_cast<bloc::import::ImportBase*>(create());
  if (!instance)
  {
    dlclose(dlhandle);
    DBG(DBG_ERROR, "%s: registering import failed\n", __FUNCTION__);
    return 0;
  }

  IMPORT_MODULE module;
  instance->declareInterface(&module.interface);
  module.instance = instance;
  module.dlhandle = dlhandle;
  /* check for name conflict */
  if (findModuleTypeId(module.interface.name) > 0)
  {
    DBG(DBG_ERROR, "%s: module '%s' already imported.\n", __FUNCTION__, module.interface.name);
    delete instance;
    dlclose(dlhandle);
    return 0;
  }
  /* register the module */
  _modules.emplace_back(module);
  return (_modules.size() - 1);
}

}

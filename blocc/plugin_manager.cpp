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

#include "plugin_manager.h"
#include "debug.h"

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

PluginManager * PluginManager::_instance = nullptr;

PLUGIN_INTERFACE PluginManager::_internal = { "null", 0, nullptr, 0, nullptr };

PluginManager& PluginManager::instance()
{
  if (_instance)
    return *_instance;
  return *(_instance = new PluginManager());
}

void PluginManager::destroy()
{
  if (_instance)
    delete _instance;
  _instance = nullptr;
}

unsigned PluginManager::findModuleTypeId(const std::string & name) const
{
  for (unsigned i = _modules.size() - 1; i > 0; --i)
  {
    if (name == _modules[i].interface.name)
      return i;
  }
  return 0;
}

PluginManager::PluginManager()
{
  PLUGGED_MODULE plug;
  plug.interface = _internal;
  plug.instance = nullptr;
  plug.dlhandle = nullptr;
  _modules.emplace_back(plug);
}

PluginManager::~PluginManager()
{
  for (PLUGGED_MODULE& m : _modules)
  {
    if (m.instance)
      delete m.instance;
    if (m.dlhandle)
      dlclose(m.dlhandle);
  }
  _modules.clear();
}

std::vector<const PLUGIN_INTERFACE*> PluginManager::reportInterfaces() const
{
  std::vector<const PLUGIN_INTERFACE*> v;
  v.reserve(_modules.size() - 1);
  for (unsigned i = 1; i < _modules.size(); ++i)
    v.emplace_back(&_modules[i].interface);
  return v;
}

unsigned PluginManager::importModuleByName(const std::string & name)
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

unsigned PluginManager::importModuleByPath(const char * libpath)
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

void PluginManager::unbanPlugin(const std::string& name)
{
  if (bannedPlugin(name))
    _trustedPluginNames.push_back(name);
}

bool PluginManager::bannedPlugin(const std::string& name)
{
  for (auto& n : _trustedPluginNames)
  {
    if (n == name)
      return false;
  }
  return true;
}

unsigned PluginManager::registerModule(void* dlhandle)
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
  DLSYM_VERSION version = (DLSYM_VERSION)dlsym(dlhandle, "PLUGIN_version");
  if (!version)
  {
    DBG(DBG_ERROR, "%s: %s\n", __FUNCTION__, dlerror());
    return 0;
  }
  if (version() != PLUGIN_VERSION)
  {
    DBG(DBG_ERROR, "%s: version mismatch: found %d, requires %d\n", __FUNCTION__, version(), PLUGIN_VERSION);
    return 0;
  }
  DLSYM_CREATE create = (DLSYM_CREATE)dlsym(dlhandle, "PLUGIN_create");
  if (!create)
  {
    DBG(DBG_ERROR, "%s: %s\n", __FUNCTION__, dlerror());
    return 0;
  }

  bloc::plugin::PluginBase * instance = static_cast<bloc::plugin::PluginBase*>(create());
  if (!instance)
  {
    dlclose(dlhandle);
    DBG(DBG_ERROR, "%s: registering import failed\n", __FUNCTION__);
    return 0;
  }

  PLUGGED_MODULE plug;
  instance->declareInterface(&plug.interface);
  plug.instance = instance;
  plug.dlhandle = dlhandle;
  /* check for name conflict */
  if (findModuleTypeId(plug.interface.name) > 0)
  {
    DBG(DBG_ERROR, "%s: module '%s' already imported.\n", __FUNCTION__, plug.interface.name);
    delete instance;
    dlclose(dlhandle);
    return 0;
  }
  /* register the module */
  _modules.emplace_back(plug);
  return (_modules.size() - 1);
}

}

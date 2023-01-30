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

#ifndef PLUGIN_MANAGER_H_
#define PLUGIN_MANAGER_H_

#include "plugin.h"

#include <string>
#include <vector>

namespace bloc
{

struct PLUGGED_MODULE
{
  PLUGIN_INTERFACE interface;
  plugin::PluginBase* instance;
  void* dlhandle;
};

class PluginManager
{

public:
  ~PluginManager();

  static PluginManager& instance();
  
  static void destroy();

  std::vector<const PLUGIN_INTERFACE*> reportInterfaces() const;

  unsigned findModuleTypeId(const std::string& name) const;

  const PLUGGED_MODULE& plugged(unsigned type_id) const
  {
    if (type_id < _modules.size())
        return _modules[type_id];
    return _modules[0];
  }

  unsigned importModuleByName(const std::string& name);
  unsigned importModuleByPath(const char * libpath);

private:
  PluginManager();

  static PluginManager * _instance;
  std::vector<PLUGGED_MODULE> _modules;
  static PLUGIN_INTERFACE _internal;

  unsigned registerModule(void* dlhandle);
};

}

#endif /* PLUGIN_MANAGER_H_ */

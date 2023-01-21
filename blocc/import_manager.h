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

#ifndef IMPORT_MANAGER_H_
#define IMPORT_MANAGER_H_

#include "import.h"

#include <string>
#include <vector>

namespace bloc
{

struct IMPORT_MODULE
{
  IMPORT_INTERFACE interface;
  import::ImportBase* instance;
  void* dlhandle;
};

class ImportManager
{

public:
  ~ImportManager();

  static ImportManager& instance();
  
  static void destroy();

  std::vector<const IMPORT_INTERFACE*> reportInterfaces() const;

  unsigned findModuleTypeId(const std::string& name) const;

  const IMPORT_MODULE& module(unsigned type_id) const
  {
    if (type_id < _modules.size())
        return _modules[type_id];
    return _modules[0];
  }

  unsigned importTypeByName(const std::string& name);
  unsigned importTypeByPath(const char * libpath);

private:
  ImportManager();

  static ImportManager * _instance;
  std::vector<IMPORT_MODULE> _modules;
  static IMPORT_INTERFACE _internal;

  unsigned registerModule(void* dlhandle);
};

}

#endif /* IMPORT_MANAGER_H_ */

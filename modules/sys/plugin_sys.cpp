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
#include <cstdlib>

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
  Exec0 = 0,
};

/**********************************************************************/
/*  Method arguments                                                  */
/*  mode:         type: decl,ndim                                     */
/**********************************************************************/
static PLUGIN_ARG exec0_args[]  = {
  { PLUGIN_IN,    { "L", 0 } }, // command
};

/**********************************************************************/
/*  Methods list                                                      */
/*  id:       name:         ret: decl,ndim  args_count,args:          */
/**********************************************************************/
static PLUGIN_METHOD methods[] =
{
  { Exec0,        "exec",             { "B", 0 },     1, exec0_args,
          "Execute a system command or a block of commands." },
};

/**
 * The state of file handle
 */
struct Handle
{
  ~Handle() { }
  Handle() { }
  bool exec(const std::string& cmd);
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

} /* namespace import */
} /* namespace bloc */

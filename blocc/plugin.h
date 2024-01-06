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

#ifndef PLUGIN_H_
#define PLUGIN_H_

#include "plugin_interface.h"
#include "declspec.h"
#include "context.h"
#include "expression.h"
#include "complex.h"

#include <vector>

#define TUPLE_MAX_ITEMS   0x20

namespace bloc
{
namespace plugin
{

/**
 * Build tuple declaration from PLUGIN_DECL definition.
 * The tuple declaration (decl_t) is required to create a collection of tuple.
 * Use this function to build it, before calling the collection CTOR.
 * @param decl_def the tuple definition
 * @param type_id the identifier of the this module
 */
TupleDecl::Decl
make_decl(PLUGIN_DECL decl_def, Type::TypeMinor type_id);

/**
 * Build type from PLUGIN_TYPE definition.
 * The type (Type) is required to create a collection of intrinsic type. Use
 * this function to build it, before calling the collection CTOR.
 * @param type_def the type definition
 * @param type_id the identifier of the this module
 */
Type
make_type(PLUGIN_TYPE type_def, Type::TypeMinor type_id);


class PluginBase
{
public:
  PluginBase() { }
  virtual ~PluginBase() { }

  /**
   * Transfers the definition of the interface
   * @param interface
   */
  virtual void declareInterface(PLUGIN_INTERFACE * interface) = 0;

  /**
   * Create object instance and return its pointer.
   * @param ctx           the bloc context
   * @param args          list of CTOR arguments
   * @return              pointer to new instance
   */
  virtual void * createObject(int ctor_id, bloc::Context& ctx, const std::vector<bloc::Expression*>& args) = 0;

  /**
   * Destroy object instance and free resources
   * @param object        pointer to instance
   */
  virtual void destroyObject(void * object) = 0;

  /**
   * Execute the given method.
   * It should return a pointer to new expression from type:
   *    BooleanExpression, IntegerExpression, NumericExpression,
   *    LiteralExpression, TabcharExpression, TupleExpression,
   * or self by returning new ComplexExpression(object_this).
   * The returned pointer will be freed by the caller after payload processing.
   * Returning nullptr will throw RuntimeError(EXC_RT_MEMB_FAILED_S). Obviously
   * it is preferable to throw RuntimeError with a more meaningful message.
   *
   * Read IN/OUT argument:
   * Arguments can be read from the Expression pointer without cast.
   *
   * Write OUT argument:
   * Expression pointer must be cast to VariableExpression pointer. Then value
   * can be store calling the member store(ctx&, {Type}Expression&&(value)).
   *
   * @param object_this   wrapper for the object
   * @param method_id     identifier of the method to execute
   * @param ctx           the bloc context
   * @param args          list of arguments
   * @return              new value or nullptr
   */
  virtual Value * executeMethod(
          bloc::Complex& object_this,
          int method_id,
          bloc::Context& ctx,
          const std::vector<bloc::Expression*>& args
          ) = 0;
};


#define PLUGINCREATOR(PluginClass) \
extern "C" LIBBLOC_DLL_EXPORT PLUGIN_HANDLE PLUGIN_create() \
{\
  using namespace bloc::plugin;\
  return new PluginClass;\
}\
extern "C" LIBBLOC_DLL_EXPORT int PLUGIN_version() \
{\
  return PLUGIN_VERSION;\
}\

} /* namespace plugin */
} /* namespace bloc */

#endif /* PLUGIN_H_ */


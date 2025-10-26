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

#ifndef PLUGIN_CRYPTO_H_
#define PLUGIN_CRYPTO_H_

#include <blocc/plugin.h>

namespace bloc
{
namespace plugin
{

class CRYPTOPlugin final : public PluginBase
{
public:
  CRYPTOPlugin() = default;

  void declareInterface(PLUGIN_INTERFACE * interface) override;

  void * createObject(int ctor_id, bloc::Context& ctx, const std::vector<bloc::Expression*>& args) override;

  void destroyObject(void * object) override;

  Value * executeMethod(
          bloc::Complex& object_this,
          int method_id,
          bloc::Context& ctx,
          const std::vector<bloc::Expression*>& args
          ) override;
};

}
}

#endif /* PLUGIN_CRYPTO_H_ */


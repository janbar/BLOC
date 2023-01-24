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

#ifndef IMPORT_DATE_H_
#define IMPORT_DATE_H_

#include <blocc/import.h>

namespace bloc
{
namespace import
{

class DateImport : public ImportBase
{
public:
  ~DateImport() { }
  DateImport() = default;

  void declareInterface(IMPORT_INTERFACE * interface) override;

  void * createObject(int ctor_id, bloc::Context& ctx, const std::vector<bloc::Expression*>& args) override;

  void destroyObject(void * object) override;

  Expression * executeMethod(
          bloc::Complex object_this,
          int method_id,
          bloc::Context& ctx,
          const std::vector<bloc::Expression*>& args
          ) override;
};

}
}

#endif /* IMPORT_DATE_H_ */

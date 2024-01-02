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

#ifndef EXPRESSION_TABCHAR_H
#define EXPRESSION_TABCHAR_H

#include "expression_static.h"
#include "declspec.h"

namespace bloc
{

/**
 * This class implements the simplest possible expression, a char array value.
 */
class TabcharExpression : public SwappableExpression<TabcharExpression>
{
private:

  mutable TabChar v;

public:
  LIBBLOC_API static const Type& type_static;

  virtual ~TabcharExpression() { }

  explicit TabcharExpression(const TabChar& a) : v(a) { }
  explicit TabcharExpression(TabChar &&a) : v(std::move(a)) { }

  const Type& refType() const override
  {
    return type_static;
  }

  TabChar& tabchar(Context& ctx) const override
  {
    return v;
  }

  std::string unparse(Context& ctx) const override
  {
    return ""; /* not printable */
  }

  std::string toString(Context& ctx) const override
  {
    return std::string(typeName(ctx))
            .append(1, '[')
            .append(std::to_string(v.size()))
            .append(1, ']');
  }

  TabChar& refTabchar() override { return v; }

  void swap(TabcharExpression& e) noexcept override
  {
    v.swap(e.v);
  }

  TabcharExpression * swapNew() override
  {
    return new TabcharExpression(std::move(this->v));
  }

  TabcharExpression * cloneNew() const override
  {
    return new TabcharExpression(this->v);
  }

  static void outputTabchar(const TabChar& v, FILE * _stdout, unsigned max_lines = (-1));
};

}

#endif /* EXPRESSION_TABCHAR_H */

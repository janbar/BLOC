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

#ifndef EXPRESSION_BOOLEAN_H
#define EXPRESSION_BOOLEAN_H

#include "expression_static.h"
#include "declspec.h"

namespace bloc
{

/**
 * This class implements the simplest possible expression, a boolean value.
 */
class BooleanExpression : public StaticExpression
{
private:

  bool v;

public:
  LIBBLOC_API static const Type& type_static;
  LIBBLOC_API static const char * TRUE;
  LIBBLOC_API static const char * FALSE;

  virtual ~BooleanExpression() { }

  explicit BooleanExpression(bool a) : StaticExpression(), v(a) { }

  const Type& type(Context& ctx) const override
  {
    return type_static;
  }

  bool boolean(Context& ctx) const override
  {
    return v;
  }

  std::string unparse(Context& ctx) const override;

  std::string toString(Context& ctx) const override
  {
    return std::string(typeName(ctx))
            .append(1, ' ')
            .append(readableBoolean(v));
  }

  static std::string readableBoolean(bool b)
  {
    return (b ? TRUE : FALSE);
  }

  const Type& refType() const override { return type_static; }

  bool& refBoolean() override { return v; }

  void swap(StaticExpression& e) override
  {
    bool tmp = e.refBoolean();
    e.refBoolean() = v;
    v = tmp;
  }

  BooleanExpression * swapNew() override
  {
    return new BooleanExpression(this->v);
  }

  BooleanExpression * cloneNew() const override
  {
    return new BooleanExpression(this->v);
  }
};

}

#endif /* EXPRESSION_BOOLEAN_H */

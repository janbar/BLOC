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

#ifndef BUILTIN_NULL_H_
#define BUILTIN_NULL_H_

#include <blocc/expression_builtin.h>
#include <blocc/value.h>

namespace bloc
{

class Context;
class Parser;

class NULLExpression : public BuiltinExpression {

  mutable Value v;

public:

  virtual ~NULLExpression() { }

  NULLExpression() : BuiltinExpression(FUNC_NIL)
  , v(Value::type_no_type) { }

  const Type& type(Context& ctx) const override { return v.type(); }

  Value& value(Context& ctx) const override { return v; }

  /* immutable */
  bool isConst() const override { return true; }
};

}

#endif /* BUILTIN_NULL_H_ */

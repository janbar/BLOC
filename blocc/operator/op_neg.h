/*
 *      Copyright (C) 2023 Jean-Luc Barriere
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

#ifndef OP_NEG_H_
#define OP_NEG_H_

#include <blocc/operator.h>
#include <blocc/expression.h>

namespace bloc
{

class Context;
class Parser;

class OpNEGExpression : public Expression
{
  Expression * arg1 = nullptr;

public:

  virtual ~OpNEGExpression();

  OpNEGExpression(Expression * a)
  : arg1(a) { }

  const Type& type(Context& ctx) const override;

  Value& value(Context& ctx) const override;

  std::string unparse(Context& ctx) const override;

  std::string toString(Context& ctx) const override
  {
    return Operator::OPVALS[Operator::OP_NEG];
  }
};

}

#endif /* OP_NEG_H */


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

#ifndef OP_SUB_H_
#define OP_SUB_H_

#include <blocc/operator.h>
#include <blocc/expression.h>

namespace bloc
{

class Context;
class Parser;

class OpSUBExpression : public Expression
{
  Expression * arg1 = nullptr;
  Expression * arg2 = nullptr;
  bool enc = false;

public:

  virtual ~OpSUBExpression();

  OpSUBExpression(Expression * a, Expression * b, bool enc)
  : arg1(a), arg2(b), enc(enc) { }

  const Type& type(Context& ctx) const override;

  Value& value(Context& ctx) const override;

  std::string unparse(Context& ctx) const override;

  bool enclosed() const override { return enc; }

  std::string toString(Context& ctx) const override
  {
    return Operator::OPVALS[Operator::OP_SUB];
  }
};

}

#endif /* OP_SUB_H */


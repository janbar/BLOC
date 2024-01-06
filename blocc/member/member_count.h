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

#ifndef MEMBER_COUNT_H_
#define MEMBER_COUNT_H_

#include <blocc/expression_member.h>
#include <blocc/value.h>

namespace bloc
{

class Context;
class Parser;

class MemberCOUNTExpression : public MemberExpression
{
public:

  virtual ~MemberCOUNTExpression() { }

  explicit MemberCOUNTExpression(Expression * e)
  : MemberExpression(BTM_COUNT, e) { }

  const Type& type(Context& ctx) const override { return Value::type_integer; }

  Value& value(Context& ctx) const override;

  static MemberCOUNTExpression * parse(Parser& p, Context& ctx, Expression * exp);
};

}

#endif /* MEMBER_COUNT_H_ */


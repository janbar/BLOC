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

#ifndef MEMBER_SET_H_
#define MEMBER_SET_H_

#include <blocc/expression_member.h>

namespace bloc
{

class Context;
class Parser;

class MemberSETExpression : public MemberExpression
{
  unsigned _index = 0;

public:

  virtual ~MemberSETExpression() { }

  explicit MemberSETExpression(Expression * e, std::vector<Expression*>&& args, unsigned index)
  : MemberExpression(BTM_SET, e, std::move(args)), _index(index) { }

  const Type& type(Context& ctx) const override { return _exp->type(ctx); }

  Tuple& tuple(Context& ctx) const override;

  const Tuple::Decl& tuple_decl(Context& ctx) const override { return _exp->tuple_decl(ctx); }

  std::string typeName(Context& ctx) const override { return _exp->typeName(ctx); }

  bool isRvalue() const override { return _exp->isRvalue(); }

  std::string unparse(Context& ctx) const override;

  static MemberSETExpression * parse(Parser& p, Context& ctx, Expression * exp);
};

}

#endif /* MEMBER_SET_H_ */


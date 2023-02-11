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

#ifndef MEMBER_AT_H_
#define MEMBER_AT_H_

#include <blocc/expression_member.h>

namespace bloc
{

class Context;
class Parser;

class MemberATExpression : public MemberExpression
{
  mutable Type _type_volatile;

public:

  virtual ~MemberATExpression() { }

  MemberATExpression(Expression * e, std::vector<Expression*>&& args)
  : MemberExpression(BTM_AT, e, std::move(args)) { }

  const Type& type(Context& ctx) const override;

  bool boolean(Context& ctx) const override;

  int64_t integer(Context& ctx) const override;

  double numeric(Context& ctx) const override;

  std::string& literal(Context& ctx) const override;

  TabChar& tabchar(Context& ctx) const override;

  Collection& collection(Context& ctx) const override;

  Tuple& tuple(Context& ctx) const override;

  const TupleDecl::Decl& tuple_decl(Context& ctx) const override;

  Complex& complex(Context& ctx) const override;

  bool isRvalue() const override { return _exp->isRvalue(); }

  static MemberATExpression * parse(Parser& p, Context& ctx, Expression * exp);
};

}

#endif /* MEMBER_AT_H */


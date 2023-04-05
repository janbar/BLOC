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

#ifndef MEMBER_DELETE_H_
#define MEMBER_DELETE_H_

#include <blocc/expression_member.h>

namespace bloc
{

class Context;
class Parser;

class MemberDELETEExpression : public MemberExpression
{
public:

  virtual ~MemberDELETEExpression() { }

  explicit MemberDELETEExpression(Expression * e, std::vector<Expression*>&& args)
  : MemberExpression(BTM_DELETE, e, std::move(args)) { }

  const Type& type(Context& ctx) const override { return _exp->type(ctx); }

  bool boolean(Context& ctx) const override { throw RuntimeError(EXC_RT_MEMB_NOT_IMPL_S, KEYWORDS[BTM_DELETE]); }

  int64_t integer(Context& ctx) const override { throw RuntimeError(EXC_RT_MEMB_NOT_IMPL_S, KEYWORDS[BTM_DELETE]); }

  double numeric(Context& ctx) const override { throw RuntimeError(EXC_RT_MEMB_NOT_IMPL_S, KEYWORDS[BTM_DELETE]); }

  std::string& literal(Context& ctx) const override;

  TabChar& tabchar(Context& ctx) const override;

  Collection& collection(Context& ctx) const override;

  Tuple& tuple(Context& ctx) const override { throw RuntimeError(EXC_RT_MEMB_NOT_IMPL_S, KEYWORDS[BTM_DELETE]); }

  Complex& complex(Context& ctx) const override { throw RuntimeError(EXC_RT_MEMB_NOT_IMPL_S, KEYWORDS[BTM_DELETE]); }

  const Tuple::Decl& tuple_decl(Context& ctx) const override { return _exp->tuple_decl(ctx); }

  std::string typeName(Context& ctx) const override { return _exp->typeName(ctx); }

  bool isRvalue() const override { return _exp->isRvalue(); }

  bool isStored() const override { return _exp->isStored(); }

  static MemberDELETEExpression * parse(Parser& p, Context& ctx, Expression * exp);
};

}

#endif /* MEMBER_DELETE_H_ */


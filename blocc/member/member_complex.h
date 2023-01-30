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

#ifndef MEMBER_COMPLEX_H_
#define MEMBER_COMPLEX_H_

#include <blocc/expression_member.h>
#include <blocc/plugin_interface.h>

#include <string>
#include <vector>

namespace bloc
{

class Context;
class Parser;

class MemberMETHODExpression : public MemberExpression
{
  Tuple::Decl _decl_method;
  Type _type_method;

  const PLUGIN_METHOD * _method = nullptr;
  Type::TypeMinor _method_type_id = 0;

public:

  virtual ~MemberMETHODExpression() { }

  MemberMETHODExpression(const PLUGIN_METHOD& method, unsigned type_id, Expression * e);

  MemberMETHODExpression(const PLUGIN_METHOD& method, unsigned type_id, Expression * e, std::vector<Expression*>&& args)
  : MemberMETHODExpression(method, type_id, e)
  {
    _args.swap(args);
  }

  const Type& type(Context& ctx) const override { return _type_method; }

  bool boolean(Context& ctx) const override;

  int64_t integer(Context& ctx) const override;

  double numeric(Context& ctx) const override;

  std::string& literal(Context& ctx) const override;

  TabChar& tabchar(Context& ctx) const override;

  Tuple& tuple(Context& ctx) const override;

  const Tuple::Decl& tuple_decl(Context& ctx) const override { return _decl_method; }

  Collection& collection(Context& ctx) const override;

  Complex complex(Context& ctx) const override;

  std::string typeName(Context& ctx) const override;

  std::string toString(Context& ctx) const override
  {
    return _method->name;
  }

  std::string unparse(Context& ctx) const override;

  static MemberMETHODExpression * parse(Parser& p, Context& ctx, Expression * exp);
};

}

#endif /* MEMBER_COMPLEX_H */


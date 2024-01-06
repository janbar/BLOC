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

#ifndef BUILTIN_TAB_H_
#define BUILTIN_TAB_H_

#include <blocc/expression_builtin.h>

namespace bloc
{

class Context;
class Parser;

class TABExpression : public BuiltinExpression {

  TupleDecl::Decl _decl;
  mutable Type _type_volatile;

public:

  virtual ~TABExpression() { }

  explicit TABExpression(std::vector<Expression*>&& args) : BuiltinExpression(FUNC_TAB, std::move(args)) { }

  const Type& type(Context& ctx) const override
  {
    /* return upper level of volatile level */
    _type_volatile = (_args.empty() ? Value::type_no_type.levelUp() : _args[1]->type(ctx).levelUp());
    return _type_volatile;
  }

  Value& value(Context& ctx) const override;

  const TupleDecl::Decl& tuple_decl(Context& ctx) const override
  {
    return (_args.empty() ? TupleDecl::no_decl : _args[1]->tuple_decl(ctx));
  }

  std::string typeName(Context& ctx) const override;

  static TABExpression * parse(Parser& p, Context& ctx);
};

}

#endif /* BUILTIN_TAB_H_ */

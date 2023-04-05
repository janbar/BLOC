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

#ifndef BUILTIN_TUPLE_H_
#define BUILTIN_TUPLE_H_

#include <blocc/expression_builtin.h>
#include <blocc/expression_boolean.h>
#include <blocc/expression_integer.h>
#include <blocc/expression_numeric.h>
#include <blocc/expression_literal.h>
#include <blocc/expression_tabchar.h>
#include <blocc/expression_complex.h>
#include <blocc/expression_tuple.h>

namespace bloc
{

class Context;
class Parser;

class TUPExpression : public BuiltinExpression
{

  Tuple::Decl _decl;
  Type _type;

public:

  virtual ~TUPExpression() { }

  TUPExpression(std::vector<Expression*>&& args, Context& ctx);

  const Type& type(Context& ctx) const override { return _type; }

  Tuple& tuple(Context& ctx) const override;

  const Tuple::Decl& tuple_decl(Context& ctx) const override { return _decl; }

  std::string typeName(Context& ctx) const override
  {
    return _decl.tupleName();
  }
  
  static TUPExpression * parse(Parser& p, Context& ctx);
};

}

#endif /* BUILTIN_TUPLE_H_ */

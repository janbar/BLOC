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


#ifndef BUILTIN_CLAMP_H
#define BUILTIN_CLAMP_H

#include <blocc/expression_builtin.h>

namespace bloc
{

class Context;
class Parser;

class CLAMPExpression : public BuiltinExpression {

public:

  virtual ~CLAMPExpression() { }

  explicit CLAMPExpression(std::vector<Expression*>&& args) : BuiltinExpression(FUNC_CLAMP, std::move(args)) { }

  const Type& type(Context& ctx) const override { return _args[0]->type(ctx); }

  Value& value(Context& ctx) const override;

  static CLAMPExpression * parse(Parser& p, Context& ctx);
};

}

#endif /* BUILTIN_CLAMP_H */


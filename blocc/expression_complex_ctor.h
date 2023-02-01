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

#ifndef EXPRESSION_COMPLEX_CTOR_H
#define EXPRESSION_COMPLEX_CTOR_H

#include "expression.h"
#include "plugin_interface.h"

#include <vector>

namespace bloc
{

class Context;
class Parser;

/**
 * This class implements the expression that initialize a complex.
 */
class ComplexCTORExpression : public Expression
{
private:
  Type _type;
  const PLUGIN_CTOR * _ctor = nullptr;
  std::vector<Expression*> _args;

public:

  virtual ~ComplexCTORExpression();

  ComplexCTORExpression(unsigned type_id) : Expression(), _type(Type::COMPLEX, type_id) { }
  
  ComplexCTORExpression(unsigned type_id, const PLUGIN_CTOR& ctor, std::vector<Expression*>&& args)
  : Expression(), _type(Type::COMPLEX, type_id), _ctor(&ctor)
  {
    _args.swap(args);
  }

  const Type& type(Context& ctx) const override
  {
    return _type;
  }

  Complex& complex(Context& ctx) const override;

  std::string unparse(Context& ctx) const override;

  std::string toString(Context& ctx) const override
  {
    return "complex_ctor";
  }

  static ComplexCTORExpression * parse(Parser& p, Context& ctx, unsigned type_id);

};

}

#endif /* EXPRESSION_COMPLEX_CTOR_H */

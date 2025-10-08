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

#ifndef EXPRESSION_FUNCTOR_H
#define EXPRESSION_FUNCTOR_H

#include "expression.h"
#include "complex.h"
#include "functor_manager.h"
#include "token.h"

#include <vector>

namespace bloc
{

class Context;
class Parser;

/**
 * This class implements the call of functor.
 */
class FunctorExpression : public Expression
{
private:
  FunctorManager::entry _fentry;
  std::vector<Expression*> _args;

public:

  virtual ~FunctorExpression();

  FunctorExpression(const FunctorManager::entry& functor, std::vector<Expression*>&& args)
  : Expression(), _fentry(functor), _args(std::move(args)) { }

  const Type& type(Context& ctx) const override
  {
    return (*_fentry)->returns;
  }

  Value& value(Context& ctx) const override;

  std::string unparse(Context& ctx) const override;

  std::string toString(Context& ctx) const override
  {
    return "functor";
  }

  static FunctorExpression * parse(Parser& p, Context& ctx, TokenPtr& token);

};

}

#endif /* EXPRESSION_FUNCTOR_H */

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

#ifndef EXPRESSION_ITEM_H
#define EXPRESSION_ITEM_H

#include "expression.h"

#include <string>

namespace bloc
{

class Context;
class Parser;

class ItemExpression : public Expression
{

  Expression * _exp = nullptr;
  unsigned _index = 0;

public:

  constexpr static const char OPERATOR = '@';

  virtual ~ItemExpression();

  ItemExpression(Expression * e, unsigned index)
  : Expression(), _exp(e), _index(index) { }

  std::string unparse(Context& ctx) const override;

  const Type& type(Context& ctx) const override;

  bool boolean(Context& ctx) const override;

  int64_t integer(Context& ctx) const override;

  double numeric(Context& ctx) const override;

  bool isRvalue(Context& ctx) const override { return false; }

  std::string& literal(Context& ctx) const override;

  TabChar& tabchar(Context& ctx) const override;

  Complex& complex(Context& ctx) const override;

  std::string toString(Context& ctx) const override
  {
    return std::string(_exp->typeName(ctx))
            .append(1, OPERATOR)
            .append(std::to_string(_index + 1));
  }

  static ItemExpression * item(Parser& p, Context& ctx, Expression * exp);
};

}

#endif /* EXPRESSION_ITEM_H */

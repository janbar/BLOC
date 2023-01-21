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

#ifndef EXPRESSION_LITERAL_H
#define EXPRESSION_LITERAL_H

#include "expression_static.h"
#include "declspec.h"

namespace bloc
{

/**
 * This class implements the simplest possible expression, a literal value.
 */
class LiteralExpression : public StaticExpression
{
private:

  mutable std::string v;

public:
  LIBBLOC_API static const Type& type_static;

  virtual ~LiteralExpression() { }

  LiteralExpression() : StaticExpression() { }
  explicit LiteralExpression(const std::string& a) : StaticExpression(), v(a) { }
  explicit LiteralExpression(std::string &&a) : StaticExpression(), v(std::move(a)) { }

  const Type& type(Context& ctx) const override
  {
    return type_static;
  }

  std::string& literal(Context& ctx) const override
  {
    return v;
  }

  std::string unparse(Context& ctx) const override
  {
    return readableLiteral(v);
  }

  std::string toString(Context& ctx) const override
  {
    return std::string(typeName(ctx))
            .append(1, '[')
            .append(std::to_string(v.size()))
            .append(1, ']');
  }

  static LiteralExpression * parse(const std::string& text);

  static std::string readableLiteral(const std::string& s);

  const Type& refType() const override { return type_static; }

  std::string& refLiteral() override { return v; }

  void swap(StaticExpression& e) override
  {
    v.swap(e.refLiteral());
  }

  LiteralExpression * swapNew() override
  {
    return new LiteralExpression(std::move(this->v));
  }

  LiteralExpression * cloneNew() const override
  {
    return new LiteralExpression(this->v);
  }
};

}

#endif /* EXPRESSION_LITERAL_H */

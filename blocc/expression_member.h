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

#ifndef EXPRESSION_MEMBER_H
#define EXPRESSION_MEMBER_H

#include "expression.h"
#include "declspec.h"

#include <string>
#include <vector>

namespace bloc
{

class Context;
class Parser;

class MemberExpression : public Expression
{
protected:

  int _builtin = BTM_undefined;
  Expression * _exp = nullptr;
  std::vector<Expression*> _args;

public:

  constexpr static const char OPERATOR = '.';
  LIBBLOC_API static const char * KEYWORDS[];

  enum BTMETHOD {
    unknown = -1,
    BTM_undefined = 0,
    BTM_CONCAT    = 1,
    BTM_AT        = 2,
    BTM_PUT       = 3,
    BTM_COUNT     = 4,
    BTM_DELETE    = 5,
    BTM_INSERT    = 6,
    BTM_SET       = 7,
  };

  virtual ~MemberExpression();

  MemberExpression() : Expression() { }

  MemberExpression(BTMETHOD op, Expression * e)
  : Expression(), _builtin(op), _exp(e) { }

  MemberExpression(BTMETHOD op, Expression * e, std::vector<Expression*>&& args)
  : Expression(), _builtin(op), _exp(e), _args(std::move(args)) { }

  virtual std::string unparse(Context& ctx) const override;

  virtual std::string toString(Context& ctx) const override
  {
    return KEYWORDS[_builtin];
  }

  static int findBuiltinKeyword(const std::string& s);

  static MemberExpression * expression(Parser& p, Context& ctx, Expression * exp);

protected:
  static void assertClosedMember(Parser& p, Context& ctx, const char * member);

private:
  static MemberExpression * parse_builtin(Parser&p, Context& ctx, Expression * exp);
};

}

#endif /* EXPRESSION_MEMBER_H */

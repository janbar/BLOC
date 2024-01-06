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

#ifndef PARSE_EXPRESSION_H
#define PARSE_EXPRESSION_H

#include "expression.h"
#include "operator.h"

namespace bloc
{

class Parser;
class Context;

class ParseExpression {

public:

  /**
   * Parse input stream for a valid expression.
   * Throws exception ParseError.
   */
  static Expression * expression(Parser& p, Context& ctx);

  /**
   * Validate types mixing
   * @param exp   the expression to validate
   * @param type  the expected type
   * @param p     the parser
   * @param ctx   the context
   * @return      true if types mixing is supported else false
   */
  static bool typeChecking(Expression * exp, const Type& type, Parser& p, Context& ctx);
  /**
   * Throws ParseError when types mixing is not supported. Also the given
   * expression will be deleted before throwing if deleteOnFailure is set.
   * @param exp   the expression to validate
   * @param type  the expected type
   * @param p     the parser
   * @param ctx   the context
   * @param deleteOnFailure
   * @return      forward the validated expression
   */
  static Expression * assertType(Expression * exp, const Type& type, Parser& p, Context& ctx, bool deleteOnFailure = true);
  /**
   * Throws ParseError when types are not identical. Also the given expression
   * will be deleted before throwing if deleteOnFailure is set.
   * @param exp   the expression to validate
   * @param type  the expected type
   * @param p     the parser
   * @param ctx   the context
   * @param deleteOnFailure
   * @return      forward the validated expression
   */
  static Expression * assertTypeUniform(Expression * exp, const Type& type, Parser& p, Context& ctx, bool deleteOnFailure = true);

private:

  Parser& p;
  Context& ctx;
  
  ~ParseExpression() = default;
  ParseExpression(Parser& p, Context& ctx) : p(p), ctx(ctx) { }

  Expression * member(Expression * exp);

  Expression * element();

  Expression * primary();

  Expression * factor();

  Expression * term();

  Expression * sum();

  Expression * bitshift();

  Expression * bitlogic();

  Expression * relation();

  Expression * logic();
};

}

#endif /* PARSE_EXPRESSION_H */

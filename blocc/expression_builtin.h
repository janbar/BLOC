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

#ifndef EXPRESSION_BUILTIN_H_
#define EXPRESSION_BUILTIN_H_

#include "expression.h"
#include "declspec.h"

#include <string>
#include <vector>
#include <set>

namespace bloc
{

class Context;
class Parser;

/**
 * This class implements the builtin functions.
 * The tokenizer will scan the input for one of the strings in KEYWORDS[] and
 * if it finds it, it returns a token of FUNCTION with its value being that of
 * the index of the string into the array. (which are convientiently mapped
 * into named constants).
 *
 * Parsing of the arguments to the function are left up to the parse method
 * in this class.
 */
class BuiltinExpression : public Expression {

protected:
  int oper = FUNC_NIL;
  std::vector<Expression*> _args;

public:

  LIBBLOC_API static const char * KEYWORDS[];
  LIBBLOC_API static const char * HELPS[];

  enum FUNCTION {
    unknown = -1,
    FUNC_NIL    = 0, // null
    FUNC_MAX    = 1, // max(x,y)
    FUNC_MIN    = 2, // min(x,y)
    FUNC_FLOOR  = 3, // floor(x)
    FUNC_ABS    = 4, // abs(x)
    FUNC_SIGN   = 5, // sign(x)
    FUNC_STR    = 6, // str(x)
    FUNC_NUM    = 7, // num(x)
    FUNC_CEIL   = 8, // ceil(x)
    FUNC_ROUND  = 9, // round(x) | round(x,y)
    FUNC_SIN    = 10, // sin(x)
    FUNC_COS    = 11, // cos(x)
    FUNC_TAN    = 12, // tan(x)
    FUNC_ATAN   = 13, // atan(x)
    FUNC_INT    = 14, // int(x)
    FUNC_POW    = 15, // pow(x,y)
    FUNC_SQRT   = 16, // sqrt(x)
    FUNC_LOG    = 17, // log(x)
    FUNC_EXP    = 18, // exp(x)
    FUNC_LOG10  = 19, // log10(x)
    FUNC_MOD    = 20, // mod(x,y)
    FUNC_ASIN   = 21, // asin(x)
    FUNC_ACOS   = 22, // acos(x)
    FUNC_SINH   = 23, // sinh(x)
    FUNC_COSH   = 24, // cosh(x)
    FUNC_TANH   = 25, // tanh(x)

    FUNC_READ   = 30, // read(var,count)
    FUNC_READLN = 31, // readline(x)
    FUNC_ISNUM  = 32, // isnum(x)
    FUNC_RAW    = 33, // raw() | raw(x) | raw(x,y)
    FUNC_TAB    = 34, // tab(x) | tab(x,y)
    FUNC_TUPLE  = 35, // tuple(...)
    FUNC_GETSYS = 36, // getsys(x)
    FUNC_GETENV = 37, // getenv(x)
    FUNC_TRUE   = 38, // true
    FUNC_FALSE  = 39, // false
    FUNC_ERROR  = 40, // error
    FUNC_PHI    = 41, // phi (gold number)
    FUNC_PI     = 42, // pi
    FUNC_EE     = 43, // e (euler)
    FUNC_RANDOM = 44, // random | random(x)

    FUNC_LSUB   = 50, // lsubstr(x,count)
    FUNC_RSUB   = 51, // rsubstr(x,count)
    FUNC_SUBSTR = 52, // substr(x,begin) | substr(x,begin,count)
    FUNC_CHR    = 53, // chr(x)
    FUNC_STRLEN = 54, // strlen(x)
    FUNC_LTRIM  = 55, // ltrim(x)
    FUNC_RTRIM  = 56, // rtrim(x)
    FUNC_TRIM   = 57, // trim(x)
    FUNC_UPPER  = 58, // upper(x)
    FUNC_LOWER  = 59, // lower(x)
    FUNC_STRPOS = 60, // strpos(x,y)
    FUNC_REPSTR = 61, // replace(x,y,z)
    FUNC_SUBRAW = 62, // subraw(x,begin) | subraw(x,begin,count)
    FUNC_HASH   = 63, // hash(x) | hash(x,max)
  };

  bool isRvalue(Context& ctx) const override { return true; }

  std::string unparse(Context& ctx) const override;

  std::string toString(Context& ctx) const override
  {
    return KEYWORDS[oper];
  }

  static int findKeyword(const std::string& s);
  static std::set<std::string> keywordSet();

  /**
   * Parse a function argument. This code pulls off the '(' and ')' around the
   * arguments passed to the function and parses them.
   */
  static BuiltinExpression * expression(Parser& p, Context& ctx);

protected:
  virtual ~BuiltinExpression();
  BuiltinExpression() { }
  explicit BuiltinExpression(FUNCTION fc) : oper(fc) { }
  BuiltinExpression(FUNCTION fc, std::vector<Expression*>&& args) : oper(fc), _args(std::move(args)) { }
  static void assertClosedFunction(Parser& p, Context& ctx, FUNCTION fc);
};

}

#endif /* EXPRESSION_BUILTIN_H_ */

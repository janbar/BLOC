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

#include "expression_builtin.h"
#include "builtin/builtin_abs.h"
#include "builtin/builtin_acos.h"
#include "builtin/builtin_asin.h"
#include "builtin/builtin_atan.h"
#include "builtin/builtin_ceil.h"
#include "builtin/builtin_chr.h"
#include "builtin/builtin_cos.h"
#include "builtin/builtin_cosh.h"
#include "builtin/builtin_ee.h"
#include "builtin/builtin_error.h"
#include "builtin/builtin_exp.h"
#include "builtin/builtin_false.h"
#include "builtin/builtin_floor.h"
#include "builtin/builtin_getenv.h"
#include "builtin/builtin_getsys.h"
#include "builtin/builtin_hash.h"
#include "builtin/builtin_int.h"
#include "builtin/builtin_isnum.h"
#include "builtin/builtin_log.h"
#include "builtin/builtin_log10.h"
#include "builtin/builtin_lower.h"
#include "builtin/builtin_lsubstr.h"
#include "builtin/builtin_ltrim.h"
#include "builtin/builtin_max.h"
#include "builtin/builtin_min.h"
#include "builtin/builtin_mod.h"
#include "builtin/builtin_null.h"
#include "builtin/builtin_phi.h"
#include "builtin/builtin_pi.h"
#include "builtin/builtin_pow.h"
#include "builtin/builtin_random.h"
#include "builtin/builtin_raw.h"
#include "builtin/builtin_read.h"
#include "builtin/builtin_readln.h"
#include "builtin/builtin_replace.h"
#include "builtin/builtin_round.h"
#include "builtin/builtin_rsubstr.h"
#include "builtin/builtin_rtrim.h"
#include "builtin/builtin_sign.h"
#include "builtin/builtin_sin.h"
#include "builtin/builtin_sinh.h"
#include "builtin/builtin_sqrt.h"
#include "builtin/builtin_str.h"
#include "builtin/builtin_strlen.h"
#include "builtin/builtin_strpos.h"
#include "builtin/builtin_subraw.h"
#include "builtin/builtin_substr.h"
#include "builtin/builtin_tab.h"
#include "builtin/builtin_tan.h"
#include "builtin/builtin_tanh.h"
#include "builtin/builtin_trim.h"
#include "builtin/builtin_true.h"
#include "builtin/builtin_upper.h"
#include "builtin/builtin_num.h"
#include "builtin/builtin_tuple.h"

#include "exception_parse.h"
#include "context.h"
#include "parser.h"

#include <cstring>

namespace bloc
{

const char * BuiltinExpression::KEYWORDS[] = {
    "null",       "max",        "min",        "floor",      "abs",
    "sign",       "str",        "num",        "ceil",       "round",
    "sin",        "cos",        "tan",        "atan",       "int",
    "pow",        "sqrt",       "log",        "exp",        "log10",
    "mod",        "asin",       "acos",       "sinh",       "cosh",
    "tanh",       "",           "",           "",           "",
    "read",       "readln",     "isnum",      "raw",        "tab",
    "tuple",      "getsys",     "getenv",     "true",       "false",
    "error",      "phi",        "pi",         "ee",         "random",
    "",           "",           "",           "",           "",
    "lsubstr",    "rsubstr",    "substr",     "chr",        "strlen",
    "ltrim",      "rtrim",      "trim",       "upper",      "lower",
    "strpos",     "replace",    "subraw",     "hash",
};

BuiltinExpression::~BuiltinExpression()
{
  for (Expression * e : _args)
    delete e;
}

std::string BuiltinExpression::unparse(Context& ctx) const
{
  std::string sb;
  sb.append(KEYWORDS[oper]);
  switch (oper)
  {
  case FUNC_NIL:
  case FUNC_PHI:
  case FUNC_PI:
  case FUNC_EE:
  case FUNC_TRUE:
  case FUNC_FALSE:
  case FUNC_ERROR:
    break;
  default:
    sb.append("(");
    for (int i = 0; i < _args.size(); ++i)
    {
      if (i > 0)
        sb.append(1, Parser::CHAIN).append(1, ' ');
      sb.append(_args[i]->unparse(ctx));
    }
    sb.append(1, ')');
  }
  return sb;
}

int BuiltinExpression::findKeyword(const std::string& s)
{
  for (int i = 0; i < (sizeof (KEYWORDS) / sizeof (char*)); ++i)
  {
    if (strcmp(KEYWORDS[i], s.c_str()) == 0)
      return i;
  }
  return unknown;
}

std::set<std::string> BuiltinExpression::keywordSet()
{
  std::set<std::string> lst;
  for (int i = 0; i < (sizeof (KEYWORDS) / sizeof (char*)); ++i)
    if (KEYWORDS[i][0] != '\0')
      lst.insert(KEYWORDS[i]);
  return lst;
}

BuiltinExpression * BuiltinExpression::parse(Parser& p, Context& ctx)
{
  TokenPtr t = p.pop();
  FUNCTION fc = (FUNCTION)findKeyword(t->text);
  if (fc == unknown)
    throw ParseError(EXC_PARSE_NOT_A_FUNCTION);

  switch (fc)
  {
    /* constant */
  case FUNC_NIL:
    return new NULLExpression();
  case FUNC_TRUE:
    return new TRUEExpression();
  case FUNC_FALSE:
    return new FALSEExpression();
  case FUNC_PHI:
    return new PHIExpression();
  case FUNC_PI:
    return new PIExpression();
  case FUNC_EE:
    return new EEExpression();
  case FUNC_ERROR:
    return new ERRORExpression();

    /* 1 numeric */
  case FUNC_ABS:
    return ABSExpression::parse(p, ctx);
  case FUNC_SIN:
    return SINExpression::parse(p, ctx);
  case FUNC_COS:
    return COSExpression::parse(p, ctx);
  case FUNC_TAN:
    return TANExpression::parse(p, ctx);
  case FUNC_ATAN:
    return ATANExpression::parse(p, ctx);
  case FUNC_ASIN:
    return ASINExpression::parse(p, ctx);
  case FUNC_ACOS:
    return ACOSExpression::parse(p, ctx);
  case FUNC_SINH:
    return SINHExpression::parse(p, ctx);
  case FUNC_COSH:
    return COSHExpression::parse(p, ctx);
  case FUNC_TANH:
    return TANHExpression::parse(p, ctx);
  case FUNC_SQRT:
    return SQRTExpression::parse(p, ctx);
  case FUNC_EXP:
    return EXPExpression::parse(p, ctx);
  case FUNC_FLOOR:
    return FLOORExpression::parse(p, ctx);
  case FUNC_CEIL:
    return CEILExpression::parse(p, ctx);
  case FUNC_LOG:
    return LOGExpression::parse(p, ctx);
  case FUNC_LOG10:
    return LOG10Expression::parse(p, ctx);
  case FUNC_RANDOM:
    return RANDOMExpression::parse(p, ctx);
  case FUNC_SIGN:
    return SIGNExpression::parse(p, ctx);
  case FUNC_CHR:
    return CHRExpression::parse(p, ctx);

    /* 2 numeric */
  case FUNC_ROUND:
    return ROUNDExpression::parse(p, ctx);
  case FUNC_RAW:
    return RAWExpression::parse(p, ctx);
  case FUNC_MAX:
    return MAXExpression::parse(p, ctx);
  case FUNC_MIN:
    return MINExpression::parse(p, ctx);
  case FUNC_MOD:
    return MODExpression::parse(p, ctx);
  case FUNC_POW:
    return POWExpression::parse(p, ctx);

    /* 1 any type */
  case FUNC_STR:
    return STRExpression::parse(p, ctx);
  case FUNC_ISNUM:
    return ISNUMExpression::parse(p, ctx);
  case FUNC_NUM:
    return NUMExpression::parse(p, ctx);
  case FUNC_INT:
    return INTExpression::parse(p, ctx);
  case FUNC_HASH:
    return HASHExpression::parse(p, ctx);

    /* READ input */
  case FUNC_READLN:
    return READLNExpression::parse(p, ctx);
  case FUNC_READ:
    return READExpression::parse(p, ctx);

    /* 1 literal */
  case FUNC_STRLEN:
    return STRLENExpression::parse(p, ctx);
  case FUNC_LTRIM:
    return LTRIMExpression::parse(p, ctx);
  case FUNC_TRIM:
    return TRIMExpression::parse(p, ctx);
  case FUNC_RTRIM:
    return RTRIMExpression::parse(p, ctx);
  case FUNC_LOWER:
    return LOWERExpression::parse(p, ctx);
  case FUNC_UPPER:
    return UPPERExpression::parse(p, ctx);
  case FUNC_GETENV:
    return GETENVExpression::parse(p, ctx);
  case FUNC_GETSYS:
    return GETSYSExpression::parse(p, ctx);

    /* 1 literal, 1 numeric */
  case FUNC_LSUB:
    return LSUBSTRExpression::parse(p, ctx);
  case FUNC_RSUB:
    return RSUBSTRExpression::parse(p, ctx);

    /* others cases */
  case FUNC_SUBSTR:
    return SUBSTRExpression::parse(p, ctx);
  case FUNC_STRPOS:
    return STRPOSExpression::parse(p, ctx);
  case FUNC_REPSTR:
    return REPLACEExpression::parse(p, ctx);
  case FUNC_SUBRAW:
    return SUBRAWExpression::parse(p, ctx);
  case FUNC_TAB:
    return TABExpression::parse(p, ctx);
  case FUNC_TUPLE:
    return TUPLEExpression::parse(p, ctx);

  default:
    throw ParseError(EXC_PARSE_NOT_A_FUNCTION);
  }
  return nullptr;
}

void BuiltinExpression::assertClosedFunction(Parser& p, Context& ctx, FUNCTION fc)
{
  TokenPtr t = p.pop();
  /* check for enclosure */
  if (t->code != ')')
    throw ParseError(EXC_PARSE_FUNC_ARG_NUM_S, KEYWORDS[fc]);
}

const char * BuiltinExpression::HELPS[] = {
  /*NIL   */  "is the object null.",
  /*MAX   */  "returns the largest value among numbers x and y."
          "\n\nmax( x , y )",
  /*MIN   */  "returns the smallest value among numbers x and y."
          "\n\nmin( x , y )",
  /*FLOOR */  "returns the largest integer value less than or equal to x."
          "\n\nfloor( x )",
  /*ABS   */  "returns the absolute value of x."
          "\n\nabs( x )",
  /*SIGN  */  "returns the signed unity of x, i.e the number +1 or -1."
          "\n\nsign( x )",
  /*STR   */  "returns the string representation of x."
          "\n\nstr( [ x ] )"
          "\n\nString has the method at(), put(), insert(), delete(), count(),"
          "\nand concat({STRING | INTEGER}).",
  /*NUM   */  "returns the decimal value of x, else throw error."
          "\n\nnum( x )",
  /*CEIL  */  "returns the smallest integer value greater than or equal to x."
          "\n\nceil( x )",
  /*ROUND */  "returns a value to the nearest integer, or round to the decimal place y."
          "\n\nround( x [, y] )",
  /*SIN   */  "returns the sine of a radian angle x."
          "\n\nsin( x )",
  /*COS   */  "returns the cosine of a radian angle x."
          "\n\ncos( x )",
  /*TAN   */  "returns the tangent of a radian angle x."
          "\n\ntan( x )",
  /*ATAN  */  "returns the arc tangent of x in radians."
          "\n\natan( x )",
  /*INT   */  "returns the integer value of x, else throw error."
          "\n\nint( x )",
  /*POW*/     "returns x raised to the power of y."
          "\n\npow( x , y )",
  /*SQTR  */  "returns the square root of x."
          "\n\nsqrt( x )",
  /*LOG   */  "returns the natural logarithm (base-e logarithm) of x."
          "\n\nlog( x )",
  /*EXP   */  "returns the value of e raised to the xth power."
          "\n\nexp( x )",
  /*LOG10 */  "returns the common logarithm (base-10 logarithm) of x."
          "\n\nlog10( x )",
  /*MOD   */  "returns the remainder of x divided by y."
          "\n\nmod( x , y )",
  /*ASIN  */  "returns the arc sine of x in radians."
          "\n\nasin( x )",
  /*ACOS  */  "returns the arc cosine of x in radians."
          "\n\nacos( x )",
  /*SINH  */  "returns the hyperbolic sine of x."
          "\n\nsinh( x )",
  /*COSH  */  "returns the hyperbolic cos of x."
          "\n\ncosh( x )",
  /*TANH  */  "returns the hyperbolic tangent of x."
          "\n\ntanh( x )",
  "", "", "", "",
  /*READ  */  "returns the count of characters read from the input."
          "\nThe read value, including newline terminator, is stored into var with"
          "\na maximum of y characters. The 'string' variable var must be initialized"
          "\nfirst."
          "\n\nread( var [, y] )",
  /*READLN*/  "returns the literal value read from the input."
          "\nThe value does not include the newline terminator. Optionally x contains"
          "\nthe desired prompt."
          "\n\nreadln( [x] )",
  /*ISNUM */  "returns true if x can be converted to number."
          "\n\nisnum( x )",
  /*RAW   */  "returns a new empty bytes array, or filled with string x, or filled with"
          "\nx byte y."
          "\n\nraw( [ x [, y] ] )"
          "\n\nBytes array has the method at(), put(), insert(), delete(), count(),"
          "\nand concat({BYTES | INTEGER}).",
  /*TAB   */  "returns a new uniform table filled with x element(s) y."
          "\n\ntab( [ x , y] )"
          "\n\nNested element can be any type, or tuple. Nesting level is supported up"
          "\nto 254 dimmensions. Table has the methods at(), put(), insert(), delete(),"
          "\ncount(), and concat({TABLE | ELEMENT}).",
  /*TUPLE */  "returns a new tuple of 2 or more items."
          "\n\ntuple( x , y [, ...] )"
          "\n\nItem can be boolean, integer, decimal, string, object, or bytes array."
          "\nNesting and table are not allowed. Tuple is unchangeable, meaning that we"
          "\ncannot change type, add or remove items after the tuple has been created."
          "\nThe operator '@#' accesses the item at position # [1..n], as shown below."
          "\nExpression {TUPLE}@1 returns the value of item at position 1."
          "\nTuple has the method set@#(x) to assign the value x to the item of rank #.",
  /*GETSYS*/  "returns the corresponding value string of context environment variable x."
          "\n\ngetsys( x ) , where x could be 'compatible', 'language', 'country',"
          "\n'integer_max', or 'last_error'.",
  /*GETENV*/  "returns the corresponding value string of the environment variable x."
          "\n\ngetenv( x )",
  /*TRUE  */  "is the boolean expression 'true'",
  /*FALSE */  "is the boolean expression 'false'",
  /*ERROR */  "is the current thrown error as tuple { string(name), string(message), integer(code) }.",
  /*PHI   */  "is the golden ratio.",
  /*PI    */  "is the number PI.",
  /*EE    */  "is the Euler's number",
  /*RANDOM*/  "returns pseudo-random number that's greater than or equal to 0 and less"
          "\nthan 1 or optionally x."
          "\n\nrandom( [x] )",
  "", "", "", "", "",
  /*LSUB  */  "returns left part of string x."
          "\n\nlsubstr( x , count )",
  /*RSUB  */  "returns right part of string x."
          "\n\nrsubstr( x , count )",
  /*SUBSTR*/  "returns a part of string x."
          "\n\nsubstr( x , begin [, count] )",
  /*CHR   */  "returns a character from the specified ASCII value x."
          "\n\nchr( x )",
  /*STRLEN*/  "returns the size of string x."
          "\n\nstrlen( x )",
  /*LTRIM */  "Remove whitespace from the left side of string x."
          "\n\nltrim( x )",
  /*RTRIM */  "Remove whitespace from the right side of string x."
          "\n\nrtrim( x )",
  /*TRIM  */  "Remove whitespace from the left and right side of string x."
          "\n\ntrim( x )",
  /*UPPER */  "convert any lowercase letters in string x to uppercase letters."
          "\n\nupper( x )",
  /*LOWER */  "convert any uppercase letters in string x to lowercase letters."
          "\n\nlower( x )",
  /*STRPOS*/  "finds the position of the first occurrence of the string y inside the"
          "\nstring x."
          "\n\nstrpos( x , y )",
  /*REPSTR*/  "returns a string from x with all occurences of y replaced by z."
          "\n\nreplace( x , y , z)",
  /*SUBRAW*/  "returns a part of bytes array x."
          "\n\nsubraw( x , begin [, count] )",
  /*HASH  */  "returns the DJB Hash value (32 bits) of string or bytes array."
          "\nOptionally the number of buckets is specified by y [1..n]."
          "\n\nhash( x [, y ] )",
};

}

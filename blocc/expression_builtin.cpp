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
#include "builtin/builtin_bool.h"
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
#include "builtin/builtin_isnull.h"
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
#include "builtin/builtin_num.h"
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
#include "builtin/builtin_tup.h"
#include "builtin/builtin_clamp.h"
#include "builtin/builtin_ii.h"
#include "builtin/builtin_imag.h"
#include "builtin/builtin_iphase.h"
#include "builtin/builtin_iconj.h"
#include "builtin/builtin_input.h"
#include "builtin/builtin_atan2.h"
#include "builtin/builtin_tokenize.h"

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
    "tanh",       "clamp",      "isnull",     "atan2",      "",
    "read",       "readln",     "isnum",      "raw",        "tab",
    "tup",        "getsys",     "getenv",     "true",       "on",
    "false",      "off",        "error",      "phi",        "pi",
    "ee",         "random",     "bool",       "ii",         "input",
    "lsubstr",    "rsubstr",    "substr",     "chr",        "strlen",
    "ltrim",      "rtrim",      "trim",       "upper",      "lower",
    "strpos",     "replace",    "subraw",     "hash",       "imag",
    "iphase",     "iconj",      "tokenize",
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
  case FUNC_II:
  case FUNC_TRUE:
  case FUNC_ON:
  case FUNC_FALSE:
  case FUNC_OFF:
  case FUNC_ERROR:
    break;
  default:
    sb.append("(");
    for (int i = 0; i < _args.size(); ++i)
    {
      if (i > 0)
        sb.append(1, Parser::Chain).append(1, ' ');
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
  case FUNC_NIL:
    return new NULLExpression();
  case FUNC_MAX:
    return MAXExpression::parse(p, ctx);
  case FUNC_MIN:
    return MINExpression::parse(p, ctx);
  case FUNC_FLOOR:
    return FLOORExpression::parse(p, ctx);
  case FUNC_ABS:
    return ABSExpression::parse(p, ctx);
  case FUNC_SIGN:
    return SIGNExpression::parse(p, ctx);
  case FUNC_STR:
    return STRExpression::parse(p, ctx);
  case FUNC_NUM:
    return NUMExpression::parse(p, ctx);
  case FUNC_CEIL:
    return CEILExpression::parse(p, ctx);
  case FUNC_ROUND:
    return ROUNDExpression::parse(p, ctx);
  case FUNC_SIN:
    return SINExpression::parse(p, ctx);
  case FUNC_COS:
    return COSExpression::parse(p, ctx);
  case FUNC_TAN:
    return TANExpression::parse(p, ctx);
  case FUNC_ATAN:
    return ATANExpression::parse(p, ctx);
  case FUNC_INT:
    return INTExpression::parse(p, ctx);
  case FUNC_POW:
    return POWExpression::parse(p, ctx);
  case FUNC_SQRT:
    return SQRTExpression::parse(p, ctx);
  case FUNC_LOG:
    return LOGExpression::parse(p, ctx);
  case FUNC_EXP:
    return EXPExpression::parse(p, ctx);
  case FUNC_LOG10:
    return LOG10Expression::parse(p, ctx);
  case FUNC_MOD:
    return MODExpression::parse(p, ctx);
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
  case FUNC_CLAMP:
    return CLAMPExpression::parse(p, ctx);
  case FUNC_ISNIL:
    return ISNULLExpression::parse(p, ctx);
  case FUNC_ATAN2:
    return ATAN2Expression::parse(p, ctx);
  case FUNC_READ:
    return READExpression::parse(p, ctx);
  case FUNC_READLN:
    return READLNExpression::parse(p, ctx);
  case FUNC_ISNUM:
    return ISNUMExpression::parse(p, ctx);
  case FUNC_RAW:
    return RAWExpression::parse(p, ctx);
  case FUNC_TAB:
    return TABExpression::parse(p, ctx);
  case FUNC_TUP:
    return TUPExpression::parse(p, ctx);
  case FUNC_GETSYS:
    return GETSYSExpression::parse(p, ctx);
  case FUNC_GETENV:
    return GETENVExpression::parse(p, ctx);
  case FUNC_TRUE:
  case FUNC_ON:
    return new TRUEExpression();
  case FUNC_FALSE:
  case FUNC_OFF:
    return new FALSEExpression();
  case FUNC_ERROR:
    return new ERRORExpression();
  case FUNC_PHI:
    return new PHIExpression();
  case FUNC_PI:
    return new PIExpression();
  case FUNC_EE:
    return new EEExpression();
  case FUNC_RANDOM:
    return RANDOMExpression::parse(p, ctx);
  case FUNC_BOOL:
    return BOOLExpression::parse(p, ctx);
  case FUNC_II:
    return new IIExpression();
  case FUNC_INPUT:
    return INPUTExpression::parse(p, ctx);
  case FUNC_LSUB:
    return LSUBSTRExpression::parse(p, ctx);
  case FUNC_RSUB:
    return RSUBSTRExpression::parse(p, ctx);
  case FUNC_SUBSTR:
    return SUBSTRExpression::parse(p, ctx);
  case FUNC_CHR:
    return CHRExpression::parse(p, ctx);
  case FUNC_STRLEN:
    return STRLENExpression::parse(p, ctx);
  case FUNC_LTRIM:
    return LTRIMExpression::parse(p, ctx);
  case FUNC_RTRIM:
    return RTRIMExpression::parse(p, ctx);
  case FUNC_TRIM:
    return TRIMExpression::parse(p, ctx);
  case FUNC_UPPER:
    return UPPERExpression::parse(p, ctx);
  case FUNC_LOWER:
    return LOWERExpression::parse(p, ctx);
  case FUNC_STRPOS:
    return STRPOSExpression::parse(p, ctx);
  case FUNC_REPSTR:
    return REPLACEExpression::parse(p, ctx);
  case FUNC_SUBRAW:
    return SUBRAWExpression::parse(p, ctx);
  case FUNC_HASH:
    return HASHExpression::parse(p, ctx);
  case FUNC_IMAG:
    return IMAGExpression::parse(p, ctx);
  case FUNC_IPHASE:
    return IPHASEExpression::parse(p, ctx);
  case FUNC_ICONJ:
    return ICONJExpression::parse(p, ctx);
  case FUNC_TKNIZE:
    return TOKENIZEExpression::parse(p, ctx);

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
  /*NIL   */  "is the typeless null value. You can use the builtin isnull( x ) to check"
          "\nwhether a value is null or not."
          "\n\nSee bool(), int(), num(), str(), raw(), tup() or tab() to provide a type"
          "\nqualified null value.",
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
  /*STR   */  "returns the string representation of x, or the null string."
          "\n\nstr( [ x ] )"
          "\n\nString has the method at(), put(), insert(), delete(), count(),"
          "\nand concat({STRING | INTEGER}).",
  /*NUM   */  "returns the decimal value of x else throw error, or the null decimal."
          "\n\nnum( [ x ] )",
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
  /*INT   */  "returns the integer value of x else throw error, or the null integer."
          "\n\nint( [ x ] )",
  /*POW*/     "returns x raised to the power of y."
          "\n\npow( x , y )",
  /*SQRT  */  "returns the square root of x."
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
  /*CLAMP */  "restricts the given value x between the lower bound y and upper bound z."
          "\nIn this way, it acts like a combination of the min() and max() functions."
          "\n\nclamp( x , y , z )",
  /*ISNIL*/  "returns true if x is null."
          "\n\nisnull( x )",
  /*ATAN2*/  "returns the angle in radians for the quadrant y , x."
          "\n\natan2( y , x )",
  "",
  /*READ  */  "returns the count of characters read from the input."
          "\nThe read value, including newline terminator, is stored into var with"
          "\na maximum of y characters. The 'string' variable var must be initialized"
          "\nfirst."
          "\n\nread( var [, y] )",
  /*READLN*/  "returns true when new line has been read from the input."
          "\nThe read value ended with LF or limited to the size of the internal buffer,"
          "\nis stored into var. The 'string' variable var must be initialized first."
          "\n\nreadln( var )",
  /*ISNUM */  "returns true if x can be converted to number."
          "\n\nisnum( x )",
  /*RAW   */  "returns a new bytes array filled with string x, or filled with"
          "\nx byte y, or the null bytes array."
          "\n\nraw( [ x [, y] ] )"
          "\n\nBytes array has the method at(), put(), insert(), delete(), count(),"
          "\nand concat({BYTES | INTEGER}).",
  /*TAB   */  "returns a new uniform table filled with x element(s) y, or the null table."
          "\n\ntab( [ x , y ] )"
          "\n\nNested element can be any type, or tuple. Nesting level is supported up"
          "\nto 254 dimmensions. Table has the methods at(), put(), insert(), delete(),"
          "\ncount(), and concat({TABLE | ELEMENT}).",
  /*TUP   */  "returns a new tuple of items, or the null tuple."
          "\n\ntup( [ x [, ...] ] )"
          "\n\nItem can be boolean, integer, decimal, string, object, or bytes array."
          "\nNesting and table are not allowed. Tuple is unchangeable, meaning that we"
          "\ncannot change type, add or remove items after the tuple has been created."
          "\nThe operator '@#' accesses the item at position # [1..n], as shown below."
          "\nExpression {TUPLE}@1 returns the value of item at position 1."
          "\nTuple has the method set@#(x) to assign the value x to the item of rank #."
          "\nItem type must be qualified. Therefore to assign null you have to use one"
          "\nof builtin function providing the null type qualified, i.e bool(), int(),"
          "\nnum(), str() or raw().",
  /*GETSYS*/  "returns the corresponding value of context environment variable x."
          "\n\ngetsys( x ) , where x could be 'compatible', 'language', 'country',"
          "\n'integer_max', or 'integer_min'."
          "\n\nThe type of value depends of the selected variable.",
  /*GETENV*/  "returns the corresponding value string of the environment variable x."
          "\n\ngetenv( x )",
  /*TRUE  */  "is the boolean expression 'true'",
  /*ON    */  "is an alias for the boolean expression 'true'",
  /*FALSE */  "is the boolean expression 'false'",
  /*OFF   */  "is an alias for the boolean expression 'false'",
  /*ERROR */  "is the current thrown error as tuple { string(name), string(message), integer(code) }.",
  /*PHI   */  "is the golden ratio.",
  /*PI    */  "is the number PI.",
  /*EE    */  "is the Euler's number",
  /*RANDOM*/  "returns pseudo-random number that's greater than or equal to 0 and less"
          "\nthan 1 or optionally x."
          "\n\nrandom( [x] )",
  /*BOOL  */  "returns the boolean representation of numeric x, or the null boolean."
          "\n\nbool( [ x ] )"
          "\n\nWhen x is 0 the boolean false is returned, else true.",
  /*II    */  "is the imaginary number.",
  /*INPUT*/   "returns true when new data has been read from the input."
          "\nThe read value without newline terminator, is stored into var. The 'string'"
          "\nvariable var must be initialized first. Optionally y contains the desired"
          "\nprompt."
          "\n\ninput( var [, y] )",
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
  /*STRPOS*/  "finds the position of the first occurrence of the string y starting"
          "\nat position z, inside the string x. If none then null is returned."
          "\n\nstrpos( x , y [, z] )"
          "\n\nSee builtin isnull.",
  /*REPSTR*/  "returns a string from x with all occurences of y replaced by z."
          "\n\nreplace( x , y , z)",
  /*SUBRAW*/  "returns a part of bytes array x."
          "\n\nsubraw( x , begin [, count] )",
  /*HASH  */  "returns the DJB Hash value (32 bits) of string or bytes array."
          "\nOptionally the number of buckets is specified by y [1..n]."
          "\n\nhash( x [, y] )",
  /*IMAG  */ "returns the modulus or magnitude of the given complex."
          "\n\nimag( x )",
  /*IPHASE*/  "returns theta or the phase of the given complex."
          "\n\niphase( x )",
  /*ICONJ */  "returns the complex as the conjugate of the given complex."
          "\n\niconj( x )",
  /*TKNIZE */ "splits the values in the string x, using the string y as separator."
          "\nIf the boolean z is true, then null values will be trimmed."
          "\n\ntokenize( x , y [, z] )",
};

}

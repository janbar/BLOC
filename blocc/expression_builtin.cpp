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
#include "builtin/builtin_hex.h"
#include "builtin/builtin_b64enc.h"
#include "builtin/builtin_b64dec.h"

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
    "tanh",       "clamp",      "isnull",     "atan2",      "hex",
    "read",       "readln",     "isnum",      "raw",        "tab",
    "tup",        "getsys",     "getenv",     "true",       "on",
    "false",      "off",        "error",      "phi",        "pi",
    "ee",         "random",     "bool",       "ii",         "input",
    "lsubstr",    "rsubstr",    "substr",     "chr",        "strlen",
    "ltrim",      "rtrim",      "trim",       "upper",      "lower",
    "strpos",     "replace",    "subraw",     "hash",       "imag",
    "iphase",     "iconj",      "tokenize",   "b64enc",     "b64dec",
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
    throw ParseError(EXC_PARSE_NOT_A_FUNCTION, t);

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
  case FUNC_HEX:
    return HEXExpression::parse(p, ctx);
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
  case FUNC_B64ENC:
    return B64ENCExpression::parse(p, ctx);
  case FUNC_B64DEC:
    return B64DECExpression::parse(p, ctx);

  default:
    throw ParseError(EXC_PARSE_NOT_A_FUNCTION, t);
  }
  return nullptr;
}

void BuiltinExpression::assertClosedFunction(Parser& p, Context& ctx, FUNCTION fc)
{
  TokenPtr t = p.pop();
  /* check for enclosure */
  if (t->code != ')')
    throw ParseError(EXC_PARSE_FUNC_ARG_NUM_S, KEYWORDS[fc], t);
}

}

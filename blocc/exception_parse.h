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

#ifndef EXCEPTION_PARSE_H_
#define EXCEPTION_PARSE_H_

#include "exception.h"
#include "declspec.h"

namespace bloc
{

enum EXC_PARSE
{
  EXC_PARSE_EOF                   = 0,
  EXC_PARSE_INV_EXPRESSION        = 1,
  EXC_PARSE_UNDEFINED_SYMBOL_S,
  EXC_PARSE_NOT_NUMERIC,
  EXC_PARSE_NOT_INTEGER,
  EXC_PARSE_NOT_BOOLEAN,
  EXC_PARSE_NOT_COMPLEX,
  EXC_PARSE_NOT_LITERAL,
  EXC_PARSE_NOT_TABCHAR,
  EXC_PARSE_NOT_COLLECT,
  EXC_PARSE_NOT_ROWTYPE,
  EXC_PARSE_TYPE_MISMATCH_S,
  EXC_PARSE_OUT_OF_DIMENSION,
  EXC_PARSE_OUT_OF_INDICE,
  EXC_PARSE_OUT_OF_RANGE,
  EXC_PARSE_FUNC_ARG_TYPE_S,
  EXC_PARSE_FUNC_ARG_NUM_S,
  EXC_PARSE_MEMB_NOT_IMPL_S,
  EXC_PARSE_MEMB_ARG_TYPE_S,
  EXC_PARSE_MEMB_ARG_NUM_S,
  EXC_PARSE_BAD_MEMB_CALL_S,
  EXC_PARSE_NOT_A_STATEMENT,
  EXC_PARSE_NOT_A_FUNCTION,
  EXC_PARSE_UNEXPECTED_LEX_S,
  EXC_PARSE_MM_PARENTHESIS,
  EXC_PARSE_STATEMENT_END_S,
  EXC_PARSE_EXPRESSION_END_S,
  EXC_PARSE_MISSING_KEYWORD_S,
  EXC_PARSE_RESERVED_WORD_S,
  EXC_PARSE_IMPORT_FAILED_S,
  EXC_PARSE_OPAQUE_ROWTYPE,
  EXC_PARSE_OTHER_S,
};

class ParseError : public Error
{
  public:
    LIBBLOC_API static const char * PARSE_ERROR[];

    ParseError() : Error(PARSE_ERROR[EXC_PARSE_EOF]), no(EXC_PARSE_EOF) { }
    explicit ParseError(EXC_PARSE no) : Error(PARSE_ERROR[no]), no(no) { }
    ParseError(EXC_PARSE no, const char * arg) : Error(PARSE_ERROR[no], arg), no(no) { }
    const EXC_PARSE no;
};

}

#endif /* EXCEPTION_PARSE_H_ */


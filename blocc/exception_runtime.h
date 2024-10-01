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

#ifndef EXCEPTION_RUNTIME_H_
#define EXCEPTION_RUNTIME_H_

#include "exception.h"
#include "declspec.h"

#include <string>

namespace bloc
{

enum EXC_RT
{
  EXC_RT_NOERROR              = 0,
  EXC_RT_USER_S               = 1,
  EXC_RT_OTHER_S,
  EXC_RT_INTERNAL_ERROR_S,
  EXC_RT_NOT_IMPLEMENTED,
  EXC_RT_INV_EXPRESSION,
  EXC_RT_UNDEFINED_SYMBOL_S,
  EXC_RT_NOT_NUMERIC,
  EXC_RT_NOT_INTEGER,
  EXC_RT_NOT_BOOLEAN,
  EXC_RT_NOT_COMPLEX,
  EXC_RT_NOT_IMAGINARY,
  EXC_RT_NOT_LITERAL,
  EXC_RT_NOT_TABCHAR,
  EXC_RT_NOT_COLLECT,
  EXC_RT_NOT_ROWTYPE,
  EXC_RT_NOT_POINTER,
  EXC_RT_CTOR_FAILED_S,
  EXC_RT_MEMB_FAILED_S,
  EXC_RT_BAD_COMPLEX_S,
  EXC_RT_OUT_OF_DIMENSION,
  EXC_RT_OUT_OF_RANGE,
  EXC_RT_INDEX_RANGE_S,
  EXC_RT_DIVIDE_BY_ZERO,
  EXC_RT_STRING_TO_NUM,
  EXC_RT_TYPE_MISMATCH_S,
  EXC_RT_FUNC_ARG_TYPE_S,
  EXC_RT_MEMB_ARG_TYPE_S,
  EXC_RT_MEMB_NOT_IMPL_S,
  EXC_RT_COMPOUND_OPAQUE,
  EXC_RT_NO_RETURN_VALUE,
  EXC_RT_RECURSION_LIMIT,
  EXC_RT_VARYING_COLLECTION,
};

class RuntimeError : public Error
{
  public:
    LIBBLOC_API static const char * RUNTIME_ERROR[];

    RuntimeError() : Error(), no(EXC_RT_NOERROR) { }
    explicit RuntimeError(EXC_RT no) : Error(RUNTIME_ERROR[no]), no(no) { }
    RuntimeError(EXC_RT no, const char * arg) : Error(RUNTIME_ERROR[no], arg), no(no) { }

    EXC_RT no;

    typedef struct { EXC_RT no; const char * keyword; } THROWABLE;
    LIBBLOC_API static THROWABLE THROWABLES[];
    static EXC_RT findThrowable(const std::string& keyword);
    static unsigned throwable(EXC_RT no);
};


}

#endif /* EXCEPTION_RUNTIME_H_ */


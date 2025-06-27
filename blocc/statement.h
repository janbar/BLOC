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

#ifndef STATEMENT_H_
#define STATEMENT_H_

#include "exception.h"
#include "intrinsic_type.h"
#include "declspec.h"

#include <string>
#include <list>
#include <set>

namespace bloc
{

class Context;

class Statement {

public:

  LIBBLOC_API static const char * KEYWORDS[];
  LIBBLOC_API static const char * HELPS[];

  /**
   * This constants should match the indexes of the above KEYWORDS.
   */
  enum STATEMENT {
    unknown         = -1,
    STMT_NOP        = 0, // null statement
    STMT_TRACE      = 1,
    STMT_LET        = 2,
    STMT_IMPORT     = 3,
    STMT_FUNCTION   = 4,
    STMT_IF         = 5,
    STMT_THEN       = 6,
    STMT_ELSE       = 7,
    STMT_ELSIF      = 8,
    STMT_FOR        = 9,
    STMT_WHILE      = 10,
    STMT_LOOP       = 11,
    STMT_IN         = 12,
    STMT_TO         = 13,
    STMT_RETURN     = 14,
    STMT_BEGIN      = 15,
    STMT_BREAK      = 16,
    STMT_CONTINUE   = 17,
    STMT_END        = 18,
    STMT_ENDIF      = 19,
    STMT_ENDLOOP    = 20,
    STMT_PRINT      = 21,
    STMT_PUT        = 22,
    STMT_DO         = 23,
    STMT_EXCEPTION  = 24,
    STMT_WHEN       = 25,
    STMT_RAISE      = 26,
    STMT_ASC        = 27,
    STMT_DESC       = 28,
    STMT_IS         = 29,
    STMT_FORALL     = 30,
    STMT_INCLUDE    = 31,
  };

  virtual ~Statement();

  /**
   * Construct a new statement object with a valid key.
   */
  explicit Statement(STATEMENT keyword) : _keyword(keyword) { }

  /**
   * This method does the actual statement execution. It works by calling the
   * abstract function 'doit' which is defined in each statement subclass. The
   * runtime error (if any) is caught so that the line number and statement can
   * be attached to the result and then it is re-thrown.
   */
  const Statement * execute(Context& ctx) const;

  /**
   * This method "runs" this statement and returns a reference on
   * the next statement to run or null if there is no control flow.
   *
   * @throws RuntimeError if there is a problem during statement
   * execution such as divide by zero etc.
   */
  virtual const Statement * doit(Context& ctx) const { return nullptr; }

  /**
   * Return a string representation of this statement.
   */
  std::string toString(Context& ctx) const
  {
    return KEYWORDS[_keyword];
  }

  /**
   * Reconstruct the statement from the parse tree, this is most useful for
   * diagnosing parsing problems.
   */
  virtual void unparse(Context& ctx, FILE * out) const
  {
    fputs(Statement::KEYWORDS[keyword()], out);
    unparse_next(ctx, out);
  }

  /**
   * The trace method can be used during execution to print out what
   * the program is doing.
   */
  void trace_pre(Context& ctx) const;
  void trace_post(Context& ctx) const;

  int keyword() const { return _keyword; }
  Statement * next() const { return _next; }
  size_t level() const { return _level; }

  void setNext(Statement * s) { _next = s; }

  static int findKeyword(const std::string& s);
  static std::set<std::string> keywordSet();

protected:

  Statement * _next   = nullptr;
  STATEMENT _keyword  = STMT_NOP;
  mutable size_t _level  = 0;

  void unparse_next(Context& ctx, FILE * out) const;

private:
  void delete_next();
};

}

#endif /* STATEMENT_H_ */


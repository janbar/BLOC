/*
 * statement.cpp
 *
 *  Created on: 9 déc. 2022
 *      Author: jlb
 */

#include "statement.h"
#include "parser.h"

#include <cstring>

namespace bloc
{

/**
 * These are all of the statement KEYWORDS we can parse.
 */
const char * Statement::KEYWORDS[] = {
  "nop",      "trace",    "let",      "import",   "function",
  "if",       "then",     "else",     "elsif",    "for",
  "while",    "loop",     "in",       "to",       "return",
  "begin",    "break",    "continue", "end",      "end if",
  "end loop", "print",    "put",      "do",       "exception",
  "when",     "raise",    "asc",      "desc",     "is",
  "returns",
};

Statement::~Statement()
{
  delete_next();
}

const Statement *Statement::execute(Context& ctx) const
{
  bool trace = ctx.trace();
  _level = ctx.execLevel();
  if (trace) trace_pre(ctx);
  const Statement * next = doit(ctx);
  if (trace) trace_post(ctx);
  ctx.onStatementEnd(this);
  return next;
}

void Statement::delete_next()
{
  /* delete next chain */
  if (_next)
  {
    _next->delete_next();
    delete _next;
    _next = nullptr;
  }
}

void Statement::unparse_next(Context& ctx, FILE * out) const
{
  if (_next)
  {
    fputc(' ', out);
    fputc(Parser::CHAIN, out);
    fputc(' ', out);
    _next->unparse(ctx, out);
  }
}

void Statement::trace_pre(Context& ctx) const
{
  fprintf(ctx.ctxerr(), "%012.6f: %s\n", ctx.timestamp(), KEYWORDS[_keyword]);
  fflush(ctx.ctxerr());
}

void Statement::trace_post(Context& ctx) const
{
  size_t count = ctx.allocationCount();
  fprintf(ctx.ctxerr(), "%012.6f: %s : alloc_count=%u bcr=%d%d%d\n",
          ctx.timestamp(), KEYWORDS[_keyword], (unsigned)count,
          ctx.breakCondition(), ctx.continueCondition(), ctx.returnCondition());
  fflush(ctx.ctxerr());
}

int Statement::findKeyword(const std::string& s)
{
  for (size_t i = 0; i < (sizeof (KEYWORDS) / sizeof (char*)); ++i)
  {
    if (strcmp(KEYWORDS[i], s.c_str()) == 0)
      return i;
  }
  return unknown;
}

std::set<std::string> Statement::keywordSet()
{
  std::set<std::string> lst;
  for (int i = 0; i < (sizeof (KEYWORDS) / sizeof (char*)); ++i)
    if (KEYWORDS[i][0] != '\0')
      lst.insert(KEYWORDS[i]);
  return lst;
}

const char * Statement::HELPS[] = {
  /*NOP   */  "It does nothing.",
  /*TRACE */  "The TRACE statement enable or disable trace verbosity."
          "\n\ntrace {boolean expression};",
  /*LET   */  "The LET statement is the standard variable declaration and assignment."
  "\n\nlet {var} = {expression} [, var = ...];\n"
  "\nHowever we allow you to omit the LET or simply use:"
  "\n{var} = {expression} [, var = ...];",
  /*IMPORT*/  "The IMPORT statement is the directive to load an external module."
          "\nExternal module provides new type with features (object). Loading is"
          "\nperformed immediately, so the new type can be used now."
          "\n\nimport {module name};"
          "\nimport {encapsulated string containing the library file path};"
          "\n\nUsing module name will try to load the library 'libbloc_{name}.so'"
          "\non UNIX, and 'bloc_{name}.dll' on WINDOWS.",
  /*FUNCTION*/  "The FUNCTION statement declares a block of code which only runs"
          "\nwhen it is called, and should return data as a result. You can pass"
          "\ndata, known as parameters, into a function."
          "\n\nfunction {name} [( x [, y ...] )] returns {type name} is"
          "\nbegin [statement ...]"
          "\n[ exception [ when {exception name} then [statement ...] ...] ]"
          "\nend;"
          "\n\nThe returned type can be boolean, integer, decimal, string, bytes,"
          "\ntuple, table, or an imported object. Tuple and table types are opaque,"
          "\nso the result must be stored in a variable before accessing an item."
          "\nSee statement BEGIN.",
  /*IF    */  "The IF statement provides conditional control flow in execution."
          "\nIt has a boolean expression associated with it that is evaluated at runtime."
          "\nIf the result of that evaluation is a 'true' value, then statements beyond"
          "\nthe THEN keyword will be executed until end delimited by ELSIF or ELSE or"
          "\nEND IF."
          "\n\nif {boolean expression} then [statement ...]"
          "\n[ elsif {boolean expression} then [statement ...] ...]"
          "\n[ else [statement ...] ]"
          "\nend if;",
  /*THEN  */  "See statement IF, EXCEPTION.",
  /*ELSE  */  "See statement IF.",
  /*ELSIF */  "See statement IF.",
  /*FOR   */  "The FOR statement provides iterator construct. It defines a control variable"
          "\nthat is initialized to an initial value. Then block of statements after the"
          "\nLOOP keyword will be executed while the specified limit isn't reached. The"
          "\ncontrol value is incremented or decremented after each iteration. The sequence"
          "\norder is automatic else it could be specified with keywords ASC or DESC."
          "\n\nfor {var} in {integer expression} to {integer expression} [asc|desc] loop"
          "\n    [statement ...]"
          "\nend loop;",
  /*WHILE */  "The WHILE statement provides loop flow in execution."
          "\nIt has a boolean expression associated with it that is evaluated at runtime."
          "\nWhile the result of that evaluation is a 'true' value, then statement after"
          "\nthe LOOP keyword will be executed. If multiple statements follow the LOOP"
          "\nkeyword, they are executed in sequence but only while the expression is true."
          "\n\nwhile {boolean expression} loop"
          "\n    [statement ...]"
          "\nend loop;",
  /*LOOP  */  "The LOOP statement begins the block of statements inside loop FOR or WHILE.",
  /*IN    */  "See statement FOR.",
  /*TO    */  "See statement FOR.",
  /*RETURN*/  "The RETURN statement exit from program returning a payload value or not."
          "\n\nreturn [{expression}];"
          "\n\nOnly boolean, integer, numeric, and literal expression can be returned.",
  /*BEGIN */  "The BEGIN statement begins a new block of statements."
          "\n\nbegin [statement ...]"
          "\n[ exception [ when {exception name} then [statement ...] ...] ]"
          "\nend;"
          "\n\nSee EXCEPTION, or statement RAISE.",
  /*BREAK */  "The BREAK statement jumps out of a loop."
          "\n\nbreak;",
  /*CONTINUE*/"The CONTINUE statement \"jumps over\" one iteration in the loop."
          "\n\ncontinue;",
  /*END   */  "The END statement close the latest block of statements opened by BEGIN, FOR,"
          "\nWHILE or IF.",
  /*ENDIF*/   "The END IF statement close the latest block of statement IF. See IF.",
  /*ENDLOOP*/ "The END LOOP statement close the latest block of statement in a loop.",
  /*PRINT */  "The PRINT statement writes out the string representation of expression(s),"
          "\nterminated by a newline."
          "\n\nprint [{expression} ...];"
          "\n\nThe printed values are human readable text, depending of expression type.",
  /*PUT   */  "The PUT statement writes out the string value of expression(s)."
          "\n\nput [{expression} ...];"
          "\n\nOnly boolean, integer, decimal, and string expression can be written out."
          "\nAll other types must be converted first (see function STR).",
  /*DO    */  "The DO statement execute the expression to follow."
          "\n\ndo {expression};",
  /*EXCEPT*/  "The keyword EXCEPTION marks the beginning of exception processing section,"
          "\ninside a block BEGIN...END. We could catch user defined exception by RAISE,"
          "\nor throwable error among $Bdivide_by_zero$N, $Bout_of_range$N."
          "\nThe keyword $Bothers$N allows us to catch any throwable exception."
          "\n\nexception"
          "\n[ when {exception name} then [statement ...] ...]"
          "\n[ when others then [statement ...] ]"
          "\n\nWe can retrieve the catched exception calling the constant $Berror$N."
          "\nSee statement BEGIN, or RAISE.",
  /*WHEN  */  "See EXCEPTION.",
  /*RAISE */  "The RAISE statement throws a user named exception, or throwable error."
          "\n\nraise {exception name};"
          "\n\nSee EXCEPTION, or statement BEGIN.",
  /*ASC   */  "See statement FOR.",
  /*DESC  */  "See statement FOR.",
  /*IS    */  "See statement FUNCTION.",
  /*RETURNS*/ "See statement FUNCTION.",
};

}

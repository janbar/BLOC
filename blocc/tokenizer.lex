/* don't generate functions we don't need */
%option nounput noyywrap

/* generate a scanner that's thread safe */
%option reentrant stack
%x COMMENT
%x LITERAL

/* Generate "words" rather than "yy" as a prefix, e.g.
   wordslex() rather than yylex(). This allows multiple
   Flex scanners to be linked with the same application */
%option prefix="_tokenizer"

%{
#include <stdio.h>
#include <stdlib.h>

#include "tokenizer.h"

/* Disable features we do not need. */
#define YY_NEVER_INTERACTIVE 1
#define YY_NO_UNPUT 1
#if (defined(_WIN32) || defined(_WIN64))
#define YY_NO_UNISTD_H 1
#endif
%}

DIGIT     [0-9]
LETTER    [a-zA-Z_$]
DOT       "."
BLANK     " "

KEYWORD   {LETTER}+[0-9a-zA-Z_$]*
INTEGER   {DIGIT}+
D1        {DIGIT}+{DOT}{DIGIT}+
D2        {DOT}{DIGIT}+
DOUBLE    ({D1}|{D2})
FLOAT     ({DIGIT}+|{DOUBLE})([eE][+-]?){DIGIT}+
HEXANUM   [0][xX][0-9a-fA-F]+

SP        (u8|u|U|L)
ES        (\\(['"\?\\abfnrtv]|[0-7]{1,3}|x[a-fA-F0-9]+))
WS        [ \t\v\f]

SPACE     {WS}*

%%

 /* attempting to match and capture an entire multi-line
    comment could strain lex's buffers, so we match the
    beginning, then deal with the ensuing characters,
    until the end */

"/*"                            {
  yy_push_state( COMMENT, yyscanner);
  return TOKEN_COMMENTBEG;
}
<COMMENT>"*/"                   {
  yy_pop_state( yyscanner );
  return TOKEN_COMMENTEND;
}
<COMMENT>.|\n                   {
    return TOKEN_COMMENTSTR;
}

 /* attempting to match and capture an entire multi-line
    string could strain lex's buffers, so we match the
    beginning, then deal with the ensuing characters,
    until the end.
    Encapsulator is <quotation>
    Encapsulator is escaped with <quotation><quotation> */

({SP}?\")                       {
  yy_push_state( LITERAL, yyscanner);
  return TOKEN_LITERALBEG;
}
<LITERAL>"\""                   {
  yy_pop_state( yyscanner );
  return TOKEN_LITERALEND;
}
<LITERAL>.|\n|"\"\""            {
    return TOKEN_LITERALSTR;
}

 /* single-line comments can be handled the default way.
    The yytext variable is provided by lex and points
    to the characters that match the regex */

"//".*                          { return TOKEN_COMMENT; }
^[ \t]*#.*                      { return TOKEN_DIRECTIVE; }

{INTEGER}                       { return TOKEN_INTEGER; }
{HEXANUM}                       { return TOKEN_HEXANUM; }
{DOUBLE}                        { return TOKEN_DOUBLE; }
{FLOAT}                         { return TOKEN_FLOAT; }
{SPACE}                         { return TOKEN_SPACE; }
"=="                            { return TOKEN_ISEQUAL; }
">="                            { return TOKEN_ISEQMORE; }
"<="                            { return TOKEN_ISEQLESS; }
"!="                            { return TOKEN_ISNOTEQ; }
"<>"                            { return TOKEN_ISNOTEQ; }
":="                            { return TOKEN_ASSIGN; }
"<<"                            { return TOKEN_POPLEFT; }
">>"                            { return TOKEN_PUSHRIGHT; }
"++"                            { return TOKEN_INCREMENT; }
"--"                            { return TOKEN_DECREMENT; }
"**"                            { return TOKEN_POWER; }
"&&"                            { return TOKEN_AND; }
"||"                            { return TOKEN_OR; }
{KEYWORD}                       { return TOKEN_KEYWORD; }

.|\n                            {
  if (yytext[0] != 0)
    return (unsigned char) yytext[0];
}
%%

struct _token_scanner {
  void * handle;
  yyscan_t scanner;
  YY_BUFFER_STATE buf;
  TOKEN_READER reader;
  int enable_space;
};

typedef int (*_callback)(void *, int, const char *);

TOKEN_SCANNER
tokenizer_init(void * handle, TOKEN_READER reader)
{
  TOKEN_SCANNER scanner = (TOKEN_SCANNER) malloc(sizeof(struct _token_scanner));
  if (scanner == 0)
    return 0;
  memset(scanner, 0, sizeof(struct _token_scanner));
  /* init the lexer */
  if (yylex_init(&(scanner->scanner)) != 0)
  {
    free(scanner);
    return 0;
  }
  scanner->handle = handle;
  scanner->reader = reader;
  scanner->buf = yy_scan_string("", scanner->scanner);
  return scanner;
}

void tokenizer_free(TOKEN_SCANNER scanner)
 {
  if (scanner == 0 || scanner->scanner == 0)
    return;
  /* finalize */
  yylex_destroy(scanner->scanner);
  free(scanner);
}

YY_BUFFER_STATE tokenizer_buf(TOKEN_SCANNER scanner)
{
  char str[1024];
  /* check the reader exists */
  if (scanner->reader != 0)
  {
    int n = 0;
    scanner->reader(scanner->handle, str, &n, 1023);
    if (n > 0)
    {
      str[n] = '\0';
      return yy_scan_string(str, scanner->scanner);
    }
  }
  return 0;
}

void tokenizer_scan(TOKEN_SCANNER scanner, TOKEN_CALLBACK callback)
{
  for (;;)
  {
    int r;
    /* create a new buf */
    YY_BUFFER_STATE buf = tokenizer_buf(scanner);
    if (buf == 0)
      break;
    /* Each time yylex finds a word, it returns nonzero. It resumes where it
       left off when we call it again */
    while ((r = yylex(scanner->scanner)) > 0)
    {
      /* bypass space */
      if (r == TOKEN_SPACE && !scanner->enable_space)
        continue;
      /* call the user supplied function with rule and yytext of the match,
         then continue if it returns zero */
      if (callback(scanner->handle, r, yyget_text(scanner->scanner)) == 0)
        continue;
      /* stop the scan */
      r = (-1);
      break;
    }
    /* cleanup */
    yy_delete_buffer(buf, scanner->scanner);
    /* loop until stop */
    if (r != 0)
      break;
  }
}

void tokenizer_lex(TOKEN_SCANNER scanner, int * token, const char ** text)
{
  int r = 0;
  for (;;)
  {
    /* Each time yylex finds a word, it returns nonzero. It resumes where it
       left off when we call it again */
    r = yylex(scanner->scanner);
    if (r < 0)
    {
      /* return on error */
      break;
    }
    if (r == 0)
    {
      /* cleanup current buf */
      if (scanner->buf != 0)
        yy_delete_buffer(scanner->buf, scanner->scanner);
      if ((scanner->buf = tokenizer_buf(scanner)) == 0)
      {
        /* return on EOF */
        break;
      }
    }
    else
    {
      /* bypass space */
      if (r != TOKEN_SPACE || scanner->enable_space)
      {
        /* return token */
        *text = yyget_text(scanner->scanner);
        break;
      }
    }
  }
  *token = r;
}

int tokenizer_state(TOKEN_SCANNER scanner)
{
  struct yyguts_t * yyg = (struct yyguts_t*)(scanner->scanner);
  return YY_START;
}

void tokenizer_clear(TOKEN_SCANNER scanner)
{
  /* reset scanner state */
  while (tokenizer_state(scanner) != INITIAL)
  {
    yy_pop_state(scanner->scanner);
  }
}

void tokenizer_enable_space(TOKEN_SCANNER scanner)
{
  scanner->enable_space = 1;
}

void tokenizer_disable_space(TOKEN_SCANNER scanner)
{
  scanner->enable_space = 0;
}

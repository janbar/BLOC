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

#include "cli_parser.h"
#include "read_file.h"
#include "license.h"
#include "copyright.h"
#include "help.h"
#include "help_expression.h"
#include "help_variable.h"
#include "help_type.h"
#include "help_boolean.h"
#include "help_integer.h"
#include "help_decimal.h"
#include "help_string.h"
#include "help_object.h"
#include "help_bytes.h"
#include "help_tuple.h"
#include "help_table.h"

#include <blocc/parser.h>
#include <blocc/readstdin.h>
#include <blocc/exception_parse.h>
#include <blocc/parse_expression.h>
#include <blocc/expression_literal.h>
#include <blocc/expression_operator.h>
#include <blocc/expression_builtin.h>
#include <blocc/expression_member.h>
#include <blocc/import_manager.h>

#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <algorithm> // std::find
#include <set>

#if (defined(_WIN32) || defined(_WIN64))
#define __WINDOWS__
#endif

#ifdef __WINDOWS__
#define STDOUT stdout
#define STDOUT_FILENO _fileno(stdout)
#define FLUSHOUT fflush(STDOUT)
#define PRINT(a) fputs(a, STDOUT)
#define PRINT1(a,b) fprintf(STDOUT, a, b)
#define PRINT2(a,b,c) fprintf(STDOUT, a, b, c)
#include <io.h>
#else
#define STDOUT stdout
#define FLUSHOUT fflush(STDOUT)
#define PRINT(a) fputs(a, STDOUT)
#define PRINT1(a,b) fprintf(STDOUT, a, b)
#define PRINT2(a,b,c) fprintf(STDOUT, a, b, c)
#include <unistd.h>
#endif

#define PRINTBUF(buf,len,sout) for(int ii=0; ii<len; ++ii) fputc(buf[ii], sout)

enum CMD
{
  CMD_NOTCMD = -1,
  CMD_EXIT = 0, CMD_CLEAR,    CMD_LIST,   CMD_LOAD,   CMD_SAVE,
  CMD_RUN,      CMD_DESC,     CMD_DUMP,   CMD_HELP,   CMD_EXPR,
  CMD_COPYR,    CMD_LICENSE,  CMD_SYSTEM,
};

static const char * CMD_KEYWORDS[] =
{
  "exit",       "clear",      "list",     "load",     "save",
  "run",        "desc",       "dump",     "help",     "=",
  "copyright",  "license",    "!",
};

typedef struct { const char * help; unsigned * text_len; unsigned char * text; }
HELP_CATEGORY;
static const HELP_CATEGORY HELP_CATEGORIES[] =
{
  { "",           &help_txt_len,              help_txt },
  { "expression", &help_expression_txt_len,   help_expression_txt },
  { "variable",   &help_variable_txt_len,     help_variable_txt },
  { "type",       &help_type_txt_len,         help_type_txt },
  { "boolean",    &help_boolean_txt_len,      help_boolean_txt },
  { "integer",    &help_integer_txt_len,      help_integer_txt },
  { "decimal",    &help_decimal_txt_len,      help_decimal_txt },
  { "string",     &help_string_txt_len,       help_string_txt },
  { "object",     &help_object_txt_len,       help_object_txt },
  { "bytes",      &help_bytes_txt_len,        help_bytes_txt },
  { "tuple",      &help_tuple_txt_len,        help_tuple_txt },
  { "table",      &help_table_txt_len,        help_table_txt },
};

/* ANSI terminal colors */
enum FG
{
  fgRED      = 31,    fgGREEN    = 32,    fgORANGE   = 33,
  fgBLUE     = 34,    fgCYAN     = 36,    fgDARKGREY = 90,
  fgYELLOW   = 93,
};

static bool g_has_color = false;

static void read_input(void * handle, char * buf, int * len, int max_size);
static int find_cmd(const std::string& c);
static void set_color(enum FG c);
static void reset_color(void);
static void output_cli(bloc::Context& ctx);
static bool path_valid(const std::string& path);
static void print_table(const std::set<std::string>& list, int colw, int coln);
static void print_btml(const void * buf, unsigned len);
static void print_help(const std::string& what);
static int cli_cmd(bloc::Parser& p, bloc::Context& ctx, std::list<const bloc::Statement*>& statements);

void cli_parser(const MainOptions& options, const std::vector<std::string>& args)
{
  /* first check for virtual terminal and colored output */
  if (options.color)
    g_has_color = true;
  else if (::isatty(STDOUT_FILENO))
  {
    char * buf = ::getenv("TERM");
    if (buf != nullptr)
    {
      std::string env(buf);
      g_has_color = (env == "xterm" || env == "linux" || env.find("color") != std::string::npos);
    }
  }
  PRINT(bloc::Context::versionHeader());
  PRINT("\nType \"help\" , \"copyright\" or \"license\" for more information.\n");

  bloc::Context ctx;
  /* load args values into context as variables $0..$n */
  for (int i = 0; i < args.size(); ++i)
  {
    bloc::Symbol& symbol = ctx.registerSymbol(std::string("$").append(std::to_string(i)), bloc::Type::LITERAL);
    ctx.storeVariable(symbol, bloc::LiteralExpression(args[i]));
  }
  bloc::Parser * p = bloc::Parser::createInteractiveParser(ctx, &read_input);
  if (!p)
    return;

  std::list<const bloc::Statement*> statements;

  while (p->state() != bloc::Parser::ABORT)
  {
    bloc::Statement * s = nullptr;
    bloc::TokenPtr t;

    try { t = p->front(); }
    catch (bloc::ParseError& ee)
    {
      /* silent */
      break;
    }

    /* check for new CLI command, else begin parse */
    int cli = cli_cmd(*p, ctx, statements);
    if (cli < 0)
      break;
    if (cli > 0)
      continue;

    try { s = p->parseStatement(); }
    catch (bloc::ParseError& pe)
    {
      if (pe.no == bloc::EXC_PARSE_EOF)
      {
        set_color(fgRED); PRINT1("%s\n", pe.what()); reset_color();
        break;
      }
      set_color(fgRED); PRINT1("Error: %s\n", pe.what()); reset_color();
      p->clear();
    }

    if (s)
    {
      double d = ctx.timestamp();
      const bloc::Statement * r = s;
      while (r)
      {
        try { r = r->execute(ctx); }
        catch (bloc::RuntimeError& re)
        {
          set_color(fgRED); PRINT1("Error: %s", re.what()); reset_color();
          ctx.purgeWorkingMemory();
          break;
        }
      }
      d = ctx.elapsed(d);
      set_color(fgBLUE);
      PRINT1("\nElapsed: %f\n", d);
      reset_color();

      statements.push_back(s);

      /* clear stop condition in interative mode */
      if (ctx.returnCondition())
      {
        ctx.returnCondition(false);
        try { output_cli(ctx); }
        catch (bloc::RuntimeError& re)
        {
          set_color(fgRED); PRINT1("Error: %s", re.what()); reset_color();
        }
      }

      /* drop newline beyond the statement */
      if (p->front() && p->front()->code == bloc::Parser::NEWLINE)
        p->pop();
    }
  }
  for (auto s : statements) delete s;
  delete p;
}

static void read_input(void * handle, char * buf, int * len, int max_size)
{
  bloc::Parser * p = static_cast<bloc::Parser*>(handle);
  if (p->state() == bloc::Parser::PARSE)
    PRINT("... ");
  else
    PRINT(">>> ");
  FLUSHOUT;
  *len = bloc_readstdin(buf, max_size);
}

static int find_cmd(const std::string& c)
{
  for (size_t i = 0; i < (sizeof (CMD_KEYWORDS) / sizeof (char*)); ++i)
  {
    if (::strcmp(CMD_KEYWORDS[i], c.c_str()) == 0)
      return i;
  }
  return CMD_NOTCMD;
}

static void set_color(enum FG c)
{
  if (g_has_color)
    PRINT1("\033[%d;1m", c);
}

static void reset_color(void)
{
  if (g_has_color)
  {
    fputs("\033[0m", STDOUT);
    fflush(STDOUT);
  }
}

static void output_cli(bloc::Context& ctx)
{
  bloc::Expression * exp = ctx.dropReturned();
  if (exp)
  {
    set_color(fgGREEN);
    try
    {
      if (exp->type(ctx).level() == 0)
      {
        switch (exp->type(ctx).major())
        {
        case bloc::Type::BOOLEAN:
          PRINT(bloc::BooleanExpression::readableBoolean(exp->boolean(ctx)).c_str());
          PRINT("\n");
          break;
        case bloc::Type::INTEGER:
          PRINT(bloc::IntegerExpression::readableInteger(exp->integer(ctx)).c_str());
          PRINT("\n");
          break;
        case bloc::Type::NUMERIC:
          PRINT(bloc::NumericExpression::readableNumeric(exp->numeric(ctx)).c_str());
          PRINT("\n");
          break;
        case bloc::Type::LITERAL:
        {
          /* print sub string to the standard output */
          char buf[80];
          snprintf(buf, sizeof(buf), "%s", exp->literal(ctx).c_str());
          PRINT(buf);
          PRINT("\n");
          break;
        }
        case bloc::Type::TABCHAR:
        {
          bloc::TabChar& r = exp->tabchar(ctx);
          bloc::TabcharExpression::outputTabchar(r, STDOUT, 1);
          break;
        }
        case bloc::Type::ROWTYPE:
          PRINT(bloc::TupleExpression::readableTuple(exp->tuple(ctx), ctx).c_str());
          PRINT("\n");
          break;
        default:
          /* not printable */
          break;
        }
      }
      else
      {
        bloc::Collection& c = exp->collection(ctx);
        PRINT(exp->typeName(ctx).append(1, '[').append(std::to_string(c.size())).append(1, ']').c_str());
        PRINT("\n");
      }
    }
    catch(bloc::RuntimeError& re)
    {
      set_color(fgRED); PRINT1("Error: %s\n", re.what());
    }
    delete exp;
    ctx.purgeWorkingMemory();
    reset_color();
  }
}

static bool path_valid(const std::string& path)
{
  if (path.find('\n') != std::string::npos ||
          path.find('\r') != std::string::npos ||
          path.find('\t') != std::string::npos)
    return false;
  return true;
}

static void print_table(const std::set<std::string>& list, int colw, int coln)
{
  auto idx = list.cbegin();
  while (idx != list.cend())
  {
    int i;
    for (i = 0; i < coln && idx != list.cend(); ++i, ++idx)
      PRINT2("%*s", colw, idx->c_str());
    PRINT("\n\n");
  }
}

static void print_btml(const void * buf, unsigned len)
{
  static const char BOLD[4] = { 0x1b, 0x5b, 0x31, 0x6d };
  static const char NORM[4] = { 0x1b, 0x5b, 0x30, 0x6d };

  const char * cur = (const char *)buf;
  char pc = 0;
  char c;
  while (len > 0 && (c = *cur) != 0)
  {
    ++cur;
    --len;
    if (pc == '$')
    {
      switch (c)
      {
      case 'B':
        pc = 0;
        if (g_has_color)
          PRINTBUF(BOLD, sizeof(BOLD), STDOUT);
        break;
      case 'N':
        pc = 0;
        if (g_has_color)
          PRINTBUF(NORM, sizeof(NORM), STDOUT);
        break;
      case 'T':
        pc = 0;
        fputc('\t', STDOUT);
        break;
      default:
        fputc(pc, STDOUT);
        pc = c;
      }
    }
    else if (pc != 0)
    {
      fputc(pc, STDOUT);
      pc = c;
    }
    else
      pc = c;
  }
  if (pc != 0)
    fputc(pc, STDOUT);
  if (g_has_color)
    PRINTBUF(NORM, sizeof(NORM), STDOUT);
}

static void print_help(const std::string& what)
{
  std::string w(what);
  std::transform(w.begin(), w.end(), w.begin(), ::tolower);
  /* help from libbloc */
  if (w == "function")
  {
    print_btml("\n$BFUNCTION\n\n", UINT16_MAX);
    print_table(bloc::BuiltinExpression::keywordSet(), -10, 6);
  }
  else if (w == "statement")
  {
    print_btml("\n$BSTATEMENT\n\n", UINT16_MAX);
    print_table(bloc::Statement::keywordSet(), -10, 6);
  }
  else if (w == "operator")
  {
    print_btml("\n$BOPERATOR\n\n", UINT16_MAX);
    for (auto& op :bloc::OperatorExpression::operatorSet())
    {
      char buf[16];
      snprintf(buf, 16, "$B%s$N$T  ", bloc::OperatorExpression::OPVALS[op]);
      print_btml(buf, 16);
      print_btml(bloc::OperatorExpression::HELPS[op], UINT16_MAX);
      PRINT("\n");
    }
    PRINT("\n");
  }
  else
  {
    /* search category from the local table */
    int cat = 0;
    for (int i = 0; i < (sizeof(HELP_CATEGORIES) / sizeof(HELP_CATEGORY)); ++i)
    {
      if (w != HELP_CATEGORIES[i].help)
        continue;
      cat = i;
      break;
    }
    if (cat > 0)
    {
      print_btml(HELP_CATEGORIES[cat].text, *HELP_CATEGORIES[cat].text_len);
      PRINT("\n");
    }
    else
    {
      /* search keyword */
      int k;
      if ((k = bloc::Statement::findKeyword(w)) != bloc::Statement::unknown)
      {
        char buf[16];
        snprintf(buf, 16, "\n$B%s\n\n", bloc::Statement::KEYWORDS[k]);
        print_btml(buf, 16);
        print_btml(bloc::Statement::HELPS[k], UINT16_MAX);
        PRINT("\n\n");
      }
      else if ((k = bloc::BuiltinExpression::findKeyword(w)) != bloc::BuiltinExpression::unknown)
      {
        char buf[16];
        snprintf(buf, 16, "\n$B%s ", bloc::BuiltinExpression::KEYWORDS[k]);
        print_btml(buf, 16);
        print_btml(bloc::BuiltinExpression::HELPS[k], UINT16_MAX);
        PRINT("\n\n");
      }
      else
      {
        set_color(fgYELLOW); PRINT("Unknown keyword.\n"); reset_color();
      }
    }
  }
}

static int cli_cmd(bloc::Parser& p, bloc::Context& ctx, std::list<const bloc::Statement*>& statements)
{
  bloc::TokenPtr t = p.front();
  CMD c = (CMD)find_cmd(p.front()->text);
  switch (c)
  {
  case CMD_NOTCMD:
    return 0;
  case CMD_EXIT:
    return (-1); /* QUIT */
  case CMD_RUN:
  {
    p.pop();
    double d = ctx.timestamp();
    try
    {
      bloc::Executable::run(ctx, statements);
      if (ctx.returnCondition())
      {
        ctx.returnCondition(false);
        output_cli(ctx);
      }
    }
    catch(bloc::RuntimeError& re)
    {
      set_color(fgRED); PRINT1("Error: %s\n", re.what()); reset_color();
    }
    d = ctx.elapsed(d);
    set_color(fgBLUE);
    PRINT1("\nElapsed: %f\n", d);
    reset_color();
    p.clear();
    return 1;
  }
  case CMD_DESC:
  {
    t = p.pop();
    if (t->code != TOKEN_KEYWORD)
    {
      p.push(t);
      return 0;
    }
    t = p.pop();
    ctx.describeSymbol(t->text);
    p.clear();
    return 1;
  }
  case CMD_DUMP:
  {
    p.pop();
    ctx.dumpVariables();
    p.clear();
    return 1;
  }
  case CMD_CLEAR:
  {
    p.pop();
    for (auto s : statements) delete s;
    statements.clear();
    ctx.purge();
    p.clear();
    return 1;
  }
  case CMD_LIST:
  {
    p.pop();
    for (auto s : statements)
    {
      s->unparse(ctx, STDOUT);
      fputc(bloc::Parser::SEPARATOR, STDOUT);
      fputc(bloc::Parser::NEWLINE, STDOUT);
    }
    fputc(bloc::Parser::NEWLINE, STDOUT);
    p.clear();
    return 1;
  }
  case CMD_EXPR:
  {
    /* perform inline expression */
    p.pop();
    try
    {
      ctx.saveReturned(p.parseExpression());
      /* eat extra terminators */
      while ((t = p.pop())->code == bloc::Parser::SEPARATOR) { }
      if (t->code != bloc::Parser::NEWLINE)
      {
        delete ctx.dropReturned();
        throw bloc::ParseError(bloc::EXC_PARSE_EXPRESSION_END_S, t->text.c_str());
      }
      output_cli(ctx);
    }
    catch (bloc::ParseError& pe)
    {
      if ((t = p.pop())->code != bloc::Parser::NEWLINE)
        p.clear();
      set_color(fgRED); PRINT1("Error: %s\n", pe.what()); reset_color();
    }
    return 1;
  }
  case CMD_LOAD:
  {
    std::string path;
    p.pop();
    t = p.pop();
    if (t->code == bloc::Parser::NEWLINE)
    {
      p.push(t);
      return 0;
    }
    else if (t->code == TOKEN_LITERALSTR)
    {
      bloc::LiteralExpression * str = bloc::LiteralExpression::parse(t->text);
      path.assign(str->literal(ctx));
      delete str;
      if (p.front()->code != bloc::Parser::NEWLINE)
        p.clear();
    }
    else
    {
      while (t->code != bloc::Parser::NEWLINE)
      {
        path.append(t->text);
        /* pop all to follow */
        if (!p.popAny(t))
          return 0;
      }
      p.push(t); /* put back last NL */
    }
    if (!path_valid(path))
    {
      set_color(fgRED);
      PRINT("Error: Path is invalid.\n");
      reset_color();
      return 1;
    }
    FILE * progfile = ::fopen(path.c_str(), "r");
    if (!progfile)
    {
      set_color(fgRED);
      PRINT("Error: Failed to open file for read.\n");
      reset_color();
      return 1;
    }
    bloc::Executable * exec = nullptr;
    try { exec = bloc::Parser::parse(ctx, progfile, &read_file, true); }
    catch (bloc::ParseError& pe)
    {
      set_color(fgRED); PRINT1("Error: %s\n", pe.what()); reset_color();
    }
    ::fclose(progfile);
    if (!exec)
      return 1;
    /* move statements into statements pool */
    for (auto s : exec->statements())
      statements.push_back(s);
    exec->statements().clear();
    /* cleanup */
    delete exec;
    return 1;
  }
  case CMD_SAVE:
  {
    std::string path;
    p.pop();
    t = p.pop();
    if (t->code == bloc::Parser::NEWLINE)
    {
      p.push(t);
      return 0;
    }
    else if (t->code == TOKEN_LITERALSTR)
    {
      bloc::LiteralExpression * str = bloc::LiteralExpression::parse(t->text);
      path.assign(str->literal(ctx));
      delete str;
      if (p.front()->code != bloc::Parser::NEWLINE)
        p.clear();
    }
    else
    {
      while (t->code != bloc::Parser::NEWLINE)
      {
        path.append(t->text);
        /* pop all to follow */
        if (!p.popAny(t))
          return 0;
      }
      p.push(t); /* put back last NL */
    }
    if (!path_valid(path))
    {
      set_color(fgRED);
      PRINT("Error: Path is invalid.\n");
      reset_color();
      return 1;
    }
    FILE * progfile = ::fopen(path.c_str(), "w");
    if (!progfile)
    {
      set_color(fgRED);
      PRINT("Error: Failed to open file for write.\n");
      reset_color();
      return 1;
    }
    for (auto s : statements)
    {
      s->unparse(ctx, progfile);
      fputc(bloc::Parser::SEPARATOR, progfile);
      fputc(bloc::Parser::NEWLINE, progfile);
    }
    ::fclose(progfile);
    return 1;
  }
  case CMD_SYSTEM:
  {
    std::string cmd;
    p.pop();
    t = p.pop();
    if (t->code == bloc::Parser::NEWLINE)
    {
      p.push(t);
      return 0;
    }
    while (t->code != bloc::Parser::NEWLINE)
    {
      cmd.append(t->text);
      /* pop all to follow */
      if (!p.popAny(t))
        return 0;
    }
    p.push(t); /* put back last NL */
    int r = system(cmd.c_str());
    (void)r;
    return 1;
  }
  case CMD_COPYR:
  {
    PRINTBUF(copyright_txt, copyright_txt_len, STDOUT);
    PRINT("\n");
    p.clear();
    return 1;
  }
  case CMD_LICENSE:
  {
    PRINTBUF(license_txt, license_txt_len, STDOUT);
    PRINT("\n");
    p.clear();
    return 1;
  }
  case CMD_HELP:
  {
    p.pop();
    t = p.pop();
    if (t->code == bloc::Parser::NEWLINE)
    {
      print_btml(HELP_CATEGORIES[0].text, *HELP_CATEGORIES[0].text_len);
      PRINT("\n");
      return 1;
    }
    std::string w;
    if (t->code == TOKEN_KEYWORD)
      w.assign(t->text);
    while ((t = p.pop())->code == TOKEN_KEYWORD)
      w.append(" ").append(t->text);
    if (!w.empty())
      print_help(w);
    else
    {
      set_color(fgRED); PRINT("Not a keyword. type \"help\" for usage.\n"); reset_color();
    }
    if (t->code != bloc::Parser::NEWLINE)
      p.clear();
    return 1;
  }
  default:
    return 0;
  }
}

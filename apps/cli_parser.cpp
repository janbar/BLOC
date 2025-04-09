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
#include "breaker.h"
#include "license.h"
#include "copyright.h"
#include "help.h"
#include "help_expression.h"
#include "help_variable.h"
#include "help_type.h"
#include "help_boolean.h"
#include "help_integer.h"
#include "help_decimal.h"
#include "help_complex.h"
#include "help_string.h"
#include "help_object.h"
#include "help_bytes.h"
#include "help_tuple.h"
#include "help_table.h"

#include <blocc/parser.h>
#include <blocc/readstdin.h>
#include <blocc/exception_parse.h>
#include <blocc/expression_builtin.h>
#include <blocc/expression_member.h>
#include <blocc/plugin_manager.h>
#include <blocc/operator.h>
#include <blocc/collection.h>
#include <blocc/tuple.h>

#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <algorithm> // std::find
#include <set>

#ifdef HAVE_READLINE
#include <readline/readline.h>
#include <readline/history.h>
/* a static variable for holding the line */
static char * rl_line = nullptr;
#endif

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
#include "winstub.h"
#include <io.h>
#else
#define STDOUT stdout
#define FLUSHOUT fflush(STDOUT)
#define PRINT(a) fputs(a, STDOUT)
#define PRINT1(a,b) fprintf(STDOUT, a, b)
#define PRINT2(a,b,c) fprintf(STDOUT, a, b, c)
#include "signalhandler.h"
#include <unistd.h>
#endif

#define PRINTBUF(buf,len,sout) for(int ii=0; ii<len; ++ii) fputc(buf[ii], sout)

enum CMD
{
  CMD_EXIT = 0, CMD_CLEAR,    CMD_LIST,   CMD_LOAD,   CMD_SAVE,
  CMD_RUN,      CMD_DESC,     CMD_DUMP,   CMD_HELP,   CMD_EXPR,
  CMD_COPYR,    CMD_LICENSE,  CMD_SYSTEM,
  CMD_unknown,
};

static const char * CMD_KEYWORDS[] =
{
  "exit",       "clear",      "list",     "load",     "save",
  "run",        "desc",       "dump",     "help",     "=",
  "copyright",  "license",    "!",
  "" /*unknown command*/,
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
  { "complex",    &help_complex_txt_len,      help_complex_txt },
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

static void load_args(bloc::Context& ctx, std::vector<bloc::Value>&& args);
static CMD find_cmd(const std::string& c);
static void set_color(enum FG c);
static void reset_color(void);
static void output_cli(bloc::Context& ctx);
static bool path_valid(const std::string& path);
static void print_table(const std::set<std::string>& list, int colw, int coln);
static void print_btml(const void * buf, unsigned len);
static void describe_module(unsigned type_id);
static void print_help(const std::string& what);
static int cli_cmd(bloc::Parser& p, bloc::Context& ctx, std::list<const bloc::Statement*>& statements);

static breaker_t g_breaker;

class ReadInput : public bloc::Parser::StreamReader
{
public:
  int read(bloc::Parser * p, char * buf, int max_size) override;
};

void cli_parser(const MainOptions& options, std::vector<bloc::Value>&& args)
{
  /* first check for virtual terminal and colored output */
  if (options.color)
    g_has_color = true;
  else if (::isatty(STDOUT_FILENO))
  {
    const char * buf = ::getenv("TERM");
    if (buf != nullptr)
    {
      std::string env(buf);
      g_has_color = (env == "xterm" || env == "linux" || env.find("color") != std::string::npos);
    }
  }
  PRINT(bloc::Context::versionHeader());
  PRINT("\nType \"help\" , \"copyright\" or \"license\" for more information.\n");

  bloc::Context ctx;
  /* set context as trusted to allow use of restricted plugins */
  ctx.trusted(true);
  /* setup breaking state */
  g_breaker = { true, &ctx };
#ifdef __WINDOWS__
  init_ctrl_handler(&g_breaker);
#else
  SignalHandler sh;
  sh.setCallback(sig_handler, &g_breaker);
  /* catch SIGINT to handle break */
  sh.catchSignal(SIGINT);
#endif

#ifdef HAVE_READLINE
  stifle_history(512);
#endif

  ReadInput input;
  bloc::Parser * p = bloc::Parser::createInteractiveParser(ctx, input);
  if (!p)
    return;

  load_args(ctx, std::move(args));
  std::list<const bloc::Statement*> statements;

  while (p->state() != bloc::Parser::Aborted)
  {
    const bloc::Statement * s = nullptr;
    bloc::TokenPtr t;

    try { t = p->front(); }
    catch (bloc::ParseError& ee)
    {
      /* silence EOF */
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
        /* broken input issued by CTRL+D */
        set_color(fgRED); PRINT1("%s\n", pe.what()); reset_color();
        ::clearerr(stdin);
        delete p;
        p = bloc::Parser::createInteractiveParser(ctx, input);
      }
      else
      {
        set_color(fgRED); PRINT1("Error: %s\n", pe.what()); reset_color();
        p->clear();
      }
    }

    if (s)
    {
      /* disable break */
      g_breaker.state = false;
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
      if (g_breaker.state)
      {
        set_color(fgRED);
        PRINT("\nStopped\n");
        reset_color();
      }
      else
      {
        g_breaker.state = true;
        set_color(fgBLUE);
        PRINT1("\nElapsed: %f\n", d);
        reset_color();
      }

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
      if (p->front() && p->front()->code == bloc::Parser::NewLine)
        p->pop();
    }
  }
  for (auto s : statements) delete s;
  delete p;
#ifdef __WINDOWS__
  stop_ctrl_handler();
#else
  sh.omitSignal(SIGINT);
  sh.setCallback(nullptr, nullptr);
#endif
#ifdef HAVE_READLINE
  if (rl_line)
    free(rl_line);
  clear_history();
#endif
}

int ReadInput::read(bloc::Parser * p, char * buf, int max_size)
{
#ifdef HAVE_READLINE
  /* a static variable to store the read position */
  static char * rl_pos = nullptr;
  unsigned n = 0;
  if (rl_pos)
  {
    if (*rl_pos)
    {
      /* fill a chunk from previous read */
      while (*rl_pos && n < max_size)
      {
        char c = *rl_pos;
        buf[n] = c;
        ++rl_pos;
        ++n;
        if (c == bloc::Parser::NewLine)
        {
          return n;
        }
      }
      if (n >= max_size)
      {
        /* buffer is full */
        return max_size;
      }
    }
    /* free old buffer */
    free(rl_line);
    rl_line = rl_pos = nullptr;
    buf[n++] = bloc::Parser::NewLine;
    return n;
  }

  /* get a new line */
  if (p->state() == bloc::Parser::Parsing)
    rl_line = readline("... ");
  else {
    rl_line = readline(">>> ");
  }
  /* if the line has any text in it */
  if (rl_line)
  {
    if (*rl_line)
    {
      /* save it on the history */
      add_history(rl_line);
      rl_pos = rl_line;
      while (*rl_pos && n < max_size)
      {
        char c = *rl_pos;
        buf[n] = c;
        ++rl_pos;
        ++n;
        if (c == bloc::Parser::NewLine)
        {
          return n;
        }
      }
      if (n >= max_size)
      {
        /* buffer is full */
        return max_size;
      }
    }
    /* free old buffer */
    free(rl_line);
    rl_line = rl_pos = nullptr;
    buf[n++] = bloc::Parser::NewLine;
    return n;
  }
  /* EOF */
  return 0;
#else
  if (p->state() == bloc::Parser::Parsing)
    PRINT("... ");
  else
    PRINT(">>> ");
  FLUSHOUT;
  return bloc_readstdin(buf, max_size);
#endif
}

static void load_args(bloc::Context& ctx, std::vector<bloc::Value>&& args)
{
  /* load arguments into the context, as table named $ARG */
  bloc::Collection * c = new bloc::Collection(
          bloc::Value::type_literal.levelUp(), std::move(args));
  const bloc::Symbol& symbol = ctx.registerSymbol(std::string("$ARG"), c->table_type());
  ctx.storeVariable(symbol, bloc::Value(c));
}

static CMD find_cmd(const std::string& c)
{
  for (unsigned i = 0; i < (sizeof (CMD_KEYWORDS) / sizeof (char*)); ++i)
  {
    if (::strcmp(CMD_KEYWORDS[i], c.c_str()) == 0)
      return (CMD)i;
  }
  return CMD_unknown;
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
  bloc::Value * val = ctx.dropReturned();
  if (val != nullptr)
  {
    set_color(fgGREEN);
    try
    {
      if (val->isNull())
      {
        PRINT(bloc::Value::STR_NIL);
        PRINT("\n");
      }
      else if (val->type().level() == 0)
      {
        switch (val->type().major())
        {
        case bloc::Type::BOOLEAN:
          PRINT(bloc::Value::readableBoolean(*(val->boolean())).c_str());
          PRINT("\n");
          break;
        case bloc::Type::INTEGER:
          PRINT(bloc::Value::readableInteger(*(val->integer())).c_str());
          PRINT("\n");
          break;
        case bloc::Type::NUMERIC:
          PRINT(bloc::Value::readableNumeric(*(val->numeric())).c_str());
          PRINT("\n");
          break;
        case bloc::Type::IMAGINARY:
          PRINT(bloc::Value::readableImaginary(*(val->imaginary())).c_str());
          PRINT("\n");
          break;
        case bloc::Type::LITERAL:
        {
          /* print sub string to the standard output */
          char buf[80];
          snprintf(buf, sizeof(buf), "%s", val->literal()->c_str());
          PRINT(buf);
          PRINT("\n");
          break;
        }
        case bloc::Type::TABCHAR:
        {
          bloc::Value::outputTabchar(*(val->tabchar()), STDOUT, 1);
          break;
        }
        case bloc::Type::ROWTYPE:
          PRINT(bloc::Value::readableTuple(*(val->tuple())).c_str());
          PRINT("\n");
          break;
        default:
          /* not printable */
          break;
        }
      }
      else
      {
        const bloc::Collection * c = val->collection();
        PRINT(val->typeName().append(1, '[').append(std::to_string(c->size())).append(1, ']').c_str());
        PRINT("\n");
      }
    }
    catch(bloc::RuntimeError& re)
    {
      set_color(fgRED); PRINT1("Error: %s\n", re.what());
    }
    delete val;
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
    for (int i = 0; i < coln && idx != list.cend(); ++i, ++idx)
      PRINT2("%*s", colw, idx->c_str());
    PRINT("\n\n");
  }
}

static const char BOLD[4] = { 0x1b, 0x5b, 0x31, 0x6d };
static const char NORM[4] = { 0x1b, 0x5b, 0x30, 0x6d };

static void print_btml(const void * buf, unsigned len)
{
  const char * cur = (const char *)buf;
  char pc = 0;
  char c;
  while (len > 0)
  {
    if ((c = *cur) == 0)
      break;
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

    ++cur;
    --len;
  }
  if (pc != 0)
    fputc(pc, STDOUT);
  if (g_has_color)
    PRINTBUF(NORM, sizeof(NORM), STDOUT);
}

static void describe_module(unsigned type_id)
{
  char buf[80];
  const bloc::PLUGGED_MODULE& plug = bloc::PluginManager::instance().plugged(type_id);
  print_btml("\n$BCONSTRUCTORS\n\n", UINT16_MAX);
  snprintf(buf, sizeof(buf), "$B%s$N", plug.interface.name);
  print_btml(buf, sizeof(buf));
  PRINT("() is default contructor\n\n");
  for (unsigned i = 0; i < plug.interface.ctors_count; ++i)
  {
    snprintf(buf, sizeof(buf), "$B%s$N", plug.interface.name);
    print_btml(buf, sizeof(buf));
    PRINT("(");
    for (unsigned j = 0; j < plug.interface.ctors[i].args_count; ++j)
    {
      bloc::Type t = bloc::plugin::make_type(plug.interface.ctors[i].args[j], type_id);
      if (j > 0)
        PRINT(", ");
      if (t.major() == bloc::Type::ROWTYPE)
      {
        bloc::TupleDecl::Decl decl = bloc::plugin::make_decl(plug.interface.ctors[i].args[j].decl, type_id);
        PRINT(t.typeName(decl.tupleName()).c_str());
      }
      else
      {
        PRINT(t.typeName().c_str());
      }
    }
    PRINT(")\n");
    if (plug.interface.ctors[i].brief)
      PRINT1("%s\n", plug.interface.ctors[i].brief);
    PRINT("\n");
  }
  print_btml("\n$BMETHODS\n\n", UINT16_MAX);
  for (unsigned i = 0; i < plug.interface.method_count; ++i)
  {
    snprintf(buf, sizeof(buf), "$B%s$N", plug.interface.methods[i].name);
    print_btml(buf, sizeof(buf));
    PRINT("(");
    for (unsigned j = 0; j < plug.interface.methods[i].args_count; ++j)
    {
      bloc::Type t = bloc::plugin::make_type(plug.interface.methods[i].args[j].type, type_id);
      if (j > 0)
        PRINT(", ");
      if (t.major() == bloc::Type::ROWTYPE)
      {
        bloc::TupleDecl::Decl decl = bloc::plugin::make_decl(plug.interface.methods[i].args[j].type.decl, type_id);
        PRINT1("%s", t.typeName(decl.tupleName()).c_str());
      }
      else
      {
        PRINT(t.typeName().c_str());
      }
      switch (plug.interface.methods[i].args[j].mode)
      {
      case PLUGIN_IN:
        PRINT(" IN");
        break;
      case PLUGIN_INOUT:
        PRINT(" INOUT");
        break;
      default:
        break;
      }
    }
    PRINT(") returns ");
    bloc::Type t = bloc::plugin::make_type(plug.interface.methods[i].ret, type_id);
    if (t.major() == bloc::Type::ROWTYPE)
    {
      bloc::TupleDecl::Decl decl = bloc::plugin::make_decl(plug.interface.methods[i].ret.decl, type_id);
      PRINT(t.typeName(decl.tupleName()).c_str());
    }
    else
    {
      PRINT(t.typeName().c_str());
    }
    PRINT("\n");
    if (plug.interface.methods[i].brief)
      PRINT1("%s\n", plug.interface.methods[i].brief);
    PRINT("\n");
  }
}

static void print_help(const std::string& what)
{
  std::string w(what);
  std::transform(w.begin(), w.end(), w.begin(), ::tolower);
  /* help from libbloc */
  if (w == "builtin")
  {
    print_btml("\n$BBUILTIN FUNCTION\n\n", UINT16_MAX);
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
    for (auto& op : bloc::Operator::operatorSet())
    {
      char buf[16];
      snprintf(buf, 16, "$B%s$N$T  ", bloc::Operator::OPVALS[op]);
      print_btml(buf, 16);
      print_btml(bloc::Operator::HELPS[op], UINT16_MAX);
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
        unsigned type_id = bloc::PluginManager::instance().findModuleTypeId(w);
        if (type_id > 0)
          describe_module(type_id);
        else
        {
          set_color(fgYELLOW); PRINT("Unknown keyword.\n"); reset_color();
        }
      }
    }
  }
}

static int cli_cmd(bloc::Parser& p, bloc::Context& ctx, std::list<const bloc::Statement*>& statements)
{
  bloc::TokenPtr t = p.front();
  CMD c = find_cmd(p.front()->text);
  switch (c)
  {
  case CMD_unknown:
    return 0;
  case CMD_EXIT:
    return (-1); /* QUIT */
  case CMD_RUN:
  {
    p.pop();
    double d = ctx.timestamp();
    try
    {
      g_breaker.state = false;
      bloc::Executable::run(ctx, statements);
      if (ctx.returnCondition() && !g_breaker.state)
        output_cli(ctx);
    }
    catch(bloc::RuntimeError& re)
    {
      set_color(fgRED); PRINT1("Error: %s\n", re.what()); reset_color();
    }
    d = ctx.elapsed(d);
    if (g_breaker.state)
    {
      set_color(fgRED);
      PRINT("\nStopped\n");
      reset_color();
    }
    else
    {
      g_breaker.state = true;
      set_color(fgBLUE);
      PRINT1("\nElapsed: %f\n", d);
      reset_color();
    }
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
    fputc(bloc::Parser::NewLine, STDOUT);
    p.clear();
    return 1;
  }
  case CMD_DUMP:
  {
    p.pop();
    if (g_has_color)
      PRINTBUF(BOLD, sizeof(BOLD), STDOUT);
    PRINT("functions:\n");
    if (g_has_color)
      PRINTBUF(NORM, sizeof(NORM), STDOUT);
    FLUSHOUT;
    ctx.dumpFunctors();
    if (g_has_color)
      PRINTBUF(BOLD, sizeof(BOLD), STDOUT);
    PRINT("\nvariables:\n");
    if (g_has_color)
      PRINTBUF(NORM, sizeof(NORM), STDOUT);
    FLUSHOUT;
    ctx.dumpVariables();
    fputc(bloc::Parser::NewLine, STDOUT);
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
      fputc(bloc::Parser::Separator, STDOUT);
      fputc(bloc::Parser::NewLine, STDOUT);
    }
    fputc(bloc::Parser::NewLine, STDOUT);
    p.clear();
    return 1;
  }
  case CMD_EXPR:
  {
    /* perform inline expression */
    bloc::Expression * exp = nullptr;
    p.pop();
    try
    {
      exp = p.parseExpression();
      /* eat extra terminators */
      while ((t = p.pop())->code == bloc::Parser::Separator) { }
      if (t->code != bloc::Parser::NewLine)
        throw bloc::ParseError(bloc::EXC_PARSE_EXPRESSION_END_S, t->text.c_str());
      g_breaker.state = false;
      ctx.saveReturned(exp->value(ctx));
      if (g_breaker.state)
      {
        ctx.returnCondition(false);
        set_color(fgRED);
        PRINT("\nStopped\n");
        reset_color();
      }
      else
      {
        g_breaker.state = true;
        output_cli(ctx);
      }
      delete exp;
    }
    catch (bloc::Error& ee)
    {
      if (exp)
        delete exp;
      if (t->code != bloc::Parser::NewLine)
        p.clear();
      set_color(fgRED); PRINT1("Error: %s\n", ee.what()); reset_color();
    }
    return 1;
  }
  case CMD_LOAD:
  {
    std::string path;
    p.pop();
    t = p.pop();
    if (t->code == bloc::Parser::NewLine)
    {
      p.push(t);
      return 0;
    }
    else if (t->code == TOKEN_LITERALSTR)
    {
      bloc::Value str = bloc::Value::parseLiteral(t->text);
      path.assign(*str.literal());
      if (p.front()->code != bloc::Parser::NewLine)
        p.clear();
    }
    else
    {
      while (t->code != bloc::Parser::NewLine)
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
    try
    {
      ReadFile file(progfile);
      exec = bloc::Parser::parse(ctx, file, true);
    }
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
    if (t->code == bloc::Parser::NewLine)
    {
      p.push(t);
      return 0;
    }
    else if (t->code == TOKEN_LITERALSTR)
    {
      bloc::Value str = bloc::Value::parseLiteral(t->text);
      path.assign(*str.literal());
      if (p.front()->code != bloc::Parser::NewLine)
        p.clear();
    }
    else
    {
      while (t->code != bloc::Parser::NewLine)
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
      fputc(bloc::Parser::Separator, progfile);
      fputc(bloc::Parser::NewLine, progfile);
    }
    ::fclose(progfile);
    return 1;
  }
  case CMD_SYSTEM:
  {
    std::string cmd;
    p.pop();
    t = p.pop();
    if (t->code == bloc::Parser::NewLine)
    {
      p.push(t);
      return 0;
    }
    while (t->code != bloc::Parser::NewLine)
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
    if (t->code == bloc::Parser::NewLine)
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
    if (t->code != bloc::Parser::NewLine)
      p.clear();
    return 1;
  }
  default:
    return 0;
  }
}

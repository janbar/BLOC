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

#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <algorithm> // std::find
#include <string>
#include <vector>

#include <blocc/parser.h>
#include <blocc/exception_parse.h>
#include <blocc/exception_runtime.h>
#include <blocc/string_reader.h>
#include <blocc/plugin_manager.h>
#include <blocc/debug.h>

#include "main_options.h"
#include "read_file.h"
#include "cli_parser.h"
#include "usage.h"

#if (defined(_WIN32) || defined(_WIN64))
#define __WINDOWS__
#define STDOUT stdout
#define STDERR stderr
#define FLUSHOUT fflush(STDOUT)
#define PRINT(a) fputs(a, STDOUT)
#define PRINT1(a,b) fprintf(STDOUT, a, b)
#define PRINT2(a,b,c) fprintf(STDOUT, a, b, c)
#else
#define STDOUT stdout
#define STDERR stderr
#define FLUSHOUT fflush(STDOUT)
#define PRINT(a) fputs(a, STDOUT)
#define PRINT1(a,b) fprintf(STDOUT, a, b)
#define PRINT2(a,b,c) fprintf(STDOUT, a, b, c)
#endif

static bool output(bloc::Context& ctx);

#ifdef __WINDOWS__
/* MSC handler for C runtime */
static void on_invalid_parameter(
    const wchar_t * expression, const wchar_t * function,
    const wchar_t * file, unsigned int line, uintptr_t p_reserved)
{
  bloc::DBG(DBG_WARN, "An invalid parameter was specified.");
}
#endif

int main(int argc, char **argv) {

#ifdef __WINDOWS__
  /* configure the main handler for C runtime */
  (void) _set_invalid_parameter_handler(on_invalid_parameter);
#endif

  bool ret = true;
  bloc::DBGLevel(DBG_WARN);
  FILE * outfile = STDOUT;

  MainOptions options;
  std::vector<std::string> prog;
  const char * bad = getCmd(argv + 1, argv + argc, options, prog);
  if (bad != nullptr)
  {
    std::string tmp(bad);
    if (tmp == "-h" || tmp == "--help")
      fwrite(usage_txt, 1, usage_txt_len, STDOUT);
    else
      PRINT1("Unknown command option: %s\n", bad);
    return EXIT_FAILURE;
  }

  if (options.debug)
  {
    if (options.dbg_hints == "all")
      bloc::DBGLevel(DBG_ALL);
    else
      bloc::DBGLevel(DBG_DEBUG);
  }

  /* run command-line */
  if (options.docli || prog.empty())
  {
    cli_parser(options, prog);
  }
  /* processing expression */
  else if (options.doexp)
  {
    bloc::StringReader reader;
    for (const auto& arg : prog)
      reader.append(arg).append(" ");
    /* mark the end of expression: could be semi-colon or nl */
    reader.append(";");
    bloc::Context ctx(::fileno(STDOUT), ::fileno(STDERR));
    bloc::Parser * p = bloc::Parser::createInteractiveParser(ctx, &reader, bloc::StringReader::read_input);
    try
    {
      bloc::Expression * exp = p->parseExpression();
      ctx.saveReturned(exp);
      ret = output(ctx);
    }
    catch (bloc::Error& ee)
    {
      ret = false;
      fprintf(ctx.ctxerr(), "Error: %s\n", ee.what());
      fflush(ctx.ctxerr());
    }
    delete p;
  }
  /* running program */
  else
  {
    FILE * progfile;
    /* setup output stream */
    if (!options.file_sout.empty())
    {
      outfile = ::fopen(options.file_sout.c_str(), "w");
      if (!outfile)
      {
        PRINT1("Failed to open file '%s' for write.", options.file_sout.c_str());
        return EXIT_FAILURE;
      }
    }
    /* setup prog stream */
    if (prog[0] == "-")
      progfile = stdin;
    else
      progfile = ::fopen(prog[0].c_str(), "r");
    if (!progfile)
    {
      PRINT1("Failed to open file '%s' for read.", prog[0].c_str());
      ::fclose(outfile);
      return EXIT_FAILURE;
    }

    bloc::Context ctx(::fileno(outfile), ::fileno(STDERR));
    /* load args values into context as variables $0..$n */
    for (int i = 0; i < prog.size(); ++i)
    {
      const bloc::Symbol& symbol = ctx.registerSymbol(std::string("$").append(std::to_string(i)), bloc::Type::LITERAL);
      ctx.storeVariable(symbol, bloc::LiteralExpression(prog[i]));
    }
    bloc::Executable * exec = nullptr;
    try { exec = bloc::Parser::parse(ctx, progfile, &read_file); }
    catch (bloc::ParseError& pe)
    {
      fprintf(STDERR, "Error: %s\n", pe.what());
    }
    /* keep streams open until exit, as they could be one of stdin, stdout */
    if (!exec)
    {
      /* fclose all */
      return EXIT_FAILURE;
    }
    try
    {
      exec->run();
      ret = output(exec->context());
    }
    catch (bloc::RuntimeError& re)
    {
      ret = false;
      fprintf(ctx.ctxerr(), "Error: %s\n", re.what());
      fflush(ctx.ctxerr());
    }
    delete exec;
  }

  bloc::PluginManager::destroy();
  /* fclose all */
  return (ret ? EXIT_SUCCESS : EXIT_FAILURE);
}

static bool output(bloc::Context& ctx)
{
  bool ret = true;
  bloc::Expression * exp = ctx.dropReturned();
  if (exp)
  {
    try
    {
      if (exp->type(ctx).level() == 0)
      {
        switch (exp->type(ctx).major())
        {
        case bloc::Type::BOOLEAN:
          ::fputs(bloc::BooleanExpression::readableBoolean(exp->boolean(ctx)).c_str(), ctx.ctxout());
          break;
        case bloc::Type::INTEGER:
          ::fputs(bloc::IntegerExpression::readableInteger(exp->integer(ctx)).c_str(), ctx.ctxout());
          break;
        case bloc::Type::NUMERIC:
          ::fputs(bloc::NumericExpression::readableNumeric(exp->numeric(ctx)).c_str(), ctx.ctxout());
          break;
        case bloc::Type::LITERAL:
          ::fputs(exp->literal(ctx).c_str(), ctx.ctxout());
          break;
        case bloc::Type::ROWTYPE:
          ::fputs(bloc::TupleExpression::readableTuple(exp->tuple(ctx), ctx).c_str(), ctx.ctxout());
          break;
        default:
          /* not printable */
         break;
        }
        ::fflush(ctx.ctxout());
      }
    }
    catch(bloc::RuntimeError& re)
    {
      ret = false;
      PRINT1("Error: %s\n", re.what());
    }
    delete exp;
    ctx.purgeWorkingMemory();
  }
  return ret;
}

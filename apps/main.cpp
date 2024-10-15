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
#include <forward_list>

#include <blocc/parser.h>
#include <blocc/exception_parse.h>
#include <blocc/exception_runtime.h>
#include <blocc/string_reader.h>
#include <blocc/plugin_manager.h>
#include <blocc/collection.h>
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

struct AutoFILEClose final
{
  FILE * file = nullptr;
  ~AutoFILEClose() { if (file) ::fclose(file); }
  AutoFILEClose(FILE * f) : file(f) { }
  AutoFILEClose(const AutoFILEClose&) = delete;
  AutoFILEClose& operator=(const AutoFILEClose&) = delete;
  AutoFILEClose(AutoFILEClose&& o) noexcept
  {
    file = o.file;
    o.file = nullptr;
  }
};

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
  std::vector<bloc::Value> prog;
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
    cli_parser(options, std::move(prog));
  }
  /* processing expression */
  else if (options.doexp)
  {
    bloc::StringReader reader;
    for (bloc::Value& arg : prog)
      reader.append(*arg.literal()).append(" ");
    /* mark the end of expression: could be semi-colon or nl */
    reader.append(";");
    bloc::Context ctx(::fileno(STDOUT), ::fileno(STDERR));
    bloc::Parser * p = bloc::Parser::createInteractiveParser(ctx, &reader, bloc::StringReader::token_read);
    bloc::Expression * exp = nullptr;
    try
    {
      exp = p->parseExpression();
      ctx.saveReturned(exp->value(ctx));
      ret = output(ctx);
      delete exp;
    }
    catch (bloc::Error& ee)
    {
      if (exp)
        delete exp;
      ret = false;
      fprintf(ctx.ctxerr(), "Error: %s\n", ee.what());
      fflush(ctx.ctxerr());
    }
    delete p;
  }
  /* running program */
  else
  {
    std::forward_list<AutoFILEClose> open_files;
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
      open_files.push_front(AutoFILEClose(outfile));
    }
    /* setup prog stream */
    if (*prog[0].literal() == "-")
      progfile = stdin;
    else
    {
      progfile = ::fopen(prog[0].literal()->c_str(), "r");
      if (!progfile)
      {
        PRINT1("Failed to open file '%s' for read.", prog[0].literal()->c_str());
        return EXIT_FAILURE;
      }
      open_files.push_front(AutoFILEClose(progfile));
    }

    bloc::Context ctx(::fileno(outfile), ::fileno(STDERR));
    /* set context as trusted to allow use of restricted plugins */
    ctx.trusted(true);

    /* load arguments 1..n into the context, as table named $ARG */
    bloc::Collection * c_arg = new bloc::Collection(bloc::Value::type_literal.levelUp());
    for (auto it = ++prog.begin(); it != prog.end(); ++it)
      c_arg->push_back(std::move(*it));
    const bloc::Symbol& c_sym = ctx.registerSymbol(std::string("$ARG"), c_arg->table_type());
    ctx.storeVariable(c_sym, bloc::Value(c_arg));

    bloc::Executable * exec = nullptr;
    try { exec = bloc::Parser::parse(ctx, progfile, &read_file); }
    catch (bloc::ParseError& pe)
    {
      fprintf(STDERR, "Error: %s\n", pe.what());
    }
    if (!exec)
      return EXIT_FAILURE;

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
  return (ret ? EXIT_SUCCESS : EXIT_FAILURE);
}

static bool output(bloc::Context& ctx)
{
  bool ret = true;
  bloc::Value * val = ctx.dropReturned();
  if (val != nullptr)
  {
    try
    {
      if (val->isNull())
      {
        ::fputs(bloc::Value::STR_NIL, ctx.ctxout());
      }
      else if (val->type().level() == 0)
      {
        switch (val->type().major())
        {
        case bloc::Type::BOOLEAN:
          ::fputs(bloc::Value::readableBoolean(*(val->boolean())).c_str(), ctx.ctxout());
          break;
        case bloc::Type::INTEGER:
          ::fputs(bloc::Value::readableInteger(*(val->integer())).c_str(), ctx.ctxout());
          break;
        case bloc::Type::NUMERIC:
          ::fputs(bloc::Value::readableNumeric(*(val->numeric())).c_str(), ctx.ctxout());
          break;
        case bloc::Type::LITERAL:
          ::fputs(val->literal()->c_str(), ctx.ctxout());
          break;
        case bloc::Type::ROWTYPE:
          ::fputs(bloc::Value::readableTuple(*(val->tuple())).c_str(), ctx.ctxout());
          break;
        case bloc::Type::IMAGINARY:
          ::fputs(bloc::Value::readableImaginary(*(val->imaginary())).c_str(), ctx.ctxout());
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
    delete val;
    ctx.purgeWorkingMemory();
  }
  return ret;
}

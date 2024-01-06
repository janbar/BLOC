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

#include "bloc_capi.h"
#include "context.h"
#include "parser.h"
#include "expression.h"
#include "exception_runtime.h"
#include "exception_parse.h"
#include "parse_expression.h"
#include "parse_statement.h"
#include "string_reader.h"
#include "collection.h"
#include "tuple.h"
#include "value.h"

#define to_bool(a) (a == bloc_true ? true : false)

static struct { const char * msg; int no; } bloc_error = { "", 0 };

const char*
bloc_strerror() {
  return bloc_error.msg;
}

int
bloc_errno() {
  return bloc_error.no;
}

static void
bloc_error_raz()
{
  bloc_error = { "", 0 };
}

static void
bloc_error_set(const char *msg, int no)
{
  bloc_error.msg = msg;
  bloc_error.no = no;
}

const char*
bloc_version() { return bloc::Context::version(); }

const char*
bloc_version_header() { return bloc::Context::versionHeader(); }

int
bloc_compatible() { return bloc::Context::compatible(); }

void
bloc_free_context(BLOC_CONTEXT *ctx)
{
  if (ctx)
    delete reinterpret_cast<bloc::Context*>(ctx);
}

BLOC_CONTEXT*
bloc_create_context(int fd_out, int fd_err)
{
  return reinterpret_cast<BLOC_CONTEXT*>(new bloc::Context(fd_out, fd_err));
}

void
bloc_ctx_purge(BLOC_CONTEXT *ctx)
{
  reinterpret_cast<bloc::Context*>(ctx)->purge();
}

BLOC_SYMBOL*
bloc_ctx_register_symbol(BLOC_CONTEXT *ctx, const char *name, bloc_type type)
{
  bloc::Type t((bloc::Type::TypeMajor)type.major, 0, type.ndim);
  try
  {
    bloc::Symbol& s = reinterpret_cast<bloc::Context*>(ctx)->registerSymbol(name, t);
    return reinterpret_cast<BLOC_SYMBOL*>(&s);
  }
  catch (bloc::ParseError& pe)
  {
    bloc_error_set(pe.what(), pe.no);
    return 0;
  }
}

bloc_bool
bloc_ctx_store_variable(BLOC_CONTEXT *ctx, const BLOC_SYMBOL *symbol, BLOC_VALUE *v)
{
  try
  {
    reinterpret_cast<bloc::Context*>(ctx)->storeVariable(
            *reinterpret_cast<const bloc::Symbol*>(symbol),
            std::move(*reinterpret_cast<bloc::Value*>(v)));
    return bloc_true;
  }
  catch (bloc::RuntimeError& re)
  {
    bloc_error_set(re.what(), re.no);
    return bloc_false;
  }
}

BLOC_SYMBOL*
bloc_ctx_find_symbol(BLOC_CONTEXT *ctx, const char *name)
{
  return reinterpret_cast<BLOC_SYMBOL*>(reinterpret_cast<bloc::Context*>(ctx)->findSymbol(name));
}

BLOC_VALUE*
bloc_ctx_load_variable(BLOC_CONTEXT *ctx, const BLOC_SYMBOL *symbol)
{
  bloc::Value& v = reinterpret_cast<bloc::Context*>(ctx)->loadVariable(
          *reinterpret_cast<const bloc::Symbol*>(symbol));
  return reinterpret_cast<BLOC_VALUE*>(&v);
}

void
bloc_ctx_enable_trace(BLOC_CONTEXT *ctx, bloc_bool yesno)
{
  reinterpret_cast<bloc::Context*>(ctx)->trace(to_bool(yesno));
}

bloc_bool
bloc_ctx_trace(BLOC_CONTEXT *ctx)
{
  return (reinterpret_cast<bloc::Context*>(ctx)->trace() ? bloc_true : bloc_false);
}

void
bloc_ctx_purge_working_mem(BLOC_CONTEXT *ctx)
{
  reinterpret_cast<bloc::Context*>(ctx)->purgeWorkingMemory();
}

FILE*
bloc_ctx_out(BLOC_CONTEXT *ctx)
{
  return reinterpret_cast<bloc::Context*>(ctx)->ctxout();
}

FILE*
bloc_ctx_err(BLOC_CONTEXT *ctx)
{
  return reinterpret_cast<bloc::Context*>(ctx)->ctxerr();
}

void
bloc_free_value(BLOC_VALUE *v)
{
  if (v)
    delete reinterpret_cast<bloc::Value*>(v);
}

BLOC_VALUE*
bloc_create_boolean(bloc_bool v)
{
  return reinterpret_cast<BLOC_VALUE*>(new bloc::Value(bloc::Bool(to_bool(v))));
}

BLOC_VALUE*
bloc_create_integer(int64_t v)
{
  return reinterpret_cast<BLOC_VALUE*>(new bloc::Value(bloc::Integer(v)));
}

BLOC_VALUE*
bloc_create_numeric(double v)
{
  return reinterpret_cast<BLOC_VALUE*>(new bloc::Value(bloc::Numeric(v)));
}

BLOC_VALUE*
bloc_create_literal(const char *v)
{
  return reinterpret_cast<BLOC_VALUE*>(new bloc::Value(new bloc::Literal(v)));
}


BLOC_VALUE*
bloc_create_tabchar(const char *v, unsigned len)
{
  bloc::TabChar * tv = new bloc::TabChar(len);
  tv->assign(v, v + len);
  return reinterpret_cast<BLOC_VALUE*>(new bloc::Value(tv));
}

bloc_type
bloc_expression_type(BLOC_CONTEXT *ctx, BLOC_EXPRESSION *e)
{
  const bloc::Type& type = reinterpret_cast<bloc::Expression*>(e)->type(*reinterpret_cast<bloc::Context*>(ctx));
  bloc_type t = { (bloc_type_major)type.major(), type.level() };
  return t;
}

BLOC_VALUE*
bloc_evaluate_expression(BLOC_CONTEXT *ctx, BLOC_EXPRESSION *e)
{
  try
  {
    bloc::Value& v = reinterpret_cast<bloc::Expression*>(e)->value(*reinterpret_cast<bloc::Context*>(ctx));
    return reinterpret_cast<BLOC_VALUE*>(&v);
  }
  catch (bloc::RuntimeError& re)
  {
    bloc_error_set(re.what(), re.no);
    return nullptr;
  }
}

bloc_type
bloc_value_type(BLOC_VALUE *v)
{
  const bloc::Type& type = reinterpret_cast<bloc::Value*>(v)->type();
  bloc_type t = { (bloc_type_major)type.major(), type.level() };
  return t;
}

bloc_bool
bloc_boolean(BLOC_VALUE* v, bloc_bool **buf)
{
  try
  {
    *buf = reinterpret_cast<bloc_bool*>(reinterpret_cast<bloc::Value*>(v)->boolean());
    return bloc_true;
  }
  catch (bloc::RuntimeError& re)
  {
    bloc_error_set(re.what(), re.no);
    return bloc_false;
  }
}

bloc_bool
bloc_integer(BLOC_VALUE* v, int64_t **buf)
{
  try
  {
    *buf = reinterpret_cast<bloc::Value*>(v)->integer();
    return bloc_true;
  }
  catch (bloc::RuntimeError& re)
  {
    bloc_error_set(re.what(), re.no);
    return bloc_false;
  }
}

bloc_bool
bloc_numeric(BLOC_VALUE* v, double **buf)
{
  try
  {
    *buf = reinterpret_cast<bloc::Value*>(v)->numeric();
    return bloc_true;
  }
  catch (bloc::RuntimeError& re)
  {
    bloc_error_set(re.what(), re.no);
    return bloc_false;
  }
}

bloc_bool
bloc_literal(BLOC_VALUE* v, const char **buf)
{
  try
  {
    bloc::Literal * str = reinterpret_cast<bloc::Value*>(v)->literal();
    *buf = str->data();
    return bloc_true;
  }
  catch (bloc::RuntimeError& re)
  {
    bloc_error_set(re.what(), re.no);
    return bloc_false;
  }
}

bloc_bool
bloc_tabchar(BLOC_VALUE* v, const char **buf, unsigned *len)
{
  try
  {
    bloc::TabChar * tc = reinterpret_cast<bloc::Value*>(v)->tabchar();
    *buf = tc->data();
    *len = (unsigned)tc->size();
    return bloc_true;
  }
  catch (bloc::RuntimeError& re)
  {
    bloc_error_set(re.what(), re.no);
    return bloc_false;
  }
}

BLOC_EXPRESSION*
bloc_parse_expression(BLOC_CONTEXT *ctx, const char *text)
{
  bloc::Context& _ctx = *reinterpret_cast<bloc::Context*>(ctx);
  bloc::StringReader reader(text);
  bloc::Parser * p = bloc::Parser::createInteractiveParser(_ctx, &reader, bloc::StringReader::read_input);
  try
  {
    bloc_error_raz();
    bloc::Expression * e = bloc::ParseExpression::expression(*p, _ctx);
    delete p;
    return reinterpret_cast<BLOC_EXPRESSION*>(e);
  }
  catch (bloc::ParseError& pe)
  {
    delete p;
    bloc_error_set(pe.what(), pe.no);
    return nullptr;
  }
}

void
bloc_free_expression(BLOC_EXPRESSION *e)
{
  if (e)
    delete reinterpret_cast<bloc::Expression*>(e);
}

void
bloc_free_executable(BLOC_EXECUTABLE *exec)
{
  if (exec)
    delete reinterpret_cast<bloc::Executable*>(exec);
}

BLOC_EXECUTABLE*
bloc_parse_executable(BLOC_CONTEXT *ctx, const char *text)
{
  try
  {
    bloc_error_raz();
    bloc::Context& _ctx = *reinterpret_cast<bloc::Context*>(ctx);
    bloc::StringReader reader(text);
    bloc::Executable * x = bloc::Parser::parse(_ctx, &reader, bloc::StringReader::read_input);
    return reinterpret_cast<BLOC_EXECUTABLE*>(x);
  }
  catch (bloc::ParseError& pe)
  {
    bloc_error_set(pe.what(), pe.no);
    return nullptr;
  }
}

bloc_bool
bloc_execute(BLOC_EXECUTABLE *exec)
{
  try
  {
    return (reinterpret_cast<bloc::Executable*>(exec)->run() == 0 ? bloc_true : bloc_false);
  }
  catch (bloc::RuntimeError& re)
  {
    bloc_error_set(re.what(), re.no);
    return bloc_false;
  }
}

BLOC_VALUE*
bloc_drop_returned(BLOC_CONTEXT *ctx)
{
  return reinterpret_cast<BLOC_VALUE*>(reinterpret_cast<bloc::Context*>(ctx)->dropReturned());
}

/*
 *      Copyright (C) 2023 Jean-Luc Barriere
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

#include "statement_function.h"
#include "exception_parse.h"
#include "parse_expression.h"
#include "parse_statement.h"
#include "expression_variable.h"
#include "parser.h"
#include "context.h"
#include "statement_begin.h"
#include "functor_manager.h"
#include "plugin_manager.h"
#include "debug.h"

#include <string>
#include <cassert>

namespace bloc
{

FUNCTIONStatement::~FUNCTIONStatement()
{
}

const Statement * FUNCTIONStatement::doit(Context& ctx) const
{
  FunctorManager::entry fe = FunctorManager::instance().findDeclaration(_functor->name, _functor->params.size());
  if (fe == FunctorManager::instance().npos())
    throw RuntimeError(EXC_RT_INTERNAL_ERROR_S, _functor->name.c_str());
  *fe = _functor;
  return _next;
}

void FUNCTIONStatement::unparse(Context& ctx, FILE * out) const
{
  fputs(Statement::KEYWORDS[keyword()], out);
  fputc(' ', out);
  fputs(_functor->name.c_str(), out);
  if (!_functor->params.empty())
  {
    fputc('(', out);
    bool chained = false;
    for (Symbol& s : _functor->params)
    {
      if (chained)
        fputc(Parser::CHAIN, out);
      fputs(s.name().c_str(), out);
      chained = true;
    }
    fputc(')', out);
  }
  fputs(" returns ", out);
  if (_functor->returns.level() > 0)
    fputs("table", out);
  else
  {
    if (_functor->returns.major() == Type::COMPLEX && _functor->returns.minor() != 0)
      fputs(PluginManager::instance().plugged(_functor->returns.minor()).interface.name, out);
    else
      fputs(_functor->returns.typeName().c_str(), out);
  }
  fputs(" is", out);
  fputc(Parser::NEWLINE, out);
  _functor->body->unparse(*_functor->ctx, out);
}

FUNCTIONStatement * FUNCTIONStatement::parse(Parser& p, Context& ctx)
{
  bool rollback = false;
  try
  {
    if (ctx.execLevel() > 0)
      throw ParseError(EXC_PARSE_OTHER_S, "A function cannot be defined in nested block.");
    FunctorPtr fct(new Functor());
    TokenPtr t = p.pop();
    if (t->code != TOKEN_KEYWORD)
      throw ParseError(EXC_PARSE_OTHER_S, "name required for FUNCTION.");
    fct->name = t->text;
    std::transform(fct->name.begin(), fct->name.end(), fct->name.begin(), ::toupper);

    /* create the private context of the functor */
    fct->ctx = new Context(::fileno(ctx.ctxout()), ::fileno(ctx.ctxerr()));
    /* parsing mode is always enabled for the private context */
    fct->ctx->parsingBegin();

    t = p.pop();
    if (t->code == '(')
    {
      do
      {
        t = p.pop();
        if (t->code != TOKEN_KEYWORD)
          throw ParseError(EXC_PARSE_UNEXPECTED_LEX_S, t->text.c_str());
        std::transform(t->text.begin(), t->text.end(), t->text.begin(), ::toupper);
        /* register the symbol for later use */
        fct->params.push_back(fct->ctx->registerSymbol(t->text, Type::NO_TYPE));
        t = p.pop();
      } while (t->code == Parser::CHAIN);
      if (t->code != ')')
        throw ParseError(EXC_PARSE_MM_PARENTHESIS);
      t = p.pop();
    }

    /* returns */
    if (t->code != TOKEN_KEYWORD || t->text != KEYWORDS[STMT_RETURNS])
      throw ParseError(EXC_PARSE_OTHER_S, "Keyword RETURNS required for FUNCTION.");
    t = p.pop();
    if (t->code != TOKEN_KEYWORD)
      throw ParseError(EXC_PARSE_UNEXPECTED_LEX_S, t->text.c_str());
    if (t->text == "table")
    {
      /* opaque table */
      fct->returns = Type(Type::NO_TYPE).levelUp();
    }
    else
    {
      Type::TypeMinor typeId = PluginManager::instance().findModuleTypeId(t->text);
      if (typeId != 0)
        fct->returns = Type(Type::COMPLEX, typeId);
      else
      {
        fct->returns = Type::nameType(t->text);
        if (fct->returns == Type::NO_TYPE)
          throw ParseError(EXC_PARSE_UNDEFINED_SYMBOL_S, t->text.c_str());
      }
    }

    /* is */
    t = p.pop();
    if (t->code != TOKEN_KEYWORD || t->text != KEYWORDS[STMT_IS])
      throw ParseError(EXC_PARSE_OTHER_S, "Keyword IS required for FUNCTION.");
    t = p.pop();
    if (t->code != TOKEN_KEYWORD || t->text != KEYWORDS[STMT_BEGIN])
      throw ParseError(EXC_PARSE_OTHER_S, "Keyword BEGIN required for FUNCTION.");

    /* before parsing the body, declare itself to allow recursion.
     * the previous declaration is backed up, and should be restored when
     * throwing any exception */
    FunctorManager::entry fe = FunctorManager::instance().createOrReplace(fct->name, fct->params);
    rollback = true;
    fe->swap(fct);
    /* parse body or throws */
    (*fe)->body = BEGINStatement::parse(p, *((*fe)->ctx));
    return new FUNCTIONStatement(*fe);
  }
  catch (ParseError& pe)
  {
    DBG(DBG_DEBUG, "exception %p at %s line %d\n", &pe, __PRETTY_FUNCTION__, __LINE__);
    if (rollback)
      FunctorManager::instance().rollback();
    throw;
  }
  return nullptr;
}

}

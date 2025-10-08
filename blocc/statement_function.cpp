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
  FunctorManager::entry fe = ctx.functorManager().findDeclaration(_functor->name, _functor->params.size());
  if (fe == ctx.functorManager().npos())
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
      /* parameter name */
      if (chained)
        fputc(Parser::Chain, out);
      fputs(s.name().c_str(), out);
      /* type declaration */
      if (s.level() > 0)
        fputs(":table", out);
      else if (s.major() == Type::COMPLEX && s.minor() != 0)
      {
        fputs(":", out);
        fputs(PluginManager::instance().plugged(s.minor()).interface.name, out);
      }
      else if (s.major() != Type::NO_TYPE)
      {
        fputs(":", out);
        fputs(s.typeName().c_str(), out);
      }
      chained = true;
    }
    fputc(')', out);
  }
  fputc(' ', out);
  fputs(Statement::KEYWORDS[Statement::STMT_RETURN], out);
  fputc(' ', out);
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
  fputc(Parser::NewLine, out);
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
      throw ParseError(EXC_PARSE_OTHER_S, "name required for FUNCTION.", t);
    fct->name = t->text;
    std::transform(fct->name.begin(), fct->name.end(), fct->name.begin(), ::toupper);

    /* create the private context of the functor as child context */
    fct->ctx = ctx.createChild();
    /* parsing mode is always enabled for the private context */
    fct->ctx->parsingBegin();

    t = p.pop();
    if (t->code == '(')
    {
      if (p.front()->code == ')')
        t = p.pop();
      else
      {
        /* parsing the declaration of parameters */
        do
        {
          t = p.pop();
          if (t->code != TOKEN_KEYWORD)
            throw ParseError(EXC_PARSE_UNEXPECTED_LEX_S, t->text.c_str(), t);
          std::string param_name = t->text;
          std::transform(param_name.begin(), param_name.end(), param_name.begin(), ::toupper);
          t = p.pop();
          /* check if a type declaration follows the symbol */
          if (t->code == Colon)
          {
            t = p.pop();
            if (t->code != TOKEN_KEYWORD)
              throw ParseError(EXC_PARSE_UNEXPECTED_LEX_S, t->text.c_str(), t);
            if (t->text == "table")
              /* register type table opaque */
              fct->params.push_back(fct->ctx->registerSymbol(param_name, Type(Type::NO_TYPE).levelUp()));
            else
            {
              Type::TypeMinor typeId = PluginManager::instance().findModuleTypeId(t->text);
              if (typeId != 0)
                fct->params.push_back(fct->ctx->registerSymbol(param_name, Type(Type::COMPLEX, typeId)));
              else
              {
                /* typeName returns undefined for unknown type, so check the name back */
                const Type tmp = Type::nameType(t->text);
                if (t->text == Type::typeName(tmp.major()))
                  fct->params.push_back(fct->ctx->registerSymbol(param_name, tmp));
                else
                  throw ParseError(EXC_PARSE_UNDEFINED_SYMBOL_S, t->text.c_str(), t);
              }
            }
            t = p.pop();
          }
          else
          {
            /* register type opaque */
            fct->params.push_back(fct->ctx->registerSymbol(param_name, Type::NO_TYPE));
          }
        } while (t->code == Parser::Chain);
        if (t->code != ')')
          throw ParseError(EXC_PARSE_MM_PARENTHESIS, t);
      }
      t = p.pop();
    }

    /* returns */
    if (t->code != TOKEN_KEYWORD || t->text != KEYWORDS[STMT_RETURN])
      throw ParseError(EXC_PARSE_OTHER_S, "Keyword RETURN required for FUNCTION.", t);
    t = p.pop();
    if (t->code != TOKEN_KEYWORD)
      throw ParseError(EXC_PARSE_UNEXPECTED_LEX_S, t->text.c_str(), t);
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
        /* typeName returns undefined for unknown type, so check the name back */
        const Type tmp = Type::nameType(t->text);
        if (t->text == Type::typeName(tmp.major()))
          fct->returns = tmp;
        else
          throw ParseError(EXC_PARSE_UNDEFINED_SYMBOL_S, t->text.c_str(), t);
      }
    }

    /* is */
    t = p.pop();
    if (t->code != TOKEN_KEYWORD || t->text != KEYWORDS[STMT_IS])
      throw ParseError(EXC_PARSE_OTHER_S, "Keyword IS required for FUNCTION.", t);
    t = p.pop();
    if (t->code != TOKEN_KEYWORD || t->text != KEYWORDS[STMT_BEGIN])
      throw ParseError(EXC_PARSE_OTHER_S, "Keyword BEGIN required for FUNCTION.", t);

    /* before parsing the body, declare itself to allow recursion.
     * the previous declaration is backed up, and should be restored when
     * throwing any exception */
    FunctorManager::entry fe = ctx.functorManager().createOrReplace(fct->name, fct->params);
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
      ctx.functorManager().rollback();
    throw;
  }
  return nullptr;
}

}

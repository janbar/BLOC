/*
 *      Copyright (C) 2024 Jean-Luc Barriere
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

#include "statement_letn.h"
#include "exception_parse.h"
#include "parse_expression.h"
#include "parser.h"
#include "context.h"
#include "debug.h"
#include "plugin_manager.h"

#include <string>
#include <cassert>

namespace bloc
{

const Statement * LETNStatement::doit(Context& ctx) const
{
  /* initialize or reset variable */
  if (ctx.loadVariable(*_var.symbol()).type() != Type::POINTER)
  {
    /* INITIALIZE */
    ctx.storeVariable(*_var.symbol(), std::move(Value(_typ)));
    return _next;
  }
  else
  /* update reference */
  {
    Value& ptd = ctx.loadVariable(*_var.symbol()).deref_value();
    /* values MUST be of the same type */
    if (_typ != ptd.type())
      throw RuntimeError(EXC_RT_TYPE_MISMATCH_S, ptd.typeName().c_str());
    ptd.swap(std::move(Value(_typ)));
    return _next;
  }
}

void LETNStatement::unparse(Context&ctx, FILE * out) const
{
  fputs(_var.symbol()->name().c_str(), out);
  fputs(":", out);
  if (_typ.level() > 0)
    fputs("table", out);
  else
    fputs(_typ.typeName().c_str(), out);
  unparse_next(ctx, out);
}

LETNStatement * LETNStatement::parse(Parser& p, Context& ctx)
{
  Expression * exp = nullptr;
  try
  {
    TokenPtr t = p.pop();
    if (t->code == TOKEN_KEYWORD)
    {
      /* check if keyword is reserved */
      if (Parser::reservedKeyword(t->text))
        throw ParseError(EXC_PARSE_RESERVED_WORD_S, t->text.c_str());
      std::string name = t->text;
      std::transform(name.begin(), name.end(), name.begin(), ::toupper);
      t = p.pop();
      if (t->code == ':')
      {
        t = p.pop();
        if (t->code != TOKEN_KEYWORD)
          throw ParseError(EXC_PARSE_UNEXPECTED_LEX_S, t->text.c_str());
        if (t->text == "table")
        {
          /* register type table opaque */
          Type tmp = Value::type_no_type.levelUp();
          return new LETNStatement(VariableExpression(ctx.registerSymbol(name, tmp)), tmp);
        }
        else
        {
          Type::TypeMinor typeId = PluginManager::instance().findModuleTypeId(t->text);
          if (typeId != 0)
          {
            Type tmp = Type(Type::COMPLEX, typeId);
            return new LETNStatement(VariableExpression(ctx.registerSymbol(name, tmp)), tmp);
          }
          else
          {
            /* typeName returns undefined for unknown type, so check the name back */
            const Type tmp = Type::nameType(t->text);
            if (t->text == Type::typeName(tmp.major()))
              return new LETNStatement(VariableExpression(ctx.registerSymbol(name, tmp)), tmp);
            else
              throw ParseError(EXC_PARSE_UNDEFINED_SYMBOL_S, t->text.c_str());
          }
        }
      }
    }
    throw ParseError(EXC_PARSE_NOT_A_STATEMENT);
  }
  catch (ParseError& pe)
  {
    DBG(DBG_DEBUG, "exception %p at %s line %d\n", &pe, __PRETTY_FUNCTION__, __LINE__);
    if (exp) delete exp;
    throw;
  }
  return nullptr;
}

}

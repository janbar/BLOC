/*
 *      Copyright (C) 2025 Jean-Luc Barriere
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

#include <algorithm>

#include "statement_include.h"
#include "exception_parse.h"
#include "parse_expression.h"
#include "parse_statement.h"
#include "parser.h"
#include "context.h"
#include "debug.h"

namespace
{
class ReadFile : public bloc::Parser::StreamReader
{
  FILE * _file;
public:
  explicit ReadFile(FILE * file) : _file(file) { }
  int read(bloc::Parser *, char * buf, int max_size) override
  {
    int read = 0;
    while (read < max_size)
    {
      if (::fread(&buf[read], sizeof(char), 1, _file) == 1)
      {
        // discard cr to fix source formated msdos
        if (buf[read] == '\r')
          continue;
        if (buf[read++] != '\n')
          continue;
      }
      break;
    }
    return read;
  }
};
}

namespace bloc
{

INCLUDEStatement::~INCLUDEStatement()
{
  if (_exp)
    delete _exp;
  if (_exec)
    delete _exec;
}

void INCLUDEStatement::unparse(Context& ctx, FILE * out) const
{
  fputs(Statement::KEYWORDS[keyword()], out);
  fputs(" ", out);
  fputs(_exp->unparse(ctx).c_str(), out);
}

void INCLUDEStatement::loadSource(Parser& p, Context& ctx)
{
  /* check nested block */
  if (ctx.execLevel() > 0)
    throw ParseError(EXC_PARSE_OTHER_S, "Cannot include another source in a nested block.");
  /* check permissions */
  if (!ctx.trusted())
    throw ParseError(EXC_PARSE_OTHER_S, "Including another source is restricted.");
  /* check nesting limit */
  if (p.nesting() > 2)
      throw ParseError(EXC_PARSE_OTHER_S, "Too many nested inclusions.");

  if (_exp == nullptr)
    throw ParseError(EXC_PARSE_INV_EXPRESSION);
  Value& val = _exp->value(ctx);
  if (val.isNull())
    throw ParseError(EXC_PARSE_INV_EXPRESSION);
  FILE * progfile = ::fopen(val.literal()->c_str(), "r");
  if (progfile == nullptr)
    throw ParseError(EXC_PARSE_OTHER_S, "Failed to open file for read.");

  Parser * np = nullptr;
  std::list<const Statement*> statements;

  try
  {
    ReadFile file(progfile);
    np = Parser::createInteractiveParser(ctx, file);
    if (np == nullptr)
      throw ParseError(EXC_PARSE_OTHER_S, "Internal error.");

    /* inherits */
    np->nesting(p.nesting() + 1);
    np->state(Parser::Parsing);

    for (;;)
    {
      TokenPtr t;
      try { t = np->front(); } catch (ParseError& ee) { break; }
      switch (t->code)
      {
      case Parser::Separator:
        np->pop();
        continue;
      default:
        break;
      }
      const Statement * s = ParseStatement::statement(*np, ctx);
      if (s != nullptr)
        statements.push_back(s);
    }
  }
  catch (ParseError& pe)
  {
    DBG(DBG_DEBUG, "exception %p at %s line %d\n", &pe, __PRETTY_FUNCTION__, __LINE__);
    if (np)
      delete np;
    for (auto s : statements)
      delete s;
    ::fclose(progfile);
    throw;
  }

  if (np)
    delete np;
  ::fclose(progfile);
  _exec = new Executable(ctx, statements);
}

INCLUDEStatement * INCLUDEStatement::parse(Parser& p, Context& ctx)
{
  INCLUDEStatement * s = nullptr;
  try
  {
    TokenPtr t = p.front();
    Expression * e = ParseExpression::expression(p, ctx);
    if (e->type(ctx) != Type::LITERAL)
    {
      delete e;
      throw ParseError(EXC_PARSE_NOT_LITERAL, t);
    }
    s = new INCLUDEStatement(e);
    s->loadSource(p, ctx);
    return s;
  }
  catch (ParseError& pe)
  {
    DBG(DBG_DEBUG, "exception %p at %s line %d\n", &pe, __PRETTY_FUNCTION__, __LINE__);
    if (s)
      delete s;
    throw;
  }
  return nullptr;
}

}

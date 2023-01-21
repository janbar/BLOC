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

#include "statement_import.h"
#include "exception_parse.h"
#include "parse_expression.h"
#include "parser.h"
#include "context.h"
#include "import_manager.h"

namespace bloc
{

IMPORTStatement::~IMPORTStatement()
{
  if (_exp)
    delete _exp;
}

void IMPORTStatement::unparse(Context& ctx, FILE * out) const
{
  fputs(Statement::KEYWORDS[keyword()], out);
  if (_exp != nullptr)
  {
    fputs(" ", out);
    fputs(_exp->unparse(ctx).c_str(), out);
  }
  else
  {
    fputs(" ", out);
    fputs(_name.c_str(), out);
  }
}

void IMPORTStatement::loadModule(Context& ctx)
{
  unsigned type_id = 0;
  if (_exp == nullptr)
  {
    if ((type_id = ImportManager::instance().importTypeByName(_name)) == 0)
      throw ParseError(EXC_PARSE_IMPORT_FAILED_S, _name.c_str());
  }
  else
  {
    std::string& path = _exp->literal(ctx);
    if ((type_id = ImportManager::instance().importTypeByPath(path.c_str())) == 0)
      throw ParseError(EXC_PARSE_IMPORT_FAILED_S, path.c_str());
  }
  const IMPORT_MODULE& module = ImportManager::instance().module(type_id);
  DBG(DBG_DEBUG, "%s: id=%d name=%s instance=%p dlhandle=%p\n", __FUNCTION__,
          type_id, module.interface.name, module.instance, module.dlhandle);
}

IMPORTStatement * IMPORTStatement::parse(Parser& p, Context& ctx)
{
  IMPORTStatement * s = nullptr;
  try
  {
    if (p.front()->code == TOKEN_KEYWORD)
    {
      s = new IMPORTStatement(p.front()->text);
      p.pop();
    }
    else
    {
      Expression * e = ParseExpression::expression(p, ctx);
      try
      {
        if (e->type(ctx) != Type::LITERAL)
          throw ParseError(EXC_PARSE_NOT_LITERAL);
      }
      catch (...)
      {
        delete e;
        throw;
      }
      s = new IMPORTStatement(e);
    }
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

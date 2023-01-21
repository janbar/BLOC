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

#include "expression_complex_ctor.h"
#include "parse_expression.h"
#include "exception_parse.h"
#include "import_manager.h"
#include "parser.h"

namespace bloc
{

ComplexCTORExpression::~ComplexCTORExpression()
{
  for (Expression * e : _args)
    delete e;
  _args.clear();
}

Complex ComplexCTORExpression::complex(Context& ctx) const
{
  Complex c(_type.minor());
  if (c.CTOR(ctx, _args))
    return c;
  throw RuntimeError(EXC_RT_CTOR_FAILED_S, unparse(ctx).c_str());
}

std::string ComplexCTORExpression::unparse(Context& ctx) const
{
  const IMPORT_MODULE& module = ImportManager::instance().module(_type.minor());
  std::string sb(module.interface.name);
  sb.append("(");
  for (Expression * e : _args)
    sb.append(e->unparse(ctx)).append(1, Parser::CHAIN);
  if (sb.back() == Parser::CHAIN)
    sb.back() = ')';
  else
    sb.push_back(')');
  return sb;
}

ComplexCTORExpression * ComplexCTORExpression::parse(Parser& p, Context& ctx, unsigned type_id)
{
  if (type_id == 0)
    throw ParseError(EXC_PARSE_NOT_COMPLEX);
  const IMPORT_MODULE& module = ImportManager::instance().module(type_id);
  TokenPtr t = p.pop();
  if (t->code != '(')
    throw ParseError(EXC_PARSE_INV_EXPRESSION);
  std::vector<Expression*> args;
  try
  {
    int i = 0;
    while (i < module.interface.ctor_args_count)
    {
      args.push_back(ParseExpression::expression(p, ctx));
      /* the expected type */
      Type m_arg_type = import::make_type(module.interface.ctor_args[i].type, type_id);
      /* check mode */
      switch (module.interface.ctor_args[i].mode)
      {
      case IMPORT_IN:
        /* assert type */
        ParseExpression::assertType(args.back(), m_arg_type, p, ctx, false);
        break;
      case IMPORT_INOUT:
      {
        /* assert type */
        ParseExpression::assertType(args.back(), m_arg_type, p, ctx, false);
        /* must be variable expression */
        VariableExpression * var = dynamic_cast<VariableExpression*>(args.back());
        if (var == nullptr)
          throw ParseError(EXC_PARSE_MEMB_ARG_TYPE_S, module.interface.name);
        break;
      }
      }

      if (++i == module.interface.ctor_args_count)
        break;
      t = p.pop();
      if (t->code != Parser::CHAIN)
        throw ParseError(EXC_PARSE_FUNC_ARG_NUM_S, module.interface.name);
    }
    t = p.pop();
    if (t->code != ')')
      throw ParseError(EXC_PARSE_FUNC_ARG_NUM_S, module.interface.name);
    return new ComplexCTORExpression(type_id, args);
  }
  catch (ParseError& pe)
  {
    DBG(DBG_DEBUG, "exception %p at %s line %d\n", &pe, __PRETTY_FUNCTION__, __LINE__);
    for (Expression * e : args)
      delete e;
    throw;
  }
  return nullptr;
}

}

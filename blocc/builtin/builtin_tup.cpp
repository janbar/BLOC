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

#include "builtin_tup.h"
#include <blocc/parse_expression.h>
#include <blocc/exception_parse.h>
#include <blocc/tuple.h>
#include <blocc/context.h>
#include <blocc/parser.h>
#include <blocc/debug.h>

namespace bloc
{

TUPExpression::TUPExpression(std::vector<Expression*>&& args, Context& ctx) : BuiltinExpression(FUNC_TUP, std::move(args))
{
  for (const Expression * e : _args)
    _decl.push_back(e->type(ctx));
  _type = _decl.make_type(0);
}

Value& TUPExpression::value(Context & ctx) const
{
  if (_args.empty())
    return ctx.allocate(Value(Value::type_rowtype));

  /* build items, then move */
  Tuple::container_t items;
  for (const Expression * a : _args)
  {
    Value& val = a->value(ctx); /* execute expression */
    if (val.type() == Type::NO_TYPE)
      throw RuntimeError(EXC_RT_COMPOUND_OPAQUE);
    if (val.lvalue())
      items.push_back(val.clone());
    else
      items.push_back(std::move(val));
  }
  return ctx.allocate(Value(new Tuple(std::move(items))));
}

TUPExpression * TUPExpression::parse(Parser& p, Context& ctx)
{
  std::vector<Expression*> args;

  try
  {
    TokenPtr t = p.pop();
    if (t->code != '(')
      throw ParseError(EXC_PARSE_FUNC_ARG_NUM_S, KEYWORDS[FUNC_TUP], t);
    if (p.front()->code != ')')
    {
      for (;;)
      {
        args.push_back(ParseExpression::expression(p, ctx));
        const Type& arg_type = args.back()->type(ctx);
        if (arg_type.level() > 0)
          throw ParseError(EXC_PARSE_FUNC_ARG_TYPE_S, KEYWORDS[FUNC_TUP], t);
        switch (arg_type.major())
        {
        case Type::NO_TYPE: /* opaque */
        case Type::BOOLEAN:
        case Type::INTEGER:
        case Type::NUMERIC:
        case Type::IMAGINARY:
        case Type::LITERAL:
        case Type::COMPLEX:
        case Type::TABCHAR:
          break;
        default:
          throw ParseError(EXC_PARSE_FUNC_ARG_TYPE_S, KEYWORDS[FUNC_TUP], t);
        }
        t = p.pop();
        if (t->code == Parser::Chain)
          continue;
        p.push(t);
        break;
      }
    }
    //if (args.size() < 1)
    //  throw ParseError(EXC_PARSE_FUNC_ARG_NUM_S, KEYWORDS[FUNC_TUP]);
    assertClosedFunction(p, ctx, FUNC_TUP);
    return new TUPExpression(std::move(args), ctx);
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

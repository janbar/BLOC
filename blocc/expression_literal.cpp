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

#include "expression_literal.h"

namespace bloc
{

const Type& LiteralExpression::type_static = Type(Type::LITERAL);
const std::string& LiteralExpression::null = std::string("");

LiteralExpression * LiteralExpression::parse(const std::string& text)
{
  LiteralExpression * exp = new LiteralExpression();
  exp->v.reserve(text.size());
  /* start on enclosure, then finally drop last enclosure */
  char pc = 0;
  bool bs = false;
  for (auto c : text)
  {
    if (!bs)
    {
      bs = (c == '"');
    }
    else if (pc == '\\')
    {
      pc = 0;
      switch (c)
      {
      case 'a':
        exp->v.push_back('\a');
        break;
      case 'b':
        exp->v.push_back('\b');
        break;
      case 'f':
        exp->v.push_back('\f');
        break;
      case 'n':
        exp->v.push_back('\n');
        break;
      case 'r':
        exp->v.push_back('\r');
        break;
      case 't':
        exp->v.push_back('\t');
        break;
      case '\\':
        exp->v.push_back('\\');
        break;
      default:
        pc = c;
      }
    }
    else if (pc == '"' && pc == c)
    {
      exp->v.push_back(c);
      pc = 0;
    }
    else if (pc != 0)
    {
      exp->v.push_back(pc);
      pc = c;
    }
    else
      pc = c;
  }
  return exp;
}

std::string LiteralExpression::readableLiteral(const std::string& s)
{
  std::string buf;
  buf.reserve(s.size() + 2);
  buf.append("\"");
  for (auto c : s)
  {
    switch(c)
    {
    case '\a':
      buf.append("\\a");
      break;
    case '\b':
      buf.append("\\b");
      break;
    case '\f':
      buf.append("\\f");
      break;
    case '\n':
      buf.append("\\n");
      break;
    case '\r':
      buf.append("\\r");
      break;
    case '\t':
      buf.append("\\t");
      break;
    case '\\':
      buf.append("\\\\");
      break;
    case '"':
      buf.append("\"\"");
      break;
    default:
      buf.push_back(c);
    }
  }
  buf.append("\"");
  return buf;
}

}

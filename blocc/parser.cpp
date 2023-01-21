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

#include "parser.h"
#include "context.h"
#include "debug.h"
#include "exception_parse.h"
#include "parse_statement.h"
#include "parse_expression.h"
#include "import_manager.h"
#include "expression_builtin.h"

#include <cstring>

namespace bloc
{

Parser::~Parser()
{
  close_scanner();
  _tokens.clear();
}

TokenPtr Parser::front()
{
  if (_tokens.empty())
  {
    TokenPtr t;
    if(!next_token(t))
      throw ParseError();
    push_back(t);
  }
  return _tokens.front();
}

TokenPtr Parser::pop() {
  TokenPtr t;
  if (_tokens.empty())
  {
    if(!next_token(t))
      throw ParseError();
    push_back(t);
  }
  t = _tokens.front();
  _tokens.pop_front();
  DBG(DBG_PROTO, "%s [%d] : %s\n", __FUNCTION__, t->code, t->text.c_str());
  return t;
}

void Parser::push(TokenPtr& t) {
  _tokens.push_front(t);
  DBG(DBG_PROTO, "%s [%d] : %s\n", __FUNCTION__, t->code, t->text.c_str());
}

void Parser::push_back(TokenPtr& t) {
  _tokens.push_back(t);
}

void Parser::clear() {
  _tokens.clear();
  /* discard tokens until new line */
  state(CLEAR);
}

bool Parser::popAny(TokenPtr& token) {
  int tc = 0;
  const char * ts;
  Token * t = nullptr;

  tokenizer_enable_space(_scanner);
  tokenizer_lex(_scanner, &tc, &ts);
  tokenizer_disable_space(_scanner);
  if (tc <= 0)
    return false;
  t = new Token(tc, ts);
  token.reset(t);
  return true;
}

bool Parser::init_scanner(void * reader_hdl, TOKEN_READER reader)
{
  close_scanner();
  _scanner = tokenizer_init(reader_hdl, reader);
  if (_scanner == 0)
    return false;
  tokenizer_disable_space(_scanner);
  return true;
}

void Parser::close_scanner()
{
  if (_scanner)
    tokenizer_free(_scanner);
}

Parser * Parser::createInteractiveParser(Context& ctx, void * reader_hdl, TOKEN_READER reader)
{
  Parser * p = new Parser(ctx);
  if (!p->init_scanner(reader_hdl, reader))
  {
    delete p;
    p = nullptr;
  }
  return p;
}

Parser * Parser::createInteractiveParser(Context& ctx, TOKEN_READER reader)
{
  Parser * p = new Parser(ctx);
  if (!p->init_scanner(p, reader))
  {
    delete p;
    p = nullptr;
  }
  return p;
}

Statement * Parser::parseStatement()
{
  Statement * s = nullptr;
  TokenPtr t;
  state(BEGIN);
  while ((t = front()))
  {
    switch (t->code)
    {
    /* at begining feedback NEWLINE */
    case NEWLINE:
      pop();
      return nullptr;
    case SEPARATOR:
      pop();
      continue;
    default:
      break;
    }
    state(PARSE);
    s = ParseStatement::statement(*this, _ctx);
    state(END);
    return s;
  }
  state(EXIT);
  return nullptr;
}

Expression * Parser::parseExpression()
{
  return ParseExpression::expression(*this, _ctx);
}

bool Parser::reservedKeyword(const std::string& text)
{
  return (
          Statement::findKeyword(text) != Statement::unknown ||
          BuiltinExpression::findKeyword(text) != BuiltinExpression::unknown ||
          ImportManager::instance().findModuleTypeId(text) != 0
          );
}

Executable * Parser::parse(Context& ctx, void * reader_hdl, TOKEN_READER reader, bool trace /*= false*/)
{
  Parser p(ctx);
  if (!p.init_scanner(reader_hdl, reader))
    return nullptr;

  p.enable_trace(trace);
  std::list<const Statement*> statements;

  p.state(PARSE);
  try
  {
    for (;;)
    {
      TokenPtr t;
      try { t = p.front(); } catch (ParseError& ee) { break; }
      switch (t->code)
      {
      case SEPARATOR:
        p.pop();
        continue;
      default:
        break;
      }
      const Statement * s = ParseStatement::statement(p, ctx);
      if (s != nullptr)
        statements.push_back(s);
    }
    return new Executable(ctx, statements);
  }
  catch (ParseError& pe)
  {
    /* break current trace line before print what */
    if (trace)
      fputc('\n', ctx.ctxerr());
    fprintf(ctx.ctxerr(), "Error: %s\n", pe.what());
    fflush(ctx.ctxerr());
  }
  return nullptr;
}

void Parser::enable_trace(bool b)
{
  _trace = b;
  if (b)
    tokenizer_enable_space(_scanner);
  else
    tokenizer_disable_space(_scanner);
}

bool Parser::next_token(TokenPtr& token) {
  int tc = 0;
  const char * ts;
  Token * t = nullptr;

  /* read until new token or a failure */
  while (t == nullptr)
  {
    tokenizer_lex(_scanner, &tc, &ts);
    if (tc <= 0)
      return false;

    /* discard incoming tokens until new line */
    if (state() == CLEAR)
    {
      if (tc == NEWLINE)
        state(BEGIN);
      continue;
    }

    switch (tc) {
    case NEWLINE:
      /* until new statement, let the caller handling NL  */
      if (state() != PARSE)
        t = new Token(tc, ts);
      break;
    case TOKEN_SPACE:
      break;
    case TOKEN_LITERALBEG:
      _string_buffer.clear();
      _string_buffer.append(ts);
      break;
    case TOKEN_LITERALSTR:
      _string_buffer.append(ts);
      break;
    case TOKEN_LITERALEND:
      _string_buffer.append(ts);
      t = new Token(TOKEN_LITERALSTR, _string_buffer);
      break;
    case TOKEN_COMMENTBEG:
      _string_buffer.clear();
      _string_buffer.append(ts);
      break;
    case TOKEN_COMMENTSTR:
      _string_buffer.append(ts);
      break;
    case TOKEN_COMMENTEND:
      _string_buffer.append(ts);
      //t = new Token(TOKEN_COMMENT, _string_buffer);
      break;
    case TOKEN_COMMENT:
    case TOKEN_DIRECTIVE:
      //t = new Token(TOKEN_COMMENT, ts);
      break;
    case TOKEN_INTEGER:
    case TOKEN_DOUBLE:
    case TOKEN_FLOAT:
    case TOKEN_KEYWORD:
    case TOKEN_ISEQUAL:
    case TOKEN_ISEQMORE:
    case TOKEN_ISEQLESS:
    case TOKEN_ISNOTEQ:
    case TOKEN_ASSIGN:
    case TOKEN_POPLEFT:
    case TOKEN_PUSHRIGHT:
    case TOKEN_INCREMENT:
    case TOKEN_DECREMENT:
    default:
      t = new Token(tc, ts);
      break;
    }

    /* forward token string to trace */
    if (_trace)
      fputs(ts, _ctx.ctxout());
  }

  token.reset(t);
  return true;
}

}

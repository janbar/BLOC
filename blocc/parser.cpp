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
#include "parse_statement.h"
#include "parse_expression.h"
#include "plugin_manager.h"
#include "expression_builtin.h"

#include <cstring>

namespace bloc
{

const char * Parser::Indent = "    ";

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
    _tokens.push_back(std::move(t));
  }
  return _tokens.front();
}

TokenPtr Parser::pop() {
  TokenPtr t;
  if (_tokens.empty())
  {
    if(!next_token(t))
      throw ParseError();
    _tokens.push_back(t);
  }
  t = _tokens.front();
  _tokens.pop_front();
  DBG(DBG_PROTO, "%s [%d] : %s\n", __FUNCTION__, t->code, t->text.c_str());
  return t;
}

void Parser::push(const TokenPtr& t) {
  _tokens.push_front(t);
  DBG(DBG_PROTO, "%s [%d] : %s\n", __FUNCTION__, t->code, t->text.c_str());
}

void Parser::clear() {
  _tokens.clear();
  /* discard tokens until new line */
  state(Cleared);
}

bool Parser::reservedKeyword(const std::string& text)
{
  // fill the cache on first call
  if (_cache_keywords.empty())
  {
    auto s = Statement::keywordSet();
    _cache_keywords.insert(s.begin(), s.end());
    auto b = BuiltinExpression::keywordSet();
    _cache_keywords.insert(b.begin(), b.end());
  }

  if (_cache_keywords.find(text) != _cache_keywords.end())
    return true;
  if (PluginManager::instance().findModuleTypeId(text) != 0)
  {
    // don't pin it in the cache, firstly because the found module could be
    // unloaded later, and secondly because this call will almost certainly
    // be followed by a branch ending with an error
    return true;
  }
  return false;
}

bool Parser::popAny(TokenPtr& token) {
  int tc = 0;
  const char * ts;
  Token * t = nullptr;
  tokenizer_lex(_scanner, &tc, &ts);
  if (tc <= 0)
    return false;
  t = new Token(tc, ts, 0, 0);
  token.reset(t);
  return true;
}

void Parser::token_read(void * hdl, char * buf, int * len, int max_size)
{
  Parser * p = static_cast<Parser*>(hdl);
  *len = p->_reader.read(p, buf, max_size);
}

bool Parser::init_scanner()
{
  close_scanner();
  _scanner = tokenizer_init(this, &token_read);
  if (_scanner == nullptr)
    return false;
  tokenizer_enable_space(_scanner);
  return true;
}

void Parser::close_scanner()
{
  if (_scanner)
    tokenizer_free(_scanner);
}

Parser * Parser::createInteractiveParser(Context& ctx, StreamReader& reader)
{
  Parser * p = new Parser(ctx, reader);
  if (!p->init_scanner())
  {
    delete p;
    p = nullptr;
  }
  return p;
}

Statement * Parser::parseStatement()
{
  TokenPtr t;
  state(Begin);
  while ((t = front()))
  {
    switch (t->code)
    {
    /* at begining feedback NEWLINE */
    case NewLine:
      pop();
      return nullptr;
    case Separator:
      pop();
      continue;
    default:
      break;
    }

    try
    {
      state(Parsing);
      _ctx.parsingBegin();
      Statement * s = ParseStatement::statement(*this, _ctx);
      _ctx.parsingEnd();
      state(End);
      return s;
    }
    catch (...)
    {
      _ctx.parsingEnd();
      state(End);
      throw;
    }
  }
  state(Aborted);
  return nullptr;
}

Expression * Parser::parseExpression()
{
  try
  {
    _ctx.parsingBegin();
    Expression * e = ParseExpression::expression(*this, _ctx);
    _ctx.parsingEnd();
    return e;
  }
  catch (...)
  {
    _ctx.parsingEnd();
    throw;
  }
}

Executable * Parser::parse(Context& ctx, StreamReader& reader, bool trace /*= false*/)
{
  Parser p(ctx, reader);
  if (!p.init_scanner())
    return nullptr;

  p.trace(trace);
  std::list<const Statement*> statements;

  p.state(Parsing);
  try
  {
    ctx.parsingBegin();
    for (;;)
    {
      TokenPtr t;
      try { t = p.front(); } catch (ParseError& ee) { break; }
      switch (t->code)
      {
      case Separator:
        p.pop();
        continue;
      default:
        break;
      }
      const Statement * s = ParseStatement::statement(p, ctx);
      if (s != nullptr)
        statements.push_back(s);
    }
    ctx.parsingEnd();
    if (trace && ctx.ctxerr())
      fflush(ctx.ctxerr());
    return new Executable(ctx, statements);
  }
  catch (ParseError& pe)
  {
    ctx.parsingEnd();
    for (auto s : statements)
      delete s;
    /* break current trace line */
    if (trace && ctx.ctxerr())
    {
      fputc('\n', ctx.ctxerr());
      fflush(ctx.ctxerr());
    }
    throw;
  }
  return nullptr;
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
    if (state() == Cleared)
    {
      if (tc == NewLine)
      {
        state(Begin);
        /* reset position */
        _position.lno += 1;
        _position.pno = 1;
      }
      else
      {
        /* reset the state */
        tokenizer_clear(_scanner);
      }
      continue;
    }

    switch (tc) {
    case NewLine:
      /* until new statement, let the caller handling NL  */
      if (state() != Parsing)
        t = new Token(tc, ts, _position.lno, _position.pno);
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
      t = new Token(TOKEN_LITERALSTR, _string_buffer, _position.lno, _position.pno);
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
      // comment is discarded
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
      t = new Token(tc, ts, _position.lno, _position.pno);
      break;
    }

    if (*ts == NewLine)
    {
      _position.lno += 1;
      _position.pno = 1;
    }
    else
      _position.pno += strlen(ts);

    /* forward token string to trace */
    if (_trace && _ctx.ctxerr())
      fputs(ts, _ctx.ctxerr());
  }

  token.reset(t);
  return true;
}

}

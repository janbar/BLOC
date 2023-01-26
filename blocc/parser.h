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

#ifndef PARSER_H_
#define PARSER_H_

#include "tokenizer.h"
#include "token.h"
#include "executable.h"
#include "statement.h"
#include "template_stack.h"

#include <list>

namespace bloc
{

class Parser {

public:
  static const char NEWLINE       = '\n';
  static const char SEPARATOR     = ';';
  static const char CHAIN         = ',';
  static const char * INDENT;

  enum STATE {
    BEGIN      = 0,
    PARSE,
    END,
    CLEAR,
    EXIT,
  };

  ~Parser();

  static bool reservedKeyword(const std::string& text);

  static Executable * parse(Context& ctx, void * reader_hdl, TOKEN_READER reader, bool trace = false);

  static Parser * createInteractiveParser(Context& ctx, void * reader_hdl, TOKEN_READER reader);

  static Parser * createInteractiveParser(Context& ctx, TOKEN_READER reader);

  Statement * parseStatement();
  Expression * parseExpression();

  STATE state() { return _state; }

  Context& context() { return _ctx; }

  TokenPtr front();
  TokenPtr pop();
  void push(TokenPtr& t);
  void push_back(TokenPtr& t);
  void clear();

  bool syntaxOnly() const { return _syntaxOnly; }
  bool popAny(TokenPtr& token);

private:
  explicit Parser(Context& ctx) : _ctx(ctx) { }

  Context& _ctx;
  STATE _state = BEGIN;
  TOKEN_SCANNER _scanner = nullptr;
  std::list<TokenPtr> _tokens;
  std::string _string_buffer;

  void state(STATE state) { _state = state; }
  bool init_scanner(void * reader_hdl, TOKEN_READER reader);
  void close_scanner();
  void trace(bool b) { _trace = b; }
  bool next_token(TokenPtr& token);

  bool _trace = false;      ///< forward token string to stderr
  bool _syntaxOnly = false; ///< flag for semantic analyze only
};

}

#endif /* PARSER_H_ */

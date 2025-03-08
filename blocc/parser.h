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
#include "expression.h"
#include "exception_parse.h"

#include <list>

namespace bloc
{

class Parser {

public:
  static const char NewLine       = '\n';
  static const char Separator     = ';';
  static const char Chain         = ',';
  static const char * Indent;

  enum State {
    Begin      = 0,
    Parsing,
    End,
    Cleared,
    Aborted,
  };

  ~Parser();

  static bool reservedKeyword(const std::string& text);

  /**
   * Make an executable from a source stream. On failure it throws exception
   * ParseError. Finally the returned pointer must be freed by the caller.
   * @param ctx         the context used to perform the parse
   * @param reader_hdl  the pointer to pass to the reader
   * @param reader      the function to read stream
   * @param trace       enable or disable tracing (default false)
   * @return            the new executable or throws
   */
  static Executable * parse(Context& ctx, void * reader_hdl, TOKEN_READER reader, bool trace = false);

  /**
   * Returns an interactive parser for a source stream. The returned pointer
   * must be freed by the caller.
   * @param ctx         the context used to perform the parse
   * @param reader_hdl  the pointer to pass to the reader
   * @param reader      the function to read stream
   * @return            the new parser
   */
  static Parser * createInteractiveParser(Context& ctx, void * reader_hdl, TOKEN_READER reader);

  /**
   * Returns an interactive parser for a source stream, initialized to pass itself
   * to the stream reader. The returned pointer must be freed by the caller.
   * @param ctx         the context used to perform the parse
   * @param reader      the function to read stream
   * @return            the new parser
   */
  static Parser * createInteractiveParser(Context& ctx, TOKEN_READER reader);

  /**
   * Parse the next statement read from stream. On failure it throws exception
   * ParseError. At unexpected end of stream it throws the ParseError with no
   * EXC_PARSE_EOF. Finally the returned pointer must be freed by the caller.
   * @return            the new statement or throws
   */
  Statement * parseStatement();

  /**
   * Parse the next expression read from stream. On failure it throws exception
   * ParseError. At unexpected end of stream it throws the ParseError with no
   * EXC_PARSE_EOF. Finally the returned pointer must be freed by the caller.
   * @return            the new expression or throws
   */
  Expression * parseExpression();

  State state() { return _state; }

  Context& context() { return _ctx; }

  TokenPtr front();
  TokenPtr pop();
  void push(const TokenPtr& t);
  void push_back(const TokenPtr& t);

  /**
   * Clear the parser state. It should be called after ParseError to purge
   * the current stream buffer until end of line.
   */
  void clear();

  bool popAny(TokenPtr& token);

  void trace(bool b) { _trace = b; }
  bool semantic() const { return _semantic; }

private:
  explicit Parser(Context& ctx) : _ctx(ctx) { }

  Context& _ctx;
  State _state = Begin;
  TOKEN_SCANNER _scanner = nullptr;
  std::list<TokenPtr> _tokens;
  std::string _string_buffer;

  void state(State state) { _state = state; }
  bool init_scanner(void * reader_hdl, TOKEN_READER reader);
  void close_scanner();
  bool next_token(TokenPtr& token);

  bool _trace = false;      ///< forward token string to stderr
  bool _semantic = true;    ///< enable/disable semantic analyze
  PARSING_POSITION _position = { 1, 0 }; ///< position in the source stream
};

}

#endif /* PARSER_H_ */

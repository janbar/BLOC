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
#include <unordered_set>

namespace bloc
{

class Parser {

public:
  /**
   * The interface of the input stream reader
   */
  class StreamReader
  {
  public:
    virtual int read(Parser * parser, char * buf, int max_size) = 0;
  };

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

  /**
   * Make an executable from a source stream. On failure it throws exception
   * ParseError. Finally the returned pointer must be freed by the caller.
   * @param ctx         the context used to perform the parse
   * @param reader      the function to read stream
   * @param trace       enable or disable tracing (default false)
   * @return            the new executable or throws
   */
  static Executable * parse(Context& ctx, StreamReader& reader, bool trace = false);

  /**
   * Returns an interactive parser for a source stream. The returned pointer
   * must be freed by the caller.
   * @param ctx         the context used to perform the parse
   * @param reader      the function to read stream
   * @return            the new parser
   */
  static Parser * createInteractiveParser(Context& ctx, StreamReader& reader);

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

  Context& context() { return _ctx; }

  /**
   * Retrieve the next token from queue
   */
  TokenPtr front();
  /**
   * Pop the next token from queue
   */
  TokenPtr pop();
  /**
   * Restore or push the token in front of queue
   */
  void push(const TokenPtr& t);

  /**
   * Clear the parser state. It should be called after ParseError to purge
   * the current stream buffer until end of line.
   */
  void clear();

  /**
   * Return true if the given string is a reserved word
   */
  bool reservedKeyword(const std::string& text);

  /**
   * Pop a token directly from the stream buffer including end of line marker.
   */
  bool popAny(TokenPtr& token);

  void trace(bool b) { _trace = b; }

  void state(State state) { _state = state; }

  State state() { return _state; }

  void nesting(int n) { _nesting = n; }

  int nesting() const { return _nesting; }

  void resetPosition() { _position = { 1, 1 }; }

private:
  /* TOKEN_READER */
  static void token_read(void * hdl, char * buf, int * len, int max_size);

  explicit Parser(Context& ctx, StreamReader& reader)
  : _ctx(ctx), _reader(reader) { }

  Context& _ctx;
  StreamReader& _reader;
  State _state = Begin;
  TOKEN_SCANNER _scanner = nullptr;
  std::list<TokenPtr> _tokens;
  std::string _string_buffer;

  bool init_scanner();
  void close_scanner();
  bool next_token(TokenPtr& token);

  bool _trace = false;      ///< forward token string to stderr
  int _nesting = 0;         ///< rank of parser nesting (include)
  typedef struct { int lno; int pno; } PARSING_POSITION;
  PARSING_POSITION _position = { 1, 1 }; ///< position in the source stream
  std::unordered_set<std::string> _cache_keywords;
};

}

#endif /* PARSER_H_ */

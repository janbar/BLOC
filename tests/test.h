#ifndef TEST_MAIN_H
#define TEST_MAIN_H

#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include <blocc/context.h>
#include <blocc/parser.h>

#include <string>

class IBuffer
{
  std::string _text;
  size_t _pos = 0;

public:
  virtual ~IBuffer() = default;
  IBuffer() { }
  IBuffer& reset(const std::string& text) { _text.assign(text); _pos = 0; return *this; }
  IBuffer& append(const std::string& text) { _text.append(text); return *this; }

  int read(char * buf, int max_size)
  {
    size_t r = (max_size  > _text.size() - _pos ? _text.size() - _pos : max_size);
    ::memcpy(buf, &_text[_pos], r);
    _pos += r;
    return (int) r;
  }

  static void read_input(void * handle, char * buf, int * len, int max_size)
  {
    IBuffer * ib = static_cast<IBuffer*>(handle);
    *len = ib->read(buf, max_size);
  }
};


class TestingContext : public bloc::Context
{
  IBuffer input;
  bloc::Parser * parser;

public:
  virtual ~TestingContext () { delete parser; }
  TestingContext () : Context ()
  {
    parser = bloc::Parser::createInteractiveParser (*this, &input, &IBuffer::read_input);
  }

  void reset (const std::string &text)
  {
    input.reset("\n").append(text).append (";");
  }

  bloc::Expression * parseExpression ()
  {
    parser->clear();
    return parser->parseExpression ();
  }

  bloc::Statement * parseStatement ()
  {
    parser->clear();
    return parser->parseStatement ();
  }

  bloc::Executable * parse ()
  {
    parser->clear();
    return bloc::Parser::parse (*this, &input, &IBuffer::read_input);
  }
};

#endif


#ifndef TEST_MAIN_H
#define TEST_MAIN_H

#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include <blocc/context.h>
#include <blocc/parser.h>
#include <blocc/string_reader.h>

class TestingContext : public bloc::Context
{
  bloc::StringReader input;
  bloc::Parser * parser;

public:
  virtual ~TestingContext () { delete parser; }
  TestingContext () : Context ()
  {
    parser = bloc::Parser::createInteractiveParser (*this, &input, &bloc::StringReader::token_read);
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
    return bloc::Parser::parse (*this, &input, &bloc::StringReader::token_read);
  }
};

#endif

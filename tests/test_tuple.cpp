#include <iostream>
#include <string>
#include <cstring>
#include <climits>

#include <test.h>
#include <hashvalue.c>
#include <blocc/exception_parse.h>

TestingContext ctx;

using namespace bloc;

TEST_CASE("tuple CTOR")
{
  Expression * e;
  ctx.reset("tup(\"abcd\", 1234, 0.123, raw(16, 0x20), true)");
  e = ctx.parseExpression();
  REQUIRE_NOTHROW( e->tuple(ctx) );
  delete e;
}

TEST_CASE("tuple item 0")
{
  Expression * e;
  ctx.reset("tup(\"abcd\", 1234, 0.123, raw(16, 0x20), true)@0");
  try { e = ctx.parseExpression(); delete e; }
  catch(ParseError& pe) { SUCCEED(pe.what()); return; }
  FAIL("no exception");
}

TEST_CASE("tuple item out")
{
  Expression * e;
  ctx.reset("tup(\"abcd\", 1234, 0.123, raw(16, 0x20), true)@6");
  try { e = ctx.parseExpression(); delete e; }
  catch(ParseError& pe) { SUCCEED(pe.what()); return; }
  FAIL("no exception");
}

TEST_CASE("tuple item@")
{
  Expression * e;
  ctx.reset("tup(\"abcd\", 1234, 0.123, raw(16, 0x20), true)@1");
  e = ctx.parseExpression();
  REQUIRE( (e->type(ctx) == Type::LITERAL && e->literal(ctx) == "abcd") );
  delete e;
  ctx.reset("tup(\"abcd\", 1234, 0.123, raw(16, 0x20), true)@2");
  e = ctx.parseExpression();
  REQUIRE( (e->type(ctx) == Type::INTEGER && e->integer(ctx) == 1234) );
  delete e;
  ctx.reset("tup(\"abcd\", 1234, 0.123, raw(16, 0x20), true)@3");
  e = ctx.parseExpression();
  REQUIRE( (e->type(ctx) == Type::NUMERIC && e->numeric(ctx) == 0.123) );
  delete e;
  ctx.reset("tup(\"abcd\", 1234, 0.123, raw(16, 0x20), true)@4");
  e = ctx.parseExpression();
  REQUIRE( (e->type(ctx) == Type::TABCHAR && e->tabchar(ctx).size() == 16) );
  delete e;
  ctx.reset("tup(\"abcd\", 1234, 0.123, raw(16, 0x20), true)@5");
  e = ctx.parseExpression();
  REQUIRE( (e->type(ctx) == Type::BOOLEAN && e->boolean(ctx) == true) );
  delete e;
}

TEST_CASE("tuple item.set@")
{
  Expression * e;
  ctx.reset("tup(\"abcd\", 1234, 0.123, raw(16, 0x20), true).set@1(\"xxxx\")");
  e = ctx.parseExpression();
  REQUIRE( e->tuple(ctx).at(0)->literal(ctx) == "xxxx" );
  delete e;
  ctx.reset("tup(\"abcd\", 1234, 0.123, raw(16, 0x20), true).set@2(5678)");
  e = ctx.parseExpression();
  REQUIRE( e->tuple(ctx).at(1)->integer(ctx) == 5678 );
  delete e;
  ctx.reset("tup(\"abcd\", 1234, 0.123, raw(16, 0x20), true).set@3(0.456)");
  e = ctx.parseExpression();
  REQUIRE( e->tuple(ctx).at(2)->numeric(ctx) == 0.456 );
  delete e;
  ctx.reset("tup(\"abcd\", 1234, 0.123, raw(16, 0x20), true).set@4(raw(4,0x41))");
  e = ctx.parseExpression();
  REQUIRE( e->tuple(ctx).at(3)->tabchar(ctx).size() == 4 );
  delete e;
  ctx.reset("tup(\"abcd\", 1234, 0.123, raw(16, 0x20), true).set@5(false)");
  e = ctx.parseExpression();
  REQUIRE( e->tuple(ctx).at(4)->boolean(ctx) == false );
  delete e;
}

TEST_CASE("tuple item.set@ bad type")
{
  Expression * e;
  ctx.reset("tup(\"abcd\", 1234, 0.123, raw(16, 0x20), true).set@1(0)");
  try { e = ctx.parseExpression(); delete e; }
  catch(ParseError& pe) { SUCCEED(pe.what()); return; }
  FAIL("no exception");

  ctx.reset("tup(\"abcd\", 1234, 0.123, raw(16, 0x20), true).set@2(true)");
  try { e = ctx.parseExpression(); delete e; }
  catch(ParseError& pe) { SUCCEED(pe.what()); return; }
  FAIL("no exception");

  ctx.reset("tup(\"abcd\", 1234, 0.123, raw(16, 0x20), true).set@3(raw(4,0x40))");
  try { e = ctx.parseExpression(); delete e; }
  catch(ParseError& pe) { SUCCEED(pe.what()); return; }
  FAIL("no exception");

  ctx.reset("tup(\"abcd\", 1234, 0.123, raw(16, 0x20), true).set@4(0.456)");
  try { e = ctx.parseExpression(); delete e; }
  catch(ParseError& pe) { SUCCEED(pe.what()); return; }
  FAIL("no exception");

  ctx.reset("tup(\"abcd\", 1234, 0.123, raw(16, 0x20), true).set@5(1234)");
  try { e = ctx.parseExpression(); delete e; }
  catch(ParseError& pe) { SUCCEED(pe.what()); return; }
  FAIL("no exception");
}

TEST_CASE("tuple item.set@ type mixing")
{
  Expression * e;
  ctx.reset("tup(\"abcd\", 1234, 0.123, raw(16, 0x20), true).set@2(4.56)");
  e = ctx.parseExpression();
  REQUIRE( e->tuple(ctx).at(1)->integer(ctx) == 4 );
  delete e;
  ctx.reset("tup(\"abcd\", 1234, 0.123, raw(16, 0x20), true).set@3(5678)");
  e = ctx.parseExpression();
  REQUIRE( e->tuple(ctx).at(2)->numeric(ctx) == 5678.0 );
  delete e;
}

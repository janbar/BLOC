#include <iostream>
#include <string>
#include <cstring>
#include <climits>

#include <test.h>
#include <hashvalue.c>
#include <blocc/exception_parse.h>

TestingContext ctx;

using namespace bloc;

TEST_CASE("tab of tuple CTOR")
{
  Expression * e;
  ctx.reset("tab(10, tuple(\"abcd\", 1234, 0.123, raw(16, 0x20), true))");
  e = ctx.parseExpression();
  REQUIRE_NOTHROW( e->collection(ctx) );
  REQUIRE( (e->type(ctx) == Type::ROWTYPE && e->type(ctx).level() == 1) );
  delete e;
}

TEST_CASE("tab at row@")
{
  Expression * e;
  ctx.reset("tab(10, tuple(\"abcd\", 1234, 0.123, raw(16, 0x20), true)).at(9)@3");
  e = ctx.parseExpression();
  REQUIRE( (e->type(ctx) == Type::NUMERIC && e->numeric(ctx) == 0.123) );
}

TEST_CASE("tab at row.set@")
{
  Expression * e;
  ctx.reset("tab(10, tuple(\"abcd\", 1234, 0.123, raw(16, 0x20), true)).at(9).set@1(\"xxxx\")");
  e = ctx.parseExpression();
  REQUIRE( e->tuple(ctx).at(0)->literal(ctx) == "xxxx" );
}

TEST_CASE("tab put")
{
  Expression * e;
  ctx.reset("tab(10, tuple(\"abcd\", 1234, 0.123, raw(16, 0x20), true)).put(2, tuple(\"xxxx\", 4567, 3.1415, raw(4, 0x40), true))");
  e = ctx.parseExpression();
  Collection& c = e->collection(ctx); /* value from temporary pool */
  REQUIRE( (c.at(2)->refTuple().at(0)->refLiteral() == "xxxx") );
}

TEST_CASE("tab put bad type")
{
  Expression * e;
  ctx.reset("tab(10, tuple(\"abcd\", 1234, 0.123, raw(16, 0x20), true)).put(2, tuple(1234, 0.123, raw(16, 0x20), true,\"abcd\"))");
  try { e = ctx.parseExpression(); delete e; }
  catch(ParseError& pe) { SUCCEED(pe.what()); return; }
  FAIL("no exception");
}

TEST_CASE("tab insert row")
{
  Expression * e;
  ctx.reset("tab(10, tuple(\"abcd\", 1234, 0.123, raw(16, 0x20), true)).insert(2, tuple(\"xxxx\", 4567, 3.1415, raw(4, 0x40), true))");
  e = ctx.parseExpression();
  Collection& c = e->collection(ctx); /* value from temporary pool */
  REQUIRE( (c.size() == 11 && c.at(2)->refTuple().at(0)->refLiteral() == "xxxx") );
}

TEST_CASE("tab insert bad type")
{
  Expression * e;
  ctx.reset("tab(10, tuple(\"abcd\", 1234, 0.123, raw(16, 0x20), true)).insert(2, tuple(1234, 0.123, raw(16, 0x20), true,\"abcd\"))");
  try { e = ctx.parseExpression(); delete e; }
  catch(ParseError& pe) { SUCCEED(pe.what()); return; }
  FAIL("no exception");
}

TEST_CASE("tab insert tab")
{
  Expression * e;
  ctx.reset("tab(10, tuple(\"abcd\", 1234, 0.123, raw(16, 0x20), true)).insert(2, tab(5, tuple(\"xxxx\", 1234, 0.123, raw(16, 0x20), true)))");
  e = ctx.parseExpression();
  Collection& c = e->collection(ctx); /* value from temporary pool */
  REQUIRE( (c.size() == 15 && c.at(2)->refTuple().at(0)->refLiteral() == "xxxx") );
}

TEST_CASE("tab insert bad tab")
{
  Expression * e;
  ctx.reset("tab(10, tuple(\"abcd\", 1234, 0.123, raw(16, 0x20), true)).insert(2, tab(5, tuple(1234, \"xxxx\", 0.123, raw(16, 0x20), true)))");
  try { e = ctx.parseExpression(); delete e; }
  catch(ParseError& pe) { SUCCEED(pe.what()); return; }
  FAIL("no exception");
}

TEST_CASE("tab concat row")
{
  Expression * e;
  ctx.reset("tab(10, tuple(\"abcd\", 1234, 0.123, raw(16, 0x20), true)).concat(tuple(\"xxxx\", 4567, 3.1415, raw(4, 0x40), true))");
  e = ctx.parseExpression();
  Collection& c = e->collection(ctx); /* value from temporary pool */
  REQUIRE( (c.size() == 11 && c.at(10)->refTuple().at(0)->refLiteral() == "xxxx") );
}

TEST_CASE("tab concat bad type")
{
  Expression * e;
  ctx.reset("tab(10, tuple(\"abcd\", 1234, 0.123, raw(16, 0x20), true)).concat(tuple(1234, 0.123, raw(16, 0x20), true,\"abcd\"))");
  try { e = ctx.parseExpression(); delete e; }
  catch(ParseError& pe) { SUCCEED(pe.what()); return; }
  FAIL("no exception");
}

TEST_CASE("tab delete row")
{
  Expression * e;
  ctx.reset("tab(10, tuple(\"abcd\", 1234, 0.123, raw(16, 0x20), true)).delete(2)");
  e = ctx.parseExpression();
  Collection& c = e->collection(ctx); /* value from temporary pool */
  REQUIRE( c.size() == 9 );
}

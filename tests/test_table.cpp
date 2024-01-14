#include <iostream>
#include <string>
#include <cstring>
#include <climits>

#include <test.h>
#include <hashvalue.c>
#include <blocc/exception_parse.h>
#include <blocc/collection.h>
#include <blocc/tuple.h>

TestingContext ctx;

using namespace bloc;

TEST_CASE("tab of tuple CTOR")
{
  Expression * e;
  ctx.reset("tab(10, tup(\"abcd\", 1234, 0.123, raw(16, 0x20), true))");
  e = ctx.parseExpression();
  REQUIRE_NOTHROW( e->value(ctx) );
  Value& v = e->value(ctx);
  REQUIRE( (v.type() == Type::ROWTYPE && v.type().level() == 1) );
  delete e;
}

TEST_CASE("tab at row@")
{
  Expression * e;
  ctx.reset("tab(10, tup(\"abcd\", 1234, 0.123, raw(16, 0x20), true)).at(9)@3");
  e = ctx.parseExpression();
  Value& v = e->value(ctx); /* value from temporary pool */
  REQUIRE( (v.type() == Type::NUMERIC && *(v.numeric()) == 0.123) );
  delete e;
}

TEST_CASE("tab at row.set@")
{
  Expression * e;
  ctx.reset("tab(10, tup(\"abcd\", 1234, 0.123, raw(16, 0x20), true)).at(9).set@1(\"xxxx\")");
  e = ctx.parseExpression();
  REQUIRE( *(e->value(ctx).tuple()->at(0).literal()) == "xxxx" );
  delete e;
}

TEST_CASE("tab put")
{
  Expression * e;
  ctx.reset("tab(10, tup(\"abcd\", 1234, 0.123, raw(16, 0x20), true)).put(2, tup(\"xxxx\", 4567, 3.1415, raw(4, 0x40), true))");
  e = ctx.parseExpression();
  Value& v = e->value(ctx); /* value from temporary pool */
  Collection *  c = v.collection();
  REQUIRE( (*(c->at(2).tuple()->at(0).literal()) == "xxxx") );
  delete e;
}

TEST_CASE("tab put bad type")
{
  Expression * e;
  ctx.reset("tab(10, tup(\"abcd\", 1234, 0.123, raw(16, 0x20), true)).put(2, tup(1234, 0.123, raw(16, 0x20), true,\"abcd\"))");
  try { e = ctx.parseExpression(); delete e; FAIL("No throw"); }
  catch(ParseError& pe) { SUCCEED(pe.what()); }
}

TEST_CASE("tab insert row")
{
  Expression * e;
  ctx.reset("tab(10, tup(\"abcd\", 1234, 0.123, raw(16, 0x20), true)).insert(2, tup(\"xxxx\", 4567, 3.1415, raw(4, 0x40), true))");
  e = ctx.parseExpression();
  Value& v = e->value(ctx); /* value from temporary pool */
  Collection * c = v.collection();
  REQUIRE( (c->size() == 11 && *(c->at(2).tuple()->at(0).literal()) == "xxxx") );
  delete e;
}

TEST_CASE("tab insert bad type")
{
  Expression * e;
  ctx.reset("tab(10, tup(\"abcd\", 1234, 0.123, raw(16, 0x20), true)).insert(2, tup(1234, 0.123, raw(16, 0x20), true,\"abcd\"))");
  try { e = ctx.parseExpression(); delete e; FAIL("No throw"); }
  catch(ParseError& pe) { SUCCEED(pe.what()); }
}

TEST_CASE("tab insert tab")
{
  Expression * e;
  ctx.reset("tab(10, tup(\"abcd\", 1234, 0.123, raw(16, 0x20), true)).insert(2, tab(5, tup(\"xxxx\", 1234, 0.123, raw(16, 0x20), true)))");
  e = ctx.parseExpression();
  Value& v = e->value(ctx); /* value from temporary pool */
  Collection * c = v.collection();
  REQUIRE( (c->size() == 15 && *(c->at(2).tuple()->at(0).literal()) == "xxxx") );
  delete e;
}

TEST_CASE("tab insert bad tab")
{
  Expression * e;
  ctx.reset("tab(10, tup(\"abcd\", 1234, 0.123, raw(16, 0x20), true)).insert(2, tab(5, tup(1234, \"xxxx\", 0.123, raw(16, 0x20), true)))");
  try { e = ctx.parseExpression(); delete e; FAIL("No throw"); }
  catch(ParseError& pe) { SUCCEED(pe.what()); }
}

TEST_CASE("tab concat row")
{
  Expression * e;
  ctx.reset("tab(10, tup(\"abcd\", 1234, 0.123, raw(16, 0x20), true)).concat(tup(\"xxxx\", 4567, 3.1415, raw(4, 0x40), true))");
  e = ctx.parseExpression();
  Value& v = e->value(ctx); /* value from temporary pool */
  Collection * c = v.collection();
  REQUIRE( (c->size() == 11 && *(c->at(10).tuple()->at(0).literal()) == "xxxx") );
  delete e;
}

TEST_CASE("tab concat bad type")
{
  Expression * e;
  ctx.reset("tab(10, tup(\"abcd\", 1234, 0.123, raw(16, 0x20), true)).concat(tup(1234, 0.123, raw(16, 0x20), true,\"abcd\"))");
  try { e = ctx.parseExpression(); delete e; FAIL("No throw"); }
  catch(ParseError& pe) { SUCCEED(pe.what()); }
}

TEST_CASE("tab delete row")
{
  Expression * e;
  ctx.reset("tab(10, tup(\"abcd\", 1234, 0.123, raw(16, 0x20), true)).delete(2)");
  e = ctx.parseExpression();
  Value& v = e->value(ctx);
  REQUIRE( v.collection()->size() == 9 );
  delete e;
}

TEST_CASE("table type mixing")
{
  Expression * e;
  ctx.reset("tab(1, 1234).put(0, 4.56)");
  e = ctx.parseExpression();
  REQUIRE( *(e->value(ctx).collection()->at(0).integer()) == 4 );
  delete e;
  ctx.reset("tab(1, 0.123).put(0, 5678)");
  e = ctx.parseExpression();
  REQUIRE( *(e->value(ctx).collection()->at(0).numeric()) == 5678.0 );
  delete e;

  ctx.reset("tab(1, \"abcd\").put(0, null)");
  e = ctx.parseExpression();
  REQUIRE( e->value(ctx).collection()->at(0).literal() == nullptr );
  delete e;
  ctx.reset("tab(1, 1234).insert(1, null)");
  e = ctx.parseExpression();
  REQUIRE( e->value(ctx).collection()->at(1).integer() == nullptr );
  delete e;
  ctx.reset("tab(1, 4.56).insert(1, null)");
  e = ctx.parseExpression();
  REQUIRE( e->value(ctx).collection()->at(1).numeric() == nullptr );
  delete e;
  ctx.reset("tab(1, 1234).concat(null)");
  e = ctx.parseExpression();
  REQUIRE( e->value(ctx).collection()->at(1).integer() == nullptr );
  delete e;
  ctx.reset("tab(1, 4.56).concat(null)");
  e = ctx.parseExpression();
  REQUIRE( e->value(ctx).collection()->at(1).numeric() == nullptr );
  delete e;
}

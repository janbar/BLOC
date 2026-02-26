#include <iostream>
#include <string>
#include <cstring>
#include <climits>
#include <cmath>

#include <test.h>
#include <hashvalue.c>
#include <blocc/exception_parse.h>

TestingContext ctx;

using namespace bloc;

bool fequal(double a, double b)
{
  return std::fabs(a - b) < 1e-6;
}

TEST_CASE("integer")
{
  Expression * e;
  ctx.reset("1234567890");
  e = ctx.parseExpression();
  REQUIRE( *(e->value(ctx).integer()) == 1234567890);
  REQUIRE( e->unparse(ctx) == "1234567890" );
  delete e;

  ctx.reset("0000000000000000000000001");
  e = ctx.parseExpression();
  REQUIRE( *(e->value(ctx).integer()) == 1 );
  REQUIRE( e->unparse(ctx) == "1" );
  delete e;

  ctx.reset("0x0123456789abcdef");
  e = ctx.parseExpression();
  REQUIRE( *(e->value(ctx).integer()) == 0x0123456789abcdefLL );
  REQUIRE( e->unparse(ctx) == "81985529216486895" );
  delete e;

  ctx.reset("0x7fffffffffffffff");
  e = ctx.parseExpression();
  REQUIRE( *(e->value(ctx).integer()) == 0x7fffffffffffffffLL );
  REQUIRE( e->unparse(ctx) == "9223372036854775807" );
  delete e;

  ctx.reset("0xffffffffffffffffff");
  try { e = ctx.parseExpression(); delete e; FAIL("No throw"); }
  catch (ParseError& pe) { SUCCEED(pe.what()); }
}

TEST_CASE("decimal")
{
  Expression * e;
  ctx.reset("round( 1234567890.001 , 3 )");
  e = ctx.parseExpression();
  REQUIRE( fequal(*(e->value(ctx).numeric()), 1234567890.001) );
  REQUIRE( e->unparse(ctx) == "round(1234567890.001, 3)" );
  delete e;

  ctx.reset("round( 1.234567890001e+9 , 3 )");
  e = ctx.parseExpression();
  REQUIRE( fequal(*(e->value(ctx).numeric()), 1234567890.001) );
  REQUIRE( e->unparse(ctx) == "round(1234567890.001, 3)" );
  delete e;

  ctx.reset("round( 1234567890.001e-9 , 12)");
  e = ctx.parseExpression();
  REQUIRE( fequal(*(e->value(ctx).numeric()), 1.234567890001) );
  REQUIRE( e->unparse(ctx) == "round(1.234567890001, 12)" );
  delete e;

  ctx.reset("1.23456789e+308");
  e = ctx.parseExpression();
  REQUIRE( fequal(*(e->value(ctx).numeric()), 1.23456789e+308) );
  REQUIRE( e->unparse(ctx) == "1.23456789e+308" );
  delete e;

  ctx.reset("1.23456789e+309");
  try { e = ctx.parseExpression(); delete e; FAIL("No throw"); }
  catch (ParseError& pe) { SUCCEED(pe.what()); }
}

TEST_CASE("string")
{
  Expression * e;
  ctx.reset("\"/\\\\@$^~|_.+-=#012345679\"\"ABCDEFGHIJKLMNOPQRSTUVWXYZ\\\"\\a\\b\\f\\n\\r\\t\"");
  e = ctx.parseExpression();
  REQUIRE( *(e->value(ctx).literal()) ==
          "/\\@$^~|_.+-=#012345679\"ABCDEFGHIJKLMNOPQRSTUVWXYZ\"\a\b\f\n\r\t" );
  REQUIRE( e->unparse(ctx) ==
          "\"/\\\\@$^~|_.+-=#012345679\\\"ABCDEFGHIJKLMNOPQRSTUVWXYZ\\\"\\a\\b\\f\\n\\r\\t\"" );
  delete e;
}

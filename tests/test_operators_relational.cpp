#include <iostream>
#include <string>
#include <cstring>
#include <climits>

#include <test.h>
#include <hashvalue.c>

TestingContext ctx;

using namespace bloc;

TEST_CASE("operator EQ boolean")
{
  Expression * e;
  ctx.reset("true == true");
  e = ctx.parseExpression();
  REQUIRE( e->boolean(ctx) == true );
  delete e;
  ctx.reset("true == false");
  e = ctx.parseExpression();
  REQUIRE( e->boolean(ctx) == false );
  delete e;
  ctx.reset("false == false");
  e = ctx.parseExpression();
  REQUIRE( e->boolean(ctx) == true );
  delete e;
}

TEST_CASE("operator NEQ boolean")
{
  Expression * e;
  ctx.reset("true != true");
  e = ctx.parseExpression();
  REQUIRE( e->boolean(ctx) == false );
  delete e;
  ctx.reset("true != false");
  e = ctx.parseExpression();
  REQUIRE( e->boolean(ctx) == true );
  delete e;
  ctx.reset("false != false ");
  e = ctx.parseExpression();
  REQUIRE( e->boolean(ctx) == false );
  delete e;
}

TEST_CASE("operator LESS boolean")
{
  Expression * e;
  ctx.reset("true < true");
  e = ctx.parseExpression();
  REQUIRE( e->boolean(ctx) == false );
  delete e;
  ctx.reset("true < false");
  e = ctx.parseExpression();
  REQUIRE( e->boolean(ctx) == false );
  delete e;
  ctx.reset("false < false ");
  e = ctx.parseExpression();
  REQUIRE( e->boolean(ctx) == false );
  delete e;
}

TEST_CASE("operator GREATER boolean")
{
  Expression * e;
  ctx.reset("true > true");
  e = ctx.parseExpression();
  REQUIRE( e->boolean(ctx) == false );
  delete e;
  ctx.reset("true > false");
  e = ctx.parseExpression();
  REQUIRE( e->boolean(ctx) == false );
  delete e;
  ctx.reset("false > false ");
  e = ctx.parseExpression();
  REQUIRE( e->boolean(ctx) == false );
  delete e;
}

TEST_CASE("operator LEQ boolean")
{
  Expression * e;
  ctx.reset("true <= true");
  e = ctx.parseExpression();
  REQUIRE( e->boolean(ctx) == false );
  delete e;
  ctx.reset("true <= false");
  e = ctx.parseExpression();
  REQUIRE( e->boolean(ctx) == false );
  delete e;
  ctx.reset("false <= false ");
  e = ctx.parseExpression();
  REQUIRE( e->boolean(ctx) == false );
  delete e;
}

TEST_CASE("operator GEQ boolean")
{
  Expression * e;
  ctx.reset("true >= true");
  e = ctx.parseExpression();
  REQUIRE( e->boolean(ctx) == false );
  delete e;
  ctx.reset("true >= false");
  e = ctx.parseExpression();
  REQUIRE( e->boolean(ctx) == false );
  delete e;
  ctx.reset("false >= false ");
  e = ctx.parseExpression();
  REQUIRE( e->boolean(ctx) == false );
  delete e;
}

TEST_CASE("operator EQ numeric")
{
  Expression * e;
  ctx.reset("1 == 1");
  e = ctx.parseExpression();
  REQUIRE( e->boolean(ctx) == true );
  delete e;
  ctx.reset("1.0 == 1.0");
  e = ctx.parseExpression();
  REQUIRE( e->boolean(ctx) == true );
  delete e;
  ctx.reset("1 == 1.0");
  e = ctx.parseExpression();
  REQUIRE( e->boolean(ctx) == true );
  delete e;
  ctx.reset("0 == 1");
  e = ctx.parseExpression();
  REQUIRE( e->boolean(ctx) == false );
  delete e;
  ctx.reset("0.9 == 1");
  e = ctx.parseExpression();
  REQUIRE( e->boolean(ctx) == false );
  delete e;
}

TEST_CASE("operator NEQ numeric")
{
  Expression * e;
  ctx.reset("1 != 1");
  e = ctx.parseExpression();
  REQUIRE( e->boolean(ctx) == false );
  delete e;
  ctx.reset("1.0 != 1.0");
  e = ctx.parseExpression();
  REQUIRE( e->boolean(ctx) == false );
  delete e;
  ctx.reset("1 != 1.0");
  e = ctx.parseExpression();
  REQUIRE( e->boolean(ctx) == false );
  delete e;
  ctx.reset("0 != 1");
  e = ctx.parseExpression();
  REQUIRE( e->boolean(ctx) == true );
  delete e;
  ctx.reset("0.9 != 1");
  e = ctx.parseExpression();
  REQUIRE( e->boolean(ctx) == true );
  delete e;
}

TEST_CASE("operator LESS numeric")
{
  Expression * e;
  ctx.reset("1 < 1");
  e = ctx.parseExpression();
  REQUIRE( e->boolean(ctx) == false );
  delete e;
  ctx.reset("0 < 1");
  e = ctx.parseExpression();
  REQUIRE( e->boolean(ctx) == true );
  delete e;
  ctx.reset("1 < 1.0");
  e = ctx.parseExpression();
  REQUIRE( e->boolean(ctx) == false );
  delete e;
  ctx.reset("0.9 < 1");
  e = ctx.parseExpression();
  REQUIRE( e->boolean(ctx) == true );
  delete e;
}

TEST_CASE("operator GREATER numeric")
{
  Expression * e;
  ctx.reset("1 > 1");
  e = ctx.parseExpression();
  REQUIRE( e->boolean(ctx) == false );
  delete e;
  ctx.reset("0 > 1");
  e = ctx.parseExpression();
  REQUIRE( e->boolean(ctx) == false );
  delete e;
  ctx.reset("0 > 0.9");
  e = ctx.parseExpression();
  REQUIRE( e->boolean(ctx) == false );
  delete e;
  ctx.reset("1 > 0.9");
  e = ctx.parseExpression();
  REQUIRE( e->boolean(ctx) == true );
  delete e;
}

TEST_CASE("operator LEQ numeric")
{
  Expression * e;
  ctx.reset("1 <= 1");
  e = ctx.parseExpression();
  REQUIRE( e->boolean(ctx) == true );
  delete e;
  ctx.reset("1 <= 0");
  e = ctx.parseExpression();
  REQUIRE( e->boolean(ctx) == false );
  delete e;
  ctx.reset("1 <= 1.0");
  e = ctx.parseExpression();
  REQUIRE( e->boolean(ctx) == true );
  delete e;
  ctx.reset("1 <= 0.9");
  e = ctx.parseExpression();
  REQUIRE( e->boolean(ctx) == false );
  delete e;
}

TEST_CASE("operator GEQ numeric")
{
  Expression * e;
  ctx.reset("1 >= 1");
  e = ctx.parseExpression();
  REQUIRE( e->boolean(ctx) == true );
  delete e;
  ctx.reset("0 >= 1");
  e = ctx.parseExpression();
  REQUIRE( e->boolean(ctx) == false );
  delete e;
  ctx.reset("1 >= 1.0");
  e = ctx.parseExpression();
  REQUIRE( e->boolean(ctx) == true );
  delete e;
  ctx.reset("0.9 >= 1");
  e = ctx.parseExpression();
  REQUIRE( e->boolean(ctx) == false );
  delete e;
}

TEST_CASE("operator COMPARE string")
{
  Expression * e;
  ctx.reset("\"abcd\" == \"abcd\"");
  e = ctx.parseExpression();
  REQUIRE( e->boolean(ctx) == true );
  delete e;
  ctx.reset("\"abcd\" == \"abcde\"");
  e = ctx.parseExpression();
  REQUIRE( e->boolean(ctx) == false );
  delete e;
  ctx.reset("\"abcd\" != \"abcd\"");
  e = ctx.parseExpression();
  REQUIRE( e->boolean(ctx) == false );
  delete e;
  ctx.reset("\"abcd\" != \"abcde\"");
  e = ctx.parseExpression();
  REQUIRE( e->boolean(ctx) == true );
  delete e;
  ctx.reset("\"abcd\" > \"abcd\"");
  e = ctx.parseExpression();
  REQUIRE( e->boolean(ctx) == false );
  delete e;
  ctx.reset("\"bcde\" > \"abcd\"");
  e = ctx.parseExpression();
  REQUIRE( e->boolean(ctx) == true );
  delete e;
  ctx.reset("\"abcd\" >= \"abcd\"");
  e = ctx.parseExpression();
  REQUIRE( e->boolean(ctx) == true );
  delete e;
  ctx.reset("\"abcd\" >= \"bcde\"");
  e = ctx.parseExpression();
  REQUIRE( e->boolean(ctx) == false );
  delete e;
  ctx.reset("\"abcd\" < \"abcd\"");
  e = ctx.parseExpression();
  REQUIRE( e->boolean(ctx) == false );
  delete e;
  ctx.reset("\"abcd\" < \"bcde\"");
  e = ctx.parseExpression();
  REQUIRE( e->boolean(ctx) == true );
  delete e;
  ctx.reset("\"abcd\" <= \"abcd\"");
  e = ctx.parseExpression();
  REQUIRE( e->boolean(ctx) == true );
  delete e;
  ctx.reset("\"bcde\" <= \"abcd\"");
  e = ctx.parseExpression();
  REQUIRE( e->boolean(ctx) == false );
  delete e;
}

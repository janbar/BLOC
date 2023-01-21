#include <iostream>
#include <string>
#include <cstring>
#include <climits>

#include <test.h>
#include <hashvalue.c>

TestingContext ctx;

using namespace bloc;

TEST_CASE("operator I + I")
{
  ctx.reset("7687 + 43453");
  Expression * e = ctx.parseExpression();
  REQUIRE( e->integer(ctx) == 51140 );
  delete e;
}

TEST_CASE("operator I - I")
{
  ctx.reset("7687 - 43453");
  Expression * e = ctx.parseExpression();
  REQUIRE( e->integer(ctx) == -35766 );
  delete e;
}

TEST_CASE("operator I * I")
{
  ctx.reset("7687 * 43453");
  Expression * e = ctx.parseExpression();
  REQUIRE( e->integer(ctx) == 334023211 );
  delete e;
}

TEST_CASE("operator I / I")
{
  ctx.reset("43453 / 7687");
  Expression * e = ctx.parseExpression();
  REQUIRE( e->integer(ctx) == 5 );
  delete e;
}

TEST_CASE("operator I % I")
{
  ctx.reset("43453 % 7687");
  Expression * e = ctx.parseExpression();
  REQUIRE( e->integer(ctx) == 5018 );
  delete e;
}

TEST_CASE("operator I ** I (POWER)")
{
  Expression * e;
  ctx.reset("7687 ** 3");
  e = ctx.parseExpression();
  REQUIRE( e->integer(ctx) == 454224591703LL );
  delete e;
  ctx.reset("7687 power 3");
  e = ctx.parseExpression();
  REQUIRE( e->integer(ctx) == 454224591703LL );
  delete e;
}

TEST_CASE("operator I & I")
{
  ctx.reset("43453 & 7687");
  Expression * e = ctx.parseExpression();
  REQUIRE( e->integer(ctx) == 2053 );
  delete e;
}

TEST_CASE("operator I | I")
{
  ctx.reset("43453 | 7687");
  Expression * e = ctx.parseExpression();
  REQUIRE( e->integer(ctx) == 49087 );
  delete e;
}

TEST_CASE("operator I ^ I (XOR)")
{
  ctx.reset("43453 ^ 7687");
  Expression * e = ctx.parseExpression();
  REQUIRE( e->integer(ctx) == 47034 );
  delete e;
}

TEST_CASE("operator ~ I (bitwise NOT)")
{
  Expression * e;
  ctx.reset("~0");
  e = ctx.parseExpression();
  REQUIRE( e->integer(ctx) == INT64_MAX );
  delete e;
  ctx.reset("~(~0)");
  e = ctx.parseExpression();
  REQUIRE( e->integer(ctx) == 0 );
  delete e;
  ctx.reset("~(-1)");
  e = ctx.parseExpression();
  REQUIRE( e->integer(ctx) == INT64_MIN );
  delete e;
  ctx.reset("~(~(-1))");
  e = ctx.parseExpression();
  REQUIRE( e->integer(ctx) == -1L );
  delete e;
}

TEST_CASE("operator I << I")
{
  ctx.reset("43453 << 3");
  Expression * e = ctx.parseExpression();
  REQUIRE( e->integer(ctx) == 347624 );
  delete e;
}

TEST_CASE("operator I >> I")
{
  ctx.reset("347624 >> 3");
  Expression * e = ctx.parseExpression();
  REQUIRE( e->integer(ctx) == 43453 );
  delete e;
}

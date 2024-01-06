#include <iostream>
#include <string>
#include <cstring>
#include <climits>

#include <test.h>
#include <hashvalue.c>

TestingContext ctx;

using namespace bloc;

TEST_CASE("operator TRUE")
{
  ctx.reset("true");
  Expression * e = ctx.parseExpression();
  REQUIRE( *(e->value(ctx).boolean()) == true );
  delete e;
}

TEST_CASE("operator FALSE")
{
  ctx.reset("false");
  Expression * e = ctx.parseExpression();
  REQUIRE( *(e->value(ctx).boolean()) == false );
  delete e;
}

TEST_CASE("operator B && B")
{
  Expression * e;
  ctx.reset("true && true");
  e = ctx.parseExpression();
  REQUIRE( *(e->value(ctx).boolean()) == true );
  delete e;
  ctx.reset("true && false");
  e = ctx.parseExpression();
  REQUIRE( *(e->value(ctx).boolean()) == false );
  delete e;
  ctx.reset("false && false ");
  e = ctx.parseExpression();
  REQUIRE( *(e->value(ctx).boolean()) == false );
  delete e;
}

TEST_CASE("operator B || B")
{
  Expression * e;
  ctx.reset("true || true");
  e = ctx.parseExpression();
  REQUIRE( *(e->value(ctx).boolean()) == true );
  delete e;
  ctx.reset("true || false");
  e = ctx.parseExpression();
  REQUIRE( *(e->value(ctx).boolean()) == true );
  delete e;
  ctx.reset("false || false");
  e = ctx.parseExpression();
  REQUIRE( *(e->value(ctx).boolean()) == false );
  delete e;
}

TEST_CASE("operator B XOR B")
{
  Expression * e;
  ctx.reset("true xor true");
  e = ctx.parseExpression();
  REQUIRE( *(e->value(ctx).boolean()) == false );
  delete e;
  ctx.reset("true xor false");
  e = ctx.parseExpression();
  REQUIRE( *(e->value(ctx).boolean()) == true );
  delete e;
  ctx.reset("false xor false");
  e = ctx.parseExpression();
  REQUIRE( *(e->value(ctx).boolean()) == false );
  delete e;
}

TEST_CASE("operator !B (NOT)")
{
  Expression * e;
  ctx.reset("!true");
  e = ctx.parseExpression();
  REQUIRE( *(e->value(ctx).boolean()) == false);
  delete e;
  ctx.reset("not true");
  e = ctx.parseExpression();
  REQUIRE( *(e->value(ctx).boolean()) == false);
  delete e;
  ctx.reset("!false");
  e = ctx.parseExpression();
  REQUIRE( *(e->value(ctx).boolean()) == true);
  delete e;
  ctx.reset("not false");
  e = ctx.parseExpression();
  REQUIRE( *(e->value(ctx).boolean()) == true);
  delete e;
}

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
  ctx.reset("false && false");
  e = ctx.parseExpression();
  REQUIRE( *(e->value(ctx).boolean()) == false );
  delete e;

  ctx.reset("false && null");
  e = ctx.parseExpression();
  REQUIRE( *(e->value(ctx).boolean()) == false );
  delete e;
  ctx.reset("null && false");
  e = ctx.parseExpression();
  REQUIRE( *(e->value(ctx).boolean()) == false );
  delete e;

  ctx.reset("null && null");
  e = ctx.parseExpression();
  REQUIRE( e->value(ctx).isNull() );
  delete e;
  ctx.reset("null && true");
  e = ctx.parseExpression();
  REQUIRE( e->value(ctx).isNull() );
  delete e;
  ctx.reset("true && null");
  e = ctx.parseExpression();
  REQUIRE( e->value(ctx).isNull() );
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

  ctx.reset("true || null");
  e = ctx.parseExpression();
  REQUIRE( *(e->value(ctx).boolean()) == true );
  delete e;
  ctx.reset("null || true");
  e = ctx.parseExpression();
  REQUIRE( *(e->value(ctx).boolean()) == true );
  delete e;

  ctx.reset("null || null");
  e = ctx.parseExpression();
  REQUIRE( e->value(ctx).isNull() );
  delete e;
  ctx.reset("null || false");
  e = ctx.parseExpression();
  REQUIRE( e->value(ctx).isNull() );
  delete e;
  ctx.reset("false || null");
  e = ctx.parseExpression();
  REQUIRE( e->value(ctx).isNull() );
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

  ctx.reset("null xor null");
  e = ctx.parseExpression();
  REQUIRE( e->value(ctx).isNull() );
  delete e;
  ctx.reset("null xor false");
  e = ctx.parseExpression();
  REQUIRE( e->value(ctx).isNull() );
  delete e;
  ctx.reset("false xor null");
  e = ctx.parseExpression();
  REQUIRE( e->value(ctx).isNull() );
  delete e;
  ctx.reset("null xor true");
  e = ctx.parseExpression();
  REQUIRE( e->value(ctx).isNull() );
  delete e;
  ctx.reset("true xor null");
  e = ctx.parseExpression();
  REQUIRE( e->value(ctx).isNull() );
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

  ctx.reset("not null");
  e = ctx.parseExpression();
  REQUIRE( e->value(ctx).isNull() );
  delete e;
  ctx.reset("!null");
  e = ctx.parseExpression();
  REQUIRE( e->value(ctx).isNull() );
  delete e;
  ctx.reset("not(!null)");
  e = ctx.parseExpression();
  REQUIRE( e->value(ctx).isNull() );
  delete e;
}

TEST_CASE("break on first operand")
{
  Expression * e;
  ctx.reset("false && (1/0 == 1)");
  e = ctx.parseExpression();
  REQUIRE( *(e->value(ctx).boolean()) == false );
  delete e;
  ctx.reset("null && (1/0 == 1)");
  e = ctx.parseExpression();
  REQUIRE_THROWS( e->value(ctx) );
  delete e;
  ctx.reset("true && (1/0 == 1)");
  e = ctx.parseExpression();
  REQUIRE_THROWS( e->value(ctx) );
  delete e;

  ctx.reset("true || (1/0 == 1)");
  e = ctx.parseExpression();
  REQUIRE( *(e->value(ctx).boolean()) == true );
  delete e;
  ctx.reset("false || (1/0 == 1)");
  e = ctx.parseExpression();
  REQUIRE_THROWS( e->value(ctx) );
  delete e;
  ctx.reset("null || (1/0 == 1)");
  e = ctx.parseExpression();
  REQUIRE_THROWS( e->value(ctx) );
  delete e;
}

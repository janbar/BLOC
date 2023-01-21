#include <iostream>
#include <string>
#include <cstring>
#include <climits>

#include <test.h>
#include <hashvalue.c>

TestingContext ctx;

using namespace bloc;

TEST_CASE("PI")
{
  Expression * e;
  ctx.reset("round( pi, 6 )");
  e = ctx.parseExpression();
  REQUIRE( e->numeric(ctx) == 3.141593 );
  delete e;
}

TEST_CASE("EE (e euler)")
{
  Expression * e;
  ctx.reset("round( ee, 6 )");
  e = ctx.parseExpression();
  REQUIRE( e->numeric(ctx) == 2.718282 );
  delete e;
}

TEST_CASE("PHI (gold number)")
{
  Expression * e;
  ctx.reset("round( phi , 6 )");
  e = ctx.parseExpression();
  REQUIRE( e->numeric(ctx) == 1.618034 );
  delete e;
}

TEST_CASE("TRUE")
{
  Expression * e;
  ctx.reset("true");
  e = ctx.parseExpression();
  REQUIRE( e->boolean(ctx) == true );
  delete e;
}

TEST_CASE("FALSE")
{
  Expression * e;
  ctx.reset("false");
  e = ctx.parseExpression();
  REQUIRE( e->boolean(ctx) == false );
  delete e;
}

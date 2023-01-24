#include <iostream>
#include <string>
#include <cstring>
#include <climits>
#include <cmath>

#include <test.h>
#include <hashvalue.c>

TestingContext ctx;

using namespace bloc;

bool fequal(double a, double b)
{
  return std::fabs(a - b) < 1e-6;
}

TEST_CASE("operator I + N")
{
  ctx.reset("7687 + 43453.0123456");
  Expression * e = ctx.parseExpression();
  REQUIRE( fequal(e->numeric(ctx), 51140.0123456) );
  delete e;
}

TEST_CASE("operator I - N")
{
  ctx.reset("7687 - 43453.0123456");
  Expression * e = ctx.parseExpression();
  REQUIRE( fequal(e->numeric(ctx), -35766.0123456) );
  delete e;
}

TEST_CASE("operator I * N")
{
  ctx.reset("7687 * 43453.0123456");
  Expression * e = ctx.parseExpression();
  REQUIRE( fequal(e->numeric(ctx), 334023305.9006272) );
  delete e;
}

TEST_CASE("operator I / N")
{
  ctx.reset("round( 43453 / 7687.0123456 , 9 )");
  Expression * e = ctx.parseExpression();
  REQUIRE( fequal(e->numeric(ctx), 5.652781347) );
  delete e;
}

TEST_CASE("operator N % I")
{
  ctx.reset("round( 43453.0123456 % 7687 , 7 )");
  Expression * e = ctx.parseExpression();
  REQUIRE( fequal(e->numeric(ctx), 5018.0123456) );
  delete e;
}

TEST_CASE("operator I ** N (POWER)")
{
  Expression * e;
  ctx.reset("round( 767 ** 3.012345 , 5 )");
  e = ctx.parseExpression();
  REQUIRE( fequal(e->numeric(ctx), 489777568.25277) );
  delete e;
  ctx.reset("round( 767 power 3.012345 , 5 )");
  e = ctx.parseExpression();
  REQUIRE( fequal(e->numeric(ctx), 489777568.25277) );
  delete e;
}

TEST_CASE("operator N ** I (POWER)")
{
  ctx.reset("round( 767.012345 ** 3 , 6 )");
  Expression * e = ctx.parseExpression();
  REQUIRE( fequal(e->numeric(ctx), 451239450.633787) );
  delete e;
}

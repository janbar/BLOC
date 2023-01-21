#include <iostream>
#include <string>
#include <cstring>
#include <climits>

#include <test.h>
#include <hashvalue.c>

TestingContext ctx;

using namespace bloc;

TEST_CASE("operator N + N")
{
  ctx.reset("7687.0123456 + 43453.0123456");
  Expression * e = ctx.parseExpression();
  REQUIRE( e->numeric(ctx) == 51140.0246912 );
  delete e;
}

TEST_CASE("operator N - N")
{
  ctx.reset("7687.0123456 - 43453.0123456");
  Expression * e = ctx.parseExpression();
  REQUIRE( e->numeric(ctx) == -35766.0 );
  delete e;
}

TEST_CASE("operator N * N")
{
  ctx.reset("7687.0123456 * 43453.0123456");
  Expression * e = ctx.parseExpression();
  REQUIRE( e->numeric(ctx) == 334023842.354136414 );
  delete e;
}

TEST_CASE("operator N / N")
{
  ctx.reset("round( 43453.0123456 / 7687.0123456 , 9 )");
  Expression * e = ctx.parseExpression();
  REQUIRE( e->numeric(ctx) == 5.652782953 );
  delete e;
}

TEST_CASE("operator N % N")
{
  ctx.reset("round( 43453.0123456 % 7687.0123456 , 7 )");
  Expression * e = ctx.parseExpression();
  REQUIRE( e->numeric(ctx) == 5017.9506176 );
  delete e;
}

TEST_CASE("operator N ** N (POWER)")
{
  Expression * e;
  ctx.reset("round( 767.012345 ** 3.012345 , 5 )");
  e = ctx.parseExpression();
  REQUIRE( e->numeric(ctx) == 489801315.12217 );
  delete e;
  ctx.reset("round( 767.012345 power 3.012345 , 5 )");
  e = ctx.parseExpression();
  REQUIRE( e->numeric(ctx) == 489801315.12217 );
  delete e;
  ctx.reset("round( 767.012345 ** 3.0 , 6 )");
  e = ctx.parseExpression();
  REQUIRE( e->numeric(ctx) == 451239450.633787 );
  delete e;
}

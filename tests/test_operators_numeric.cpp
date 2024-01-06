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

TEST_CASE("operator N + N")
{
  ctx.reset("7687.0123456 + 43453.0123456");
  Expression * e = ctx.parseExpression();
  REQUIRE( fequal(*(e->value(ctx).numeric()), 51140.0246912) );
  delete e;
}

TEST_CASE("operator N - N")
{
  ctx.reset("7687.0123456 - 43453.0123456");
  Expression * e = ctx.parseExpression();
  REQUIRE( fequal(*(e->value(ctx).numeric()), -35766.0) );
  delete e;
}

TEST_CASE("operator N * N")
{
  ctx.reset("7687.0123456 * 43453.0123456");
  Expression * e = ctx.parseExpression();
  REQUIRE( fequal(*(e->value(ctx).numeric()), 334023842.354136414) );
  delete e;
}

TEST_CASE("operator N / N")
{
  ctx.reset("round( 43453.0123456 / 7687.0123456 , 9 )");
  Expression * e = ctx.parseExpression();
  REQUIRE( fequal(*(e->value(ctx).numeric()), 5.652782953) );
  delete e;
}

TEST_CASE("operator N % N")
{
  ctx.reset("round( 43453.0123456 % 7687.0123456 , 7 )");
  Expression * e = ctx.parseExpression();
  REQUIRE( fequal(*(e->value(ctx).numeric()), 5017.9506176) );
  delete e;
}

TEST_CASE("operator N ** N (POWER)")
{
  Expression * e;
  ctx.reset("round( 767.012345 ** 3.012345 , 3 )");
  e = ctx.parseExpression();
  REQUIRE( fequal(*(e->value(ctx).numeric()), 489801315.122) );
  delete e;
  ctx.reset("round( 767.012345 power 3.012345 , 3 )");
  e = ctx.parseExpression();
  REQUIRE( fequal(*(e->value(ctx).numeric()), 489801315.122) );
  delete e;
  ctx.reset("round( 767.012345 ** 3.0 , 3 )");
  e = ctx.parseExpression();
  REQUIRE( fequal(*(e->value(ctx).numeric()), 451239450.634) );
  delete e;
}

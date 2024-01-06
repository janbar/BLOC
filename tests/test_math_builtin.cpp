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

TEST_CASE("isnum")
{
  Expression * e;
  ctx.reset("isnum( pi )");
  e = ctx.parseExpression();
  REQUIRE( *(e->value(ctx).boolean()) == true );
  delete e;
  ctx.reset("isnum( 123456.0 )");
  e = ctx.parseExpression();
  REQUIRE( *(e->value(ctx).boolean()) == true );
  delete e;
  ctx.reset("isnum( 123456 )");
  e = ctx.parseExpression();
  REQUIRE( *(e->value(ctx).boolean()) == true );
  delete e;
  ctx.reset("isnum( 0xff20 )");
  e = ctx.parseExpression();
  REQUIRE( *(e->value(ctx).boolean()) == true );
  delete e;
  ctx.reset("isnum( \"123456.0\" )");
  e = ctx.parseExpression();
  REQUIRE( *(e->value(ctx).boolean()) == true );
  delete e;
  ctx.reset("isnum( \"123456789\" )");
  e = ctx.parseExpression();
  REQUIRE( *(e->value(ctx).boolean()) == true );
  delete e;
  ctx.reset("isnum( \"1.23456e-9\" )");
  e = ctx.parseExpression();
  REQUIRE( *(e->value(ctx).boolean()) == true );
  delete e;
  ctx.reset("isnum( \"abcdef\" )");
  e = ctx.parseExpression();
  REQUIRE( *(e->value(ctx).boolean()) == false );
  delete e;
  ctx.reset("isnum( \"123abcdef\" )");
  e = ctx.parseExpression();
  REQUIRE( *(e->value(ctx).boolean()) == true );
  delete e;
  ctx.reset("isnum( raw(16,0x20) )");
  e = ctx.parseExpression();
  REQUIRE( *(e->value(ctx).boolean()) == false );
  delete e;
  ctx.reset("isnum( tup(123456.0, 456789) )");
  e = ctx.parseExpression();
  REQUIRE( *(e->value(ctx).boolean()) == false );
  delete e;
  ctx.reset("isnum( tup(123456.0, 456789)@1 )");
  e = ctx.parseExpression();
  REQUIRE( *(e->value(ctx).boolean()) == true );
  delete e;
  ctx.reset("isnum( tab(10, 123456) )");
  e = ctx.parseExpression();
  REQUIRE( *(e->value(ctx).boolean()) == false );
  delete e;
  ctx.reset("isnum( tab(10, 123456).at(2) )");
  e = ctx.parseExpression();
  REQUIRE( *(e->value(ctx).boolean()) == true );
  delete e;
  ctx.reset("isnum( null )");
  e = ctx.parseExpression();
  REQUIRE( *(e->value(ctx).boolean()) == false );
  delete e;
  ctx.reset("isnum( true )");
  e = ctx.parseExpression();
  REQUIRE( *(e->value(ctx).boolean()) == false );
  delete e;
}

TEST_CASE("min")
{
  Expression * e;
  ctx.reset("min( 123456.0, 456789.0 )");
  e = ctx.parseExpression();
  REQUIRE( fequal(*(e->value(ctx).numeric()), 123456.0) );
  delete e;
}

TEST_CASE("max")
{
  Expression * e;
  ctx.reset("max( 123456.0, 456789.0 )");
  e = ctx.parseExpression();
  REQUIRE( fequal(*(e->value(ctx).numeric()), 456789.0) );
  delete e;
}

TEST_CASE("floor")
{
  Expression * e;
  ctx.reset("floor( 123.5698 )");
  e = ctx.parseExpression();
  REQUIRE( fequal(*(e->value(ctx).numeric()), 123.0) );
  delete e;
  ctx.reset("floor( -123.5698 )");
  e = ctx.parseExpression();
  REQUIRE( fequal(*(e->value(ctx).numeric()), -124.0) );
  delete e;
}

TEST_CASE("ceil")
{
  Expression * e;
  ctx.reset("ceil( 123.5698 )");
  e = ctx.parseExpression();
  REQUIRE( fequal(*(e->value(ctx).numeric()), 124.0) );
  delete e;
  ctx.reset("floor( -123.5698 )");
  e = ctx.parseExpression();
  REQUIRE( fequal(*(e->value(ctx).numeric()), -124.0) );
  delete e;
}

TEST_CASE("abs")
{
  Expression * e;
  ctx.reset("abs( -2.32323 )");
  e = ctx.parseExpression();
  REQUIRE( fequal(*(e->value(ctx).numeric()), 2.32323) );
  delete e;
  ctx.reset("abs( +2.32323 )");
  e = ctx.parseExpression();
  REQUIRE( fequal(*(e->value(ctx).numeric()), 2.32323) );
  delete e;
}

TEST_CASE("sign")
{
  Expression * e;
  ctx.reset("sign( -2.32323 )");
  e = ctx.parseExpression();
  REQUIRE( fequal(*(e->value(ctx).numeric()) , -1.0) );
  delete e;
  ctx.reset("sign( +2.32323 )");
  e = ctx.parseExpression();
  REQUIRE( fequal(*(e->value(ctx).numeric()), 1.0) );
  delete e;
}

TEST_CASE("sin")
{
  Expression * e;
  ctx.reset("round( sin( pi ), 6)");
  e = ctx.parseExpression();
  REQUIRE( fequal(*(e->value(ctx).numeric()), 0.0) );
  delete e;
  ctx.reset("round( sin( pi/2 ), 6)");
  e = ctx.parseExpression();
  REQUIRE( fequal(*(e->value(ctx).numeric()), 1.0) );
  delete e;
}

TEST_CASE("asin")
{
  Expression * e;
  ctx.reset("round( asin( 1.0 ), 6)");
  e = ctx.parseExpression();
  REQUIRE( fequal(*(e->value(ctx).numeric()), 1.570796) );
  delete e;
}

TEST_CASE("cos")
{
  Expression * e;
  ctx.reset("round( cos( pi ), 6)");
  e = ctx.parseExpression();
  REQUIRE( fequal(*(e->value(ctx).numeric()), -1.0) );
  delete e;
  ctx.reset("round( cos( pi/2 ), 6)");
  e = ctx.parseExpression();
  REQUIRE( fequal(*(e->value(ctx).numeric()), 0.0) );
  delete e;
}

TEST_CASE("acos")
{
  Expression * e;
  ctx.reset("round( acos( -1.0 ), 6)");
  e = ctx.parseExpression();
  REQUIRE( fequal(*(e->value(ctx).numeric()), 3.141593) );
  delete e;
}

TEST_CASE("tan")
{
  Expression * e;
  ctx.reset("round( tan( pi ), 6)");
  e = ctx.parseExpression();
  REQUIRE( fequal(*(e->value(ctx).numeric()), 0.0) );
  delete e;
  ctx.reset("round( tan( pi/4 ), 6)");
  e = ctx.parseExpression();
  REQUIRE( fequal(*(e->value(ctx).numeric()), 1.0) );
  delete e;
}

TEST_CASE("atan")
{
  Expression * e;
  ctx.reset("round( atan( 1.0 ), 6)");
  e = ctx.parseExpression();
  REQUIRE( fequal(*(e->value(ctx).numeric()), 0.785398) );
  delete e;
}

TEST_CASE("int")
{
  Expression * e;
  ctx.reset("int( pi )");
  e = ctx.parseExpression();
  REQUIRE( *(e->value(ctx).integer()) == 3 );
  delete e;
  ctx.reset("int( 0xff20 )");
  e = ctx.parseExpression();
  REQUIRE( *(e->value(ctx).integer()) == 0xff20 );
  delete e;
  ctx.reset("int( \"-123456\" )");
  e = ctx.parseExpression();
  REQUIRE( *(e->value(ctx).integer()) == -123456 );
  delete e;
}

TEST_CASE("num")
{
  Expression * e;
  ctx.reset("round( num( pi ), 6)");
  e = ctx.parseExpression();
  REQUIRE( fequal(*(e->value(ctx).numeric()), 3.141593) );
  delete e;
  ctx.reset("round( num( \"1.23456789e-3\" ), 6)");
  e = ctx.parseExpression();
  REQUIRE( fequal(*(e->value(ctx).numeric()), 0.001235) );
  delete e;
  ctx.reset("round( num( \"123456789\" ), 6)");
  e = ctx.parseExpression();
  REQUIRE( fequal(*(e->value(ctx).numeric()), 123456789.0) );
  delete e;
}

TEST_CASE("pow")
{
  Expression * e;
  ctx.reset("round( pow( ee, 2 ), 6)");
  e = ctx.parseExpression();
  REQUIRE( fequal(*(e->value(ctx).numeric()), 7.389056) );
  delete e;
  ctx.reset("round( pow( ee, -2 ), 6)");
  e = ctx.parseExpression();
  REQUIRE( fequal(*(e->value(ctx).numeric()), 0.135335) );
  delete e;
}

TEST_CASE("sqrt")
{
  Expression * e;
  ctx.reset("round( sqrt( ee ), 6)");
  e = ctx.parseExpression();
  REQUIRE( fequal(*(e->value(ctx).numeric()), 1.648721) );
  delete e;
  ctx.reset("round( sqrt( -ee ), 6)");
  e = ctx.parseExpression();
  REQUIRE( std::isnan(*(e->value(ctx).numeric())) );
  delete e;
}

TEST_CASE("log")
{
  Expression * e;
  ctx.reset("round( log( ee ), 6)");
  e = ctx.parseExpression();
  REQUIRE( fequal(*(e->value(ctx).numeric()), 1.0) );
  delete e;
}

TEST_CASE("log10")
{
  Expression * e;
  ctx.reset("round( log10( 1000.0 ), 6)");
  e = ctx.parseExpression();
  REQUIRE( fequal(*(e->value(ctx).numeric()), 3.0) );
  delete e;
}

TEST_CASE("hash")
{
  Expression * e;
  ctx.reset("hash( raw(0,0) )");
  e = ctx.parseExpression();
  REQUIRE( *(e->value(ctx).integer()) == 5381 );
  delete e;
  ctx.reset("hash( raw(256,0x41) )");
  e = ctx.parseExpression();
  REQUIRE( *(e->value(ctx).integer()) == 3920553477 );
  delete e;
  ctx.reset("hash( str(raw(256,0x41)) )");
  e = ctx.parseExpression();
  REQUIRE( *(e->value(ctx).integer()) == 3920553477 );
  delete e;
}

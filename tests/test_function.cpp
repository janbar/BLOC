#include <iostream>
#include <string>
#include <cstring>
#include <climits>

#include <test.h>
#include <hashvalue.c>
#include <blocc/expression_integer.h>

TestingContext ctx;

using namespace bloc;

TEST_CASE("raise user")
{
  ctx.purge();
  Executable * e;
  ctx.reset(
          "begin\n"
          "  raise myerror;\n"
          "exception\n"
          "when divide_by_zero then return 1;"
          "when myerror then return 2;"
          "when others then return 3;"
          "end; return 0;"
  );
  e = ctx.parse();
  REQUIRE( e->run() == 0 );
  delete e;
  Expression * r = ctx.dropReturned();
  REQUIRE( r->integer(ctx) == 2 );
  delete r;
}

TEST_CASE("raise divide_by_zero")
{
  ctx.purge();
  Executable * e;
  ctx.reset(
          "begin\n"
          "  raise divide_by_zero;\n"
          "exception\n"
          "when divide_by_zero then return 4;"
          "when myerror then return 5;"
          "when others then return 6;"
          "end; return 0;"
  );
  e = ctx.parse();
  REQUIRE( e->run() == 0 );
  delete e;
  Expression * r = ctx.dropReturned();
  REQUIRE( r->integer(ctx) == 4 );
  delete r;
}

TEST_CASE("throw divide_by_zero")
{
  ctx.purge();
  Executable * e;
  ctx.reset(
          "begin\n"
          "  do 1/0;\n"
          "exception\n"
          "when myerror then return 7;"
          "when divide_by_zero then return 8;"
          "when others then return 9;"
          "end; return 0;"
  );
  e = ctx.parse();
  REQUIRE( e->run() == 0 );
  delete e;
  Expression * r = ctx.dropReturned();
  REQUIRE( r->integer(ctx) == 8 );
  delete r;
}

TEST_CASE("raise out_of_range")
{
  ctx.purge();
  Executable * e;
  ctx.reset(
          "begin\n"
          "  raise out_of_range;\n"
          "exception\n"
          "when myerror then return 10;"
          "when out_of_range then return 11;"
          "when others then return 12;"
          "end; return 0;"
  );
  e = ctx.parse();
  REQUIRE( e->run() == 0 );
  delete e;
  Expression * r = ctx.dropReturned();
  REQUIRE( r->integer(ctx) == 11 );
  delete r;
}

TEST_CASE("raise others")
{
  ctx.purge();
  Executable * e;
  ctx.reset(
          "begin\n"
          "  raise out_of_range;\n"
          "exception\n"
          "when divide_by_zero then return 13;"
          "when myerror then return 14;"
          "when others then return 15;"
          "end; return 0;"
  );
  e = ctx.parse();
  REQUIRE( e->run() == 0 );
  delete e;
  Expression * r = ctx.dropReturned();
  REQUIRE( r->integer(ctx) == 15 );
  delete r;
}

TEST_CASE("raise not catched")
{
  ctx.purge();
  Executable * e;
  ctx.reset(
          "begin\n"
          "  raise out_of_range;\n"
          "exception\n"
          "when divide_by_zero then return 16;"
          "when myerror then return 17;"
          "end; return 0;"
  );
  e = ctx.parse();
  REQUIRE_THROWS_AS( e->run(), bloc::RuntimeError);
  delete e;
}

TEST_CASE("unstacking on exception")
{
  ctx.purge();
  Executable * e;
  Symbol& a1 = ctx.registerSymbol("$1", Type::INTEGER);
  ctx.storeVariable(a1, IntegerExpression(100));
  ctx.reset(
          "brk=$1*2, cnt=0;\n"
          "for i in 1 to $1 loop\n"
          "  brk=brk-1; if brk <= 0 then return 0; end if;\n"
          "  begin\n"
          "    for j in 2 to i/2 asc loop\n"
          "      if i%j == 0 then raise notprim; end if;\n"
          "    end loop;\n"
          "    cnt=cnt+1;\n"
          "  exception when notprim then nop;\n"
          "  end;\n"
          "end loop;\n"
          "return cnt;"
  );
  e = ctx.parse();
  REQUIRE( e->run() == 0 );
  delete e;
  Expression * r = ctx.dropReturned();
  REQUIRE( r != nullptr );
  REQUIRE( r->integer(ctx) == 26 );
  delete r;
}

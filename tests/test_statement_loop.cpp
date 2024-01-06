#include <iostream>
#include <string>
#include <cstring>
#include <climits>

#include <test.h>
#include <hashvalue.c>

TestingContext ctx;

using namespace bloc;

bool fequal(double a, double b)
{
  return std::fabs(a - b) < 1e-6;
}

TEST_CASE("for loop")
{
  ctx.purge();
  Executable * e;
  ctx.reset("for i in 1 to 10 loop i=i*2; end loop;\n");
  e = ctx.parse();
  REQUIRE( e->run() == 0 );
  delete e;
  REQUIRE( *(ctx.loadVariable("I")->integer()) == 14 );

  ctx.reset(
    "i=null, t=0;\n"
    "for i in 10 to 1 desc loop t=t+i; end loop;\n"
  );
  e = ctx.parse();
  REQUIRE( e->run() == 0 );
  delete e;
  REQUIRE( *(ctx.loadVariable("I")->integer()) == 1 );
  REQUIRE( *(ctx.loadVariable("T")->integer()) == 55 );

  ctx.reset(
    "for i in 10 to 1 desc loop break; end loop;\n"
  );
  e = ctx.parse();
  REQUIRE( e->run() == 0 );
  delete e;
  REQUIRE( *(ctx.loadVariable("I")->integer()) == 10 );

  ctx.reset("for i in 0 to 1 loop i=true; break; end loop;\n");
  try { e = ctx.parse(); FAIL("No throw"); }
  catch (...) { SUCCEED("Throw"); }
}

TEST_CASE("forall loop")
{
  ctx.purge();
  Executable * e;
  ctx.reset("a=tab(10, tab(5, tup(pi,\"abcde\",false)));");
  e = ctx.parse();
  REQUIRE( e->run() == 0 );
  delete e;

  ctx.reset(
    "i=null, t=0;\n"
    "forall i in a loop t=t+i.count(); end loop;\n"
  );
  e = ctx.parse();
  REQUIRE( e->run() == 0 );
  delete e;
  REQUIRE( *(ctx.loadVariable("T")->integer()) == 50 );

  ctx.reset(
    "i=null, t=0;\n"
    "forall i in a loop i.at(1)@2.concat(\"fghijk\"); end loop;\n"
    "r=a.at(8).at(1)@2;\n"
  );
  e = ctx.parse();
  REQUIRE( e->run() == 0 );
  delete e;
  REQUIRE( *(ctx.loadVariable("R")->literal()) == "abcdefghijk" );

  ctx.reset(
    "a=tab(10, ee), j=0, t=0;\n"
    "forall i in a loop j=j+1; i=i*j; end loop;\n"
    "forall i in a loop t=t+i; end loop;\n"
  );
  e = ctx.parse();
  REQUIRE( e->run() == 0 );
  delete e;
  REQUIRE( fequal(*(ctx.loadVariable("T")->numeric()), 149.505501) );

  ctx.reset("forall i in a loop i=true; break; end loop;\n");
  try { e = ctx.parse(); FAIL("No throw"); }
  catch (...) { SUCCEED("Throw"); }
}

#include <iostream>
#include <string>
#include <cstring>
#include <climits>

#include <test.h>
#include <hashvalue.c>

TestingContext ctx;

using namespace bloc;

TEST_CASE("perf 10k hash 10KB")
{
  Executable * e;
  ctx.reset("for i in 1 to 10000 loop do hash( raw(10240,0x41) ); end loop;");
  e = ctx.parse();
  REQUIRE( e->run() == 0 );
  delete e;
}

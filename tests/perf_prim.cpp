#include <iostream>
#include <string>
#include <cstring>
#include <climits>

#include <test.h>
#include <hashvalue.c>
#include <blocc/expression_integer.h>

TestingContext ctx;

using namespace bloc;

TEST_CASE("perf 10000 prims")
{
  Executable * e;
  Symbol& a1 = ctx.registerSymbol("$1", Type::INTEGER);
  ctx.storeVariable(a1, Value(Integer((10000))));
  ctx.reset(
          "cnt=0;\nfor i in 1 to $1 loop\nb=true;\n"
          "for j in 2 to i/2 asc loop\nif i%j == 0 then b=false; break; end if;\n"
          "end loop;\nif b then cnt=cnt+1; end if;\nend loop;\nreturn cnt;"
  );
  e = ctx.parse();
  REQUIRE( e->run() == 0 );
  delete e;
  Value * r = ctx.dropReturned();
  REQUIRE( *(r->integer()) == 1230 );
  delete r;
}

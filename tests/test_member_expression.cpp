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

TEST_CASE("member count")
{
  ctx.purge();
  Executable * e;

  ctx.reset("b=null.count();\n");
  e = ctx.parse();
  REQUIRE( e->run() == 0 );
  delete e;
  REQUIRE( ctx.loadVariable("B")->integer() == nullptr );

  ctx.reset("a=\"abcdef\", b=a.count();\n");
  e = ctx.parse();
  REQUIRE( e->run() == 0 );
  delete e;
  REQUIRE( *(ctx.loadVariable("B")->integer()) == 6 );
}

TEST_CASE("member at")
{
  ctx.purge();
  Executable * e;

  ctx.reset("b=null.at(0);\n");
  e = ctx.parse();
  try { e->run(); FAIL("No throw"); }
  catch(RuntimeError& re) { SUCCEED(re.what()); }
  delete e;

  ctx.reset("a=\"abcdef\", b=a.at(3);\n");
  e = ctx.parse();
  REQUIRE( e->run() == 0 );
  delete e;
  REQUIRE( *(ctx.loadVariable("B")->integer()) == 100 );

  ctx.reset("a=\"abcdef\".at(null);\n");
  e = ctx.parse();
  try { e->run(); FAIL("No throw"); }
  catch(RuntimeError& re) { SUCCEED(re.what()); }
  delete e;
}

TEST_CASE("member delete")
{
  ctx.purge();
  Executable * e;

  ctx.reset("b=null.delete(0);\n");
  e = ctx.parse();
  try { e->run(); FAIL("No throw"); }
  catch(RuntimeError& re) { SUCCEED(re.what()); }
  delete e;

  ctx.reset("a=\"abcdef\", b=a.delete(3).delete(4);\n");
  e = ctx.parse();
  REQUIRE( e->run() == 0 );
  delete e;
  REQUIRE( *(ctx.loadVariable("B")->literal()) == "abce" );

  ctx.reset("a=\"abcdef\".delete(null);\n");
  e = ctx.parse();
  try { e->run(); FAIL("No throw"); }
  catch(RuntimeError& re) { SUCCEED(re.what()); }
  delete e;
}

TEST_CASE("member put")
{
  ctx.purge();
  Executable * e;

  ctx.reset("b=null.put(0, 65);\n");
  e = ctx.parse();
  try { e->run(); FAIL("No throw"); }
  catch(RuntimeError& re) { SUCCEED(re.what()); }
  delete e;

  ctx.reset("b=null.put(0, tup(1));\n");
  e = ctx.parse();
  try { e->run(); FAIL("No throw"); }
  catch(RuntimeError& re) { SUCCEED(re.what()); }
  delete e;

  ctx.reset("b=null.put(0, tab(1,1));\n");
  e = ctx.parse();
  try { e->run(); FAIL("No throw"); }
  catch(RuntimeError& re) { SUCCEED(re.what()); }
  delete e;

  ctx.reset("a=\"abcdef\", b=a.put(3, 65);\n");
  e = ctx.parse();
  REQUIRE( e->run() == 0 );
  delete e;
  REQUIRE( *(ctx.loadVariable("B")->literal()) == "abcAef" );

  ctx.reset("a=\"abcdef\", b=a.put(6, 65);\n");
  e = ctx.parse();
  try { e->run(); FAIL("No throw"); }
  catch(RuntimeError& re) { SUCCEED(re.what()); }
  delete e;

  ctx.reset("a=\"abcdef\", b=a.put(null, 65);\n");
  e = ctx.parse();
  try { e->run(); FAIL("No throw"); }
  catch(RuntimeError& re) { SUCCEED(re.what()); }
  delete e;

  ctx.reset("a=\"abcdef\", b=a.put(0, null);\n");
  e = ctx.parse();
  try { e->run(); FAIL("No throw"); }
  catch(RuntimeError& re) { SUCCEED(re.what()); }
  delete e;
}

TEST_CASE("member insert")
{
  ctx.purge();
  Executable * e;

  ctx.reset("b=null.insert(0, 65);\n");
  e = ctx.parse();
  try { e->run(); FAIL("No throw"); }
  catch(RuntimeError& re) { SUCCEED(re.what()); }
  delete e;

  ctx.reset("b=null.insert(0, tup(1));\n");
  e = ctx.parse();
  try { e->run(); FAIL("No throw"); }
  catch(RuntimeError& re) { SUCCEED(re.what()); }
  delete e;

  ctx.reset("b=null.insert(0, tab(1,1));\n");
  e = ctx.parse();
  try { e->run(); FAIL("No throw"); }
  catch(RuntimeError& re) { SUCCEED(re.what()); }
  delete e;
}

TEST_CASE("member concat")
{
  ctx.purge();
  Executable * e;

  ctx.reset("b=null.concat(tup(1));\n");
  e = ctx.parse();
  try { e->run(); FAIL("No throw"); }
  catch(RuntimeError& re) { SUCCEED(re.what()); }
  delete e;

  ctx.reset("b=null.concat(tab(1,1));\n");
  e = ctx.parse();
  try { e->run(); FAIL("No throw"); }
  catch(RuntimeError& re) { SUCCEED(re.what()); }
  delete e;

  ctx.reset("b=null.concat(65);\n");
  e = ctx.parse();
  REQUIRE( e->run() == 0 );
  delete e;
  REQUIRE( *(ctx.loadVariable("B")->literal()) == "A" );

  ctx.reset("b=null.concat(0);\n");
  e = ctx.parse();
  REQUIRE( e->run() == 0 );
  delete e;
  REQUIRE( ctx.loadVariable("B")->tabchar()->at(0) == (char)0 );

  ctx.reset("b=null.concat(\"abcdef\");\n");
  e = ctx.parse();
  REQUIRE( e->run() == 0 );
  delete e;
  REQUIRE( *(ctx.loadVariable("B")->literal()) == "abcdef" );
}

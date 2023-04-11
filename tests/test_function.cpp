#include <iostream>
#include <string>
#include <cstring>
#include <climits>

#include <test.h>
#include <hashvalue.c>
#include <blocc/expression_integer.h>

TestingContext ctx;

using namespace bloc;

TEST_CASE("create and replace function")
{
  ctx.purge();
  Executable * e;
  ctx.reset(
          "function func(r) returns decimal is\n"
          "begin\n"
          "  fn2 = 12.0 , fn1 = 6.0;\n"
          "  for i in 2 to r loop\n"
          "    c = fn1 + fn2;\n"
          "    fn2 = fn1;\n"
          "    fn1 = c;\n"
          "  end loop;\n"
          "  return fn1 / fn2;\n"
          "end;\n"
  );
  e = ctx.parse();
  REQUIRE( e->run() == 0 );
  delete e;

  ctx.reset("a = round(func(100),6);");
  e = ctx.parse();
  REQUIRE( e->run() == 0 );
  delete e;
  REQUIRE( ctx.loadVariable("A")->numeric(ctx) == 1.618034 );

  ctx.reset(
          "function func(s) returns string is\n"
          "begin\n"
          "  return upper(s);\n"
          "end;\n"
  );
  e = ctx.parse();
  REQUIRE( e->run() == 0 );
  delete e;

  ctx.reset("a = func(\"hello world!\");");
  e = ctx.parse();
  REQUIRE( e->run() == 0 );
  delete e;
  REQUIRE( ctx.loadVariable("A")->literal(ctx) == "HELLO WORLD!" );
}

TEST_CASE("function returns table")
{
  ctx.purge();
  Executable * e;
  ctx.reset(
          "function func(t) returns table is\n"
          "begin\n"
          "  for i in 0 to t.count()-1 loop t.put(i, 1); end loop;\n"
          "  return t;\n"
          "end;\n"
  );
  e = ctx.parse();
  REQUIRE( e->run() == 0 );
  delete e;

  ctx.reset(
          "a = func(tab(100,0)), b = 0;\n"
          "for i in 0 to a.count()-1 loop b = b + a.at(i); end loop;");
  e = ctx.parse();
  REQUIRE( e->run() == 0 );
  delete e;
  REQUIRE( ctx.loadVariable("B")->integer(ctx) == 100 );
}

TEST_CASE("function returns tuple")
{
  ctx.purge();
  Executable * e;
  ctx.reset(
          "function func(t) returns tuple is\n"
          "begin\n"
          "  t.set@1(upper(t@1));\n"
          "  return t;\n"
          "end;\n"
  );
  e = ctx.parse();
  REQUIRE( e->run() == 0 );
  delete e;

  ctx.reset(
          "a = func(tup(\"hello\", 0.0, true)), b = a@1;");
  e = ctx.parse();
  REQUIRE( e->run() == 0 );
  delete e;
  REQUIRE( ctx.loadVariable("B")->literal(ctx) == "HELLO" );
}

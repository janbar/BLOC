#include <iostream>
#include <string>
#include <cstring>
#include <climits>

#include <test.h>
#include <hashvalue.c>
#include <blocc/expression_integer.h>

TestingContext ctx;

using namespace bloc;

TEST_CASE("clone with functor")
{
  ctx.purge();
  Executable * e;
  ctx.reset(
          "function func(r) return decimal is\n"
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
  REQUIRE( *(ctx.loadVariable("A")->numeric()) == 1.618034 );

  bloc::Context * ctx_clone1 = ctx.clone();

  // try running e within context 1
  REQUIRE( bloc::Executable::run(*ctx_clone1, e->statements()) == 0);
  REQUIRE( *(ctx_clone1->loadVariable("A")->numeric()) == 1.618034 );

  delete ctx_clone1;
  delete e;
}

TEST_CASE("clone with purge")
{
  Executable * e;
  ctx.reset("a = round(func(100),6);");
  e = ctx.parse();
  REQUIRE( e->run() == 0 );
  REQUIRE( *(ctx.loadVariable("A")->numeric()) == 1.618034 );

  bloc::Context * ctx_clone1 = ctx.clone();

  // try running e within context 1, after the purge of source context
  ctx.purge();

  REQUIRE( bloc::Executable::run(*ctx_clone1, e->statements()) == 0);
  REQUIRE( *(ctx_clone1->loadVariable("A")->numeric()) == 1.618034 );

  delete ctx_clone1;
  delete e;
}

TEST_CASE("clone with override and reparse and delete")
{
  Executable * e;
  ctx.reset(
          "function func(r) return decimal is\n"
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
  REQUIRE( *(ctx.loadVariable("A")->numeric()) == 1.618034 );

  bloc::Context * ctx_clone1 = ctx.clone();
  bloc::Context * ctx_clone2 = ctx_clone1->clone();

  // try override FUNC in context 2, therefore the result won't be the same
  // within context 2
  bloc::StringReader input;
  input.reset(
          "function func(r) return decimal is\n"
          "begin\n"
          "  fn2 = 12.0 , fn1 = 6.0;\n"
          "  for i in 2 to r loop\n"
          "    c = fn1 + fn2;\n"
          "    fn2 = fn1;\n"
          "    fn1 = c;\n"
          "  end loop;\n"
          "  return fn1/fn2/2.0;\n"
          "end;\n"
  );
  e = bloc::Parser::parse(*ctx_clone2, input);
  REQUIRE( e->run() == 0 );
  delete e;

  // try reparse e within context 1
  input.reset("a = round(func(100),6);");
  e = bloc::Parser::parse(*ctx_clone1, input);
  REQUIRE( e != nullptr );
  REQUIRE( e->run() == 0 );
  REQUIRE( *(ctx_clone1->loadVariable("A")->numeric()) == 1.618034 );

  input.reset("b = round(func(100),6);");
  bloc::Executable * e1b = bloc::Parser::parse(*ctx_clone1, input);
  REQUIRE( e1b != nullptr );

  // delete the parsing context 1
  delete ctx_clone1;

  // run e within cloned context 2: it should work because no new variable
  // instance is created in the context 1. So the result shouldn't the same
  // as FUNC has been overriden
  REQUIRE( bloc::Executable::run(*ctx_clone2, e->statements()) == 0);
  REQUIRE( *(ctx_clone2->loadVariable("A")->numeric()) == 0.809017 );

  // running e1b within cloned context 2 should throw SEGFAULT because the
  // variable b hasn't a slot in the context 2
  //REQUIRE_THROWS( bloc::Executable::run(*ctx_clone2, e1b->statements()) );

  delete ctx_clone2;
  delete e1b;
  delete e;
}

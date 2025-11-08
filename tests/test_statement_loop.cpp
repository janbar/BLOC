#include <iostream>
#include <string>
#include <cstring>
#include <climits>

#include <test.h>
#include <hashvalue.c>
#include <blocc/exception_parse.h>

#include "blocc/exception_runtime.h"

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
    "i=null, t=0;\n"
    "for i in 10 to 1 step 2 loop t=t+i; end loop;\n"
  );
  e = ctx.parse();
  REQUIRE( e->run() == 0 );
  delete e;
  REQUIRE( *(ctx.loadVariable("I")->integer()) == 2 );
  REQUIRE( *(ctx.loadVariable("T")->integer()) == 30 );

  ctx.reset(
    "i=null, t=0;\n"
    "for i in 1 to 10 step 2 loop t=t+i; end loop;\n"
  );
  e = ctx.parse();
  REQUIRE( e->run() == 0 );
  delete e;
  REQUIRE( *(ctx.loadVariable("I")->integer()) == 9 );
  REQUIRE( *(ctx.loadVariable("T")->integer()) == 25 );

  ctx.reset(
    "for i in 10 to 1 desc loop break; end loop;\n"
  );
  e = ctx.parse();
  REQUIRE( e->run() == 0 );
  delete e;
  REQUIRE( *(ctx.loadVariable("I")->integer()) == 10 );

  ctx.reset("for i in 0 to 1 loop i=true; break; end loop;\n");
  try { e = ctx.parse(); delete e; FAIL("No throw"); }
  catch(ParseError& pe) { SUCCEED(pe.what()); }
}

TEST_CASE("Safety reset on broken for loop")
{
  ctx.purge();
  Executable * e;
  ctx.reset(
    "for i in 0 to 1 loop break; end loop;\n"
    "i=\"OK\";\n"
  );
  try {
    e = ctx.parse();
    e->run();
    delete e;
    REQUIRE( *(ctx.loadVariable("I")->literal()) == "OK" );
  }
  catch(Error& er) { delete e; FAIL(er.what()); }
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
  try { e = ctx.parse(); delete e; FAIL("No throw"); }
  catch(ParseError& pe) { SUCCEED(pe.what()); }
}

TEST_CASE("Safety reset on broken forall loop")
{
  ctx.purge();
  Executable * e;
  ctx.reset(
    "forall i in tab(2,0) loop break; end loop;\n"
    "i=\"OK\";\n"
  );
  try {
    e = ctx.parse();
    e->run();
    delete e;
    REQUIRE( *(ctx.loadVariable("I")->literal()) == "OK" );
  }
  catch(Error& er) { delete e; FAIL(er.what()); }
}

TEST_CASE("Table constness in forall loop")
{
  ctx.purge();
  Executable * e;
  ctx.reset(
    "a=tab(10, tab(5, tup(pi,\"abcde\",false)));\n"
    "b=tab(10, tab(5, \"Hello\"));\n"
  );
  e = ctx.parse();
  REQUIRE( e->run() == 0 );
  delete e;

  std::cout << "Erase target table:" << std::endl;
  ctx.reset(
    "forall i in a loop a=tab(); end loop;\n"
  );
  try {
    e = ctx.parse();
    delete e;
    FAIL("Compilation should fail.");
  }
  catch(Error& er) {
    std::cout << "\t=> " << er.what() << std::endl;
    SUCCEED(er.what());
  }

  std::cout << "Remove element from target table:" << std::endl;
  ctx.reset(
    "forall i in a loop forall j in i loop\n"
    "i.delete(0);\n"
    "end loop; end loop;\n"
  );
  try {
    e = ctx.parse();
    delete e;
    FAIL("Compilation should fail.");
  }
  catch(Error& er) {
    std::cout << "\t=> " << er.what() << std::endl;
    SUCCEED(er.what());
  }

  std::cout << "Add element to target table:" << std::endl;
  ctx.reset(
    "forall i in a loop forall j in i loop\n"
    "i.concat(tup(0.0,\"nil\",true));\n"
    "end loop; end loop;\n"
  );
  try {
    e = ctx.parse();
    delete e;
    FAIL("Compilation should fail.");
  }
  catch(Error& er) {
    std::cout << "\t=> " << er.what() << std::endl;
    SUCCEED(er.what());
  }

  std::cout << "Insert element in target table:" << std::endl;
  ctx.reset(
    "forall i in a loop forall j in i loop\n"
    "i.insert(0,tup(0.0,\"nil\",true));\n"
    "end loop; end loop;\n"
  );
  try {
    e = ctx.parse();
    delete e;
    FAIL("Compilation should fail.");
  }
  catch(Error& er) {
    std::cout << "\t=> " << er.what() << std::endl;
    SUCCEED(er.what());
  }

  std::cout << "Replace element of target table:" << std::endl;
  ctx.reset(
    "forall i in a loop forall j in i loop\n"
    "i.put(0,tup(0.0,\"nil\",true));\n"
    "end loop; end loop;\n"
  );
  try {
    e = ctx.parse();
    delete e;
    FAIL("Compilation should fail.");
  }
  catch(Error& er) {
    std::cout << "\t=> " << er.what() << std::endl;
    SUCCEED(er.what());
  }

  std::cout << "Update/Mute element of target table:" << std::endl;
  ctx.reset(
    "forall i in b loop forall j in i loop\n"
    "i.at(0).concat(\"World\");\n"
    "end loop; end loop;\n"
  );
  try {
    e = ctx.parse();
    delete e;
    FAIL("Compilation should fail.");
  }
  catch(Error& er) {
    std::cout << "\t=> " << er.what() << std::endl;
    SUCCEED(er.what());
  }

  std::cout << "Replace element item of target table:" << std::endl;
  ctx.reset(
    "forall i in a loop forall j in i loop\n"
    "i.at(0).set@1(0.0);\n"
    "end loop; end loop;\n"
  );
  try {
    e = ctx.parse();
    delete e;
    FAIL("Compilation should fail.");
  }
  catch(Error& er) {
    std::cout << "\t=> " << er.what() << std::endl;
    SUCCEED(er.what());
  }

  std::cout << "Update/Mute element item of target table:" << std::endl;
  ctx.reset(
    "forall i in a loop forall j in i loop\n"
    "i.at(0)@2.concat(\"fghij\");\n"
    "end loop; end loop;\n"
  );
  try {
    e = ctx.parse();
    delete e;
    FAIL("Compilation should fail.");
  }
  catch(Error& er) {
    std::cout << "\t=> " << er.what() << std::endl;
    SUCCEED(er.what());
  }
}

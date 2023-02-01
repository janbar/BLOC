/*
 *      Copyright (C) 2022 Jean-Luc Barriere
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "plugin_utf8.h"
#include "utf8helper.h"

/*
 * Create the module UTF8Import
 */
PLUGINCREATOR(UTF8Plugin)

namespace bloc
{
namespace plugin
{
namespace utf8
{

/*
 * The module interface
 */

/**********************************************************************/
/*  Constructors                                                      */
/**********************************************************************/
static PLUGIN_TYPE ctor_0_args[]  = {
  { "C", 0 }, // utf8
};

static PLUGIN_TYPE ctor_1_args[]  = {
  { "L", 0 }, // string
};

static PLUGIN_CTOR ctors[] =
{
  { 0,      1,  ctor_0_args,
          "Build a new unicode string as a copy of the given one." },
  { 1,      1,  ctor_1_args,
          "Build a new unicode string from the standard string." },
};

enum Method
{
  Empty = 0, Size, Rawsize, Reserve, Clear,
  Append, AppendL, ConcatC,
  Tostring, At,
  Remove, Insert, InsertC, Substr1, Substr2,
  Toupper, Tolower, Normaliz, Capital, Translit,
};

/**********************************************************************/
/*  Method arguments                                                  */
/*  mode:         type: decl,ndim                                     */
/**********************************************************************/
static PLUGIN_ARG int_args[]  = {
  { PLUGIN_IN,    { "I", 0 } }, // integer
};

static PLUGIN_ARG string_args[]  = {
  { PLUGIN_IN,    { "L", 0 } }, // string
};

static PLUGIN_ARG utf8_args[]  = {
  { PLUGIN_IN,    { "C", 0 } }, // utf8
};

static PLUGIN_ARG remove_args[]  = {
  { PLUGIN_IN,    { "I", 0 } }, // pos
  { PLUGIN_IN,    { "I", 0 } }, // size
};

static PLUGIN_ARG insert_args[]  = {
  { PLUGIN_IN,    { "I", 0 } }, // pos
  { PLUGIN_IN,    { "I", 0 } }, // codepoint
};

static PLUGIN_ARG insertC_args[]  = {
  { PLUGIN_IN,    { "I", 0 } }, // integer
  { PLUGIN_IN,    { "C", 0 } }, // utf8
};

static PLUGIN_ARG substr_args[]  = {
  { PLUGIN_IN,    { "I", 0 } }, // pos
  { PLUGIN_IN,    { "I", 0 } }, // size
};

/**********************************************************************/
/*  Methods list                                                      */
/*  id:       name:         ret: decl,ndim  args_count,args:          */
/**********************************************************************/
static PLUGIN_METHOD methods[] =
{
  { Empty,    "empty",      { "B", 0 },     0, nullptr,
          "Checks whether the unicode string is empty." },
  { Size,     "count",      { "I", 0 },     0, nullptr,
          "Returns the count of code points contained in the unicode string." },
  { Rawsize,  "rawsize",    { "I", 0 },     0, nullptr,
          "Returns the size in bytes of the unicode string." },
  { Reserve,  "reserve",    { "B", 0 },     1, int_args,
          "Reserves storage for code point capacity." },
  { Clear,    "clear",      { "B", 0 },     0, nullptr,
          "Clears the contents." },
  { Append,   "append",     { "C", 0 },     1, int_args,
          "Appends a code point to the end." },
  { AppendL,  "append",     { "C", 0 },     1, string_args,
          "Appends a standard string to the end." },
  { ConcatC,  "concat",     { "C", 0 },     1, utf8_args,
          "Appends an unicode string to the end." },
  { Tostring, "string",     { "L", 0 },     0, nullptr,
          "Returns a standard string from the content." },
  { At,       "at",         { "I", 0 },     1, int_args,
          "Returns the code point at the position in content." },
  { Remove,   "remove",     { "B", 0 },     2, remove_args,
          "Removes code points from the position, and up to the given count." },
  { Insert,   "insert",     { "B", 0 },     2, insert_args,
          "Inserts at the position, the given code point." },
  { InsertC,  "insert",     { "I", 0 },     2, insertC_args,
          "Inserts at the position, the given unicode string." },
  { Substr1,  "substr",     { "L", 0 },     1, int_args,
          "Returns a standard string from sub content starting at the position." },
  { Substr2,  "substr",     { "L", 0 },     2, substr_args,
          "Returns a standard string from sub content starting at the position, and up"
          "\nto count of code point." },
  { Toupper,  "toupper",    { "C", 0 },     0, nullptr,
          "Transform the content to upper case." },
  { Tolower,  "tolower",    { "C", 0 },     0, nullptr,
          "Transform the content to lower case." },
  { Normaliz, "normalize",  { "C", 0 },     0, nullptr,
          "Normalize the content, i.e removing extra spaces or control characters." },
  { Capital,  "capitalize", { "C", 0 },     0, nullptr,
          "Capitalize all words of the content." },
  { Translit, "translit",   { "C", 0 },     0, nullptr,
          "Transliterates the content to an ASCII string." },
};

} /* namespace utf8 */

void UTF8Plugin::declareInterface(PLUGIN_INTERFACE * interface)
{
  interface->name = "utf8";
  interface->method_count = sizeof(utf8::methods) / sizeof(PLUGIN_METHOD);
  interface->methods = utf8::methods;
  interface->ctors_count = sizeof(utf8::ctors) / sizeof(PLUGIN_CTOR);
  interface->ctors = utf8::ctors;
}

void * UTF8Plugin::createObject(int ctor_id, bloc::Context& ctx, const std::vector<bloc::Expression*>& args)
{
  switch (ctor_id)
  {
  case 0: /* copy ctor */
  {
    Complex c0 = args[0]->complex(ctx);
    utf8helper::UTF8String * u = static_cast<utf8helper::UTF8String*>(c0.instance());
    return new utf8helper::UTF8String(*u);
  }

  case 1: /* utf8( string ) */
    return new utf8helper::UTF8String(args[0]->literal(ctx));

  default: /* default ctor */
    return new utf8helper::UTF8String();
  }
  return nullptr;
}

void UTF8Plugin::destroyObject(void * object)
{
  utf8helper::UTF8String * u = static_cast<utf8helper::UTF8String*>(object);
  delete u;
}

bloc::Expression * UTF8Plugin::executeMethod(
          bloc::Complex& object_this,
          int method_id,
          bloc::Context& ctx,
          const std::vector<bloc::Expression*>& args
          )
{
  utf8helper::UTF8String * u = static_cast<utf8helper::UTF8String*>(object_this.instance());
  switch (method_id)
  {
  case utf8::Empty:
    return new BooleanExpression(u->Empty());

  case utf8::Size:
    return new IntegerExpression((int64_t) u->Size());

  case utf8::Rawsize:
    return new IntegerExpression((int64_t) u->RawSize());

  case utf8::Reserve:
    u->Reserve(args[0]->integer(ctx));
    return new BooleanExpression(true);

  case utf8::Clear:
    u->Clear();
    return new BooleanExpression(true);

  case utf8::Append:
    u->Append(args[0]->integer(ctx));
    return new ComplexExpression(object_this);

  case utf8::AppendL:
  {
    std::string& str = args[0]->literal(ctx);
    for (auto& c : str)
      u->WriteByte(c);
    return new ComplexExpression(object_this);
  }

  case utf8::ConcatC:
  {
    Complex c0 = args[0]->complex(ctx);
    utf8helper::UTF8String * u0 = static_cast<utf8helper::UTF8String*>(c0.instance());
    u->Append(u0->Data());
    return new ComplexExpression(object_this);
  }

  case utf8::Tostring:
    return new LiteralExpression(u->ToStdString());

  case utf8::At:
    return new IntegerExpression(u->operator[](args[0]->integer(ctx)));

  case utf8::Remove:
    return new BooleanExpression(u->Remove((size_t)args[0]->integer(ctx), (size_t)args[1]->integer(ctx)));

  case utf8::Insert:
    return new BooleanExpression(u->Insert((size_t)args[0]->integer(ctx), (utf8helper::codepoint)args[1]->integer(ctx)));

  case utf8::InsertC:
  {
    Complex c0 = args[1]->complex(ctx);
    utf8helper::UTF8String * u0 = static_cast<utf8helper::UTF8String*>(c0.instance());
    return new IntegerExpression((int64_t)u->Insert((size_t)args[0]->integer(ctx), u0->Data()));
  }

  case utf8::Substr1:
    return new LiteralExpression(u->Substr((size_t)args[0]->integer(ctx)));

  case utf8::Substr2:
    return new LiteralExpression(u->Substr((size_t)args[0]->integer(ctx), (size_t)args[1]->integer(ctx)));

  case utf8::Tolower:
    u->Transform(utf8helper::TransformLower);
    return new ComplexExpression(object_this);

  case utf8::Toupper:
    u->Transform(utf8helper::TransformUpper);
    return new ComplexExpression(object_this);

  case utf8::Normaliz:
    u->Transform(utf8helper::TransformNormalize);
    return new ComplexExpression(object_this);

  case utf8::Capital:
    u->Transform(utf8helper::TransformCapitalize);
    return new ComplexExpression(object_this);

  case utf8::Translit:
    u->Transform(utf8helper::TransformTransliterate);
    return new ComplexExpression(object_this);

  }
  return nullptr;
}

} /* namespace import */
} /* namespace bloc */

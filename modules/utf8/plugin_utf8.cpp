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
  { "L", 0 }, // string
};

static PLUGIN_TYPE ctor_1_args[]  = {
  { "O", 0 }, // utf8
};

static PLUGIN_CTOR ctors[] =
{
  { 0,      1,  ctor_0_args,
          "Build a new unicode string from the standard string." },
  { 1,      1,  ctor_1_args,
          "Build a new unicode string as a copy of the given one." },
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
  { PLUGIN_IN,    { "O", 0 } }, // utf8
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
  { PLUGIN_IN,    { "O", 0 } }, // utf8
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
  { Append,   "append",     { "O", 0 },     1, int_args,
          "Appends a code point to the end." },
  { AppendL,  "append",     { "O", 0 },     1, string_args,
          "Appends a standard string to the end." },
  { ConcatC,  "concat",     { "O", 0 },     1, utf8_args,
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
  { Toupper,  "toupper",    { "O", 0 },     0, nullptr,
          "Transform the content to upper case." },
  { Tolower,  "tolower",    { "O", 0 },     0, nullptr,
          "Transform the content to lower case." },
  { Normaliz, "normalize",  { "O", 0 },     0, nullptr,
          "Normalize the content, i.e removing extra spaces or control characters." },
  { Capital,  "capitalize", { "O", 0 },     0, nullptr,
          "Capitalize all words of the content." },
  { Translit, "translit",   { "O", 0 },     0, nullptr,
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
  case 0: /* utf8( string ) */
  {
    bloc::Value& a0 = args[0]->value(ctx);
    if (a0.isNull())
      return new utf8helper::UTF8String();
    return new utf8helper::UTF8String(*a0.literal());
  }

  case 1: /* copy ctor */
  {
    utf8helper::UTF8String * u = nullptr;
    bloc::Value& a0 = args[0]->value(ctx);
    if (a0.isNull() || (u = static_cast<utf8helper::UTF8String*>(a0.complex()->instance())) == nullptr)
      throw RuntimeError(EXC_RT_OTHER_S, "Invalid arguments.");
    return new utf8helper::UTF8String(*u);
  }

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

bloc::Value * UTF8Plugin::executeMethod(
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
    return new bloc::Value(bloc::Bool(u->Empty()));

  case utf8::Size:
    return new bloc::Value(bloc::Integer(u->Size()));

  case utf8::Rawsize:
    return new bloc::Value(bloc::Integer(u->RawSize()));

  case utf8::Reserve:
  {
    bloc::Value& a0 = args[0]->value(ctx);
    if (a0.isNull())
      throw RuntimeError(EXC_RT_OTHER_S, "Invalid arguments.");
    u->Reserve(*a0.integer());
    return new bloc::Value(bloc::Bool(true));
  }

  case utf8::Clear:
    u->Clear();
    return new bloc::Value(bloc::Bool(true));

  case utf8::Append:
  {
    bloc::Value& a0 = args[0]->value(ctx);
    if (!a0.isNull())
      u->Append(*a0.integer());
    return new bloc::Value(new bloc::Complex(object_this));
  }

  case utf8::AppendL:
  {
    bloc::Value& a0 = args[0]->value(ctx);
    if (!a0.isNull())
    {
      for (auto& c : *a0.literal())
        u->WriteByte(c);
    }
    return new bloc::Value(new bloc::Complex(object_this));
  }

  case utf8::ConcatC:
  {
    bloc::Value& a0 = args[0]->value(ctx);
    if (!a0.isNull())
    {
      utf8helper::UTF8String * u0 = static_cast<utf8helper::UTF8String*>(a0.complex()->instance());
      u->Append(u0->Data());
    }
    return new bloc::Value(new bloc::Complex(object_this));
  }

  case utf8::Tostring:
    return new bloc::Value(new bloc::Literal(u->ToStdString()));

  case utf8::At:
  {
    bloc::Value& a0 = args[0]->value(ctx);
    if (a0.isNull())
      throw RuntimeError(EXC_RT_OTHER_S, "Invalid arguments.");
    return new bloc::Value(bloc::Integer(u->operator[](*a0.integer())));
  }

  case utf8::Remove:
  {
    bloc::Value& a0 = args[0]->value(ctx);
    bloc::Value& a1 = args[1]->value(ctx);
    if (a0.isNull() || a1.isNull())
      throw RuntimeError(EXC_RT_OTHER_S, "Invalid arguments.");
    return new bloc::Value(bloc::Bool(u->Remove((size_t)*a0.integer(), (size_t)*a1.integer())));
  }

  case utf8::Insert:
  {
    bloc::Value& a0 = args[0]->value(ctx);
    bloc::Value& a1 = args[1]->value(ctx);
    if (a0.isNull())
      throw RuntimeError(EXC_RT_OTHER_S, "Invalid arguments.");
    if (!a1.isNull())
      return new bloc::Value(bloc::Bool(u->Insert((size_t)*a0.integer(), (utf8helper::codepoint)*a1.integer())));
    return new bloc::Value(bloc::Bool(false));
  }

  case utf8::InsertC:
  {
    bloc::Value& a0 = args[0]->value(ctx);
    bloc::Value& a1 = args[1]->value(ctx);
    if (a0.isNull())
      throw RuntimeError(EXC_RT_OTHER_S, "Invalid arguments.");
    if (!a1.isNull())
    {
      utf8helper::UTF8String * u1 = static_cast<utf8helper::UTF8String*>(a1.complex()->instance());
      return new bloc::Value(bloc::Integer(u->Insert((size_t)*a0.integer(), u1->Data())));
    }
    return new bloc::Value(bloc::Integer(0));
  }

  case utf8::Substr1:
  {
    bloc::Value& a0 = args[0]->value(ctx);
    if (a0.isNull())
      throw RuntimeError(EXC_RT_OTHER_S, "Invalid arguments.");
    return new bloc::Value(new bloc::Literal(u->Substr((size_t)*a0.integer())));
  }

  case utf8::Substr2:
  {
    bloc::Value& a0 = args[0]->value(ctx);
    bloc::Value& a1 = args[1]->value(ctx);
    if (a0.isNull() || a1.isNull())
      throw RuntimeError(EXC_RT_OTHER_S, "Invalid arguments.");
    return new bloc::Value(new bloc::Literal(u->Substr((size_t)*a0.integer(), (size_t)*a1.integer())));
  }

  case utf8::Tolower:
    u->Transform(utf8helper::TransformLower);
    return new bloc::Value(new bloc::Complex(object_this));

  case utf8::Toupper:
    u->Transform(utf8helper::TransformUpper);
    return new bloc::Value(new bloc::Complex(object_this));

  case utf8::Normaliz:
    u->Transform(utf8helper::TransformNormalize);
    return new bloc::Value(new bloc::Complex(object_this));

  case utf8::Capital:
    u->Transform(utf8helper::TransformCapitalize);
    return new bloc::Value(new bloc::Complex(object_this));

  case utf8::Translit:
    u->Transform(utf8helper::TransformTransliterate);
    return new bloc::Value(new bloc::Complex(object_this));

  }
  return nullptr;
}

} /* namespace import */
} /* namespace bloc */

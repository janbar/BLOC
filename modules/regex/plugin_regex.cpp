/*
 *      Copyright (C) 2024 Jean-Luc Barriere
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

#include "plugin_regex.h"
#include <blocc/collection.h>
#include <blocc/tuple.h>

#include <regex>
#include <iterator>

/*
 * Create the module REGEXPlugin
 */
PLUGINCREATOR(REGEXPlugin)

namespace bloc
{
namespace plugin
{
namespace regex
{

/*
 * The module interface
 */

/**********************************************************************/
/*  Constructors                                                      */
/**********************************************************************/
static PLUGIN_TYPE ctor_0_args[]  = {
  { "L", 0 }, // regular expression as string
};

static PLUGIN_TYPE ctor_1_args[]  = {
  { "O", 0 }, // regex
};

static PLUGIN_CTOR ctors[] =
{
  { 0,      1,  ctor_0_args,
          "Create a new regex for the given expression." },
  { 1,      1,  ctor_1_args,
          "Build a new regex as a copy of the given one." },
};

enum Method
{
  Search = 0, Replace,
};

/**********************************************************************/
/*  Method arguments                                                  */
/*  mode:         type: decl,ndim                                     */
/**********************************************************************/
static PLUGIN_ARG search_args[]  = {
  { PLUGIN_IN,    { "L", 0 } }, // string
};

static PLUGIN_ARG replace_args[] = {
  { PLUGIN_IN,    { "L", 0 } }, // source string
  { PLUGIN_IN,    { "L", 0 } }, // format
};

/**********************************************************************/
/*  Methods list                                                      */
/*  id:       name:         ret: decl,ndim  args_count,args:          */
/**********************************************************************/
static PLUGIN_METHOD methods[] =
{
  { Search,         "search",         { "IIL", 1 },     1, search_args,
          "Attempts to match the regular expression to any part of a string,\n"
          "and returns a table of tuple containing the matched sequences:\n"
          "{ Position, Length, Sequence that matched }" },
  { Replace,        "replace",        { "L", 0 },       2, replace_args,
          "Replaces occurrences of the regular expression with formatted\n"
          "replacement string.\n"
          "1: target string\n"
          "2: the regex replacement format string" },
};

/**
 * The state of file handle
 */
struct Handle {
  std::regex _regex;

  ~Handle() { }
  Handle() : _regex() { }
  Handle(const Handle& h) : _regex(h._regex) { }

  void search(const std::string& str, bloc::Collection& subs);
  void replace(const std::string& str, const std::string& format, bloc::Literal& out);
};

} /* namespace csv */

void REGEXPlugin::declareInterface(PLUGIN_INTERFACE * interface)
{
  interface->name = "regex";
  interface->method_count = sizeof(regex::methods) / sizeof(PLUGIN_METHOD);
  interface->methods = regex::methods;
  interface->ctors_count = sizeof(regex::ctors) / sizeof(PLUGIN_CTOR);
  interface->ctors = regex::ctors;
}

void * REGEXPlugin::createObject(int ctor_id, bloc::Context& ctx, const std::vector<bloc::Expression*>& args)
{
  regex::Handle * regx = nullptr;
  try
  {
    switch (ctor_id)
    {
    case 0: /* regex ( regular_expression ) */
    {
      bloc::Value& a0 = args[0]->value(ctx);
      regx = new regex::Handle();
      if (!a0.isNull() && !a0.literal()->empty())
      {
        try
        {
          std::regex rx(*a0.literal());
          regx->_regex.swap(rx);
        }
        catch (std::regex_error& re)
        {
          throw RuntimeError(EXC_RT_OTHER_S, re.what());
        }
      }
      break;
    }

    case 1: /* regex ( regex ), i.e copy constructor */
    {
      /* the complex handle to copy */
      regex::Handle * h = nullptr;
      bloc::Value& a0 = args[0]->value(ctx);
      if (a0.isNull() || (h = static_cast<regex::Handle*>(a0.complex()->instance())) == nullptr)
        throw RuntimeError(EXC_RT_OTHER_S, "Invalid arguments.");
      regx = new regex::Handle(*h);
      break;
    }

    default: /* default ctor */
      // using the nil expression
      regx = new regex::Handle();
      break;
    }
  }
  catch (...)
  {
    if (regx)
      delete regx;
    throw;
  }
  return regx;
}

void REGEXPlugin::destroyObject(void * object)
{
  regex::Handle * regx = static_cast<regex::Handle*>(object);
  delete regx;
}

bloc::Value * REGEXPlugin::executeMethod(
          bloc::Complex& object_this,
          int method_id,
          bloc::Context& ctx,
          const std::vector<bloc::Expression*>& args
          )
{
  regex::Handle * regx = static_cast<regex::Handle*>(object_this.instance());
  switch (method_id)
  {
  case regex::Search:
  {
    bloc::Value& a0 = args[0]->value(ctx);
    if (a0.isNull())
      return new bloc::Value(static_cast<bloc::Collection*>(nullptr));
    bloc::Collection * c = new bloc::Collection(bloc::Type(bloc::Type::ROWTYPE).levelUp());
    regx->search(*a0.literal(), *c);
    return new bloc::Value(c);
  }

  case regex::Replace:
  {
    bloc::Value& a0 = args[0]->value(ctx);
    bloc::Value& a1 = args[1]->value(ctx);
    if (a0.isNull() || a1.isNull())
      return new bloc::Value(bloc::Value::type_literal);
    bloc::Literal * r = new bloc::Literal();
    regx->replace(*a0.literal(), *a1.literal(), *r);
    return new bloc::Value(r);
  }

  default:
    break;
  }
  return nullptr;
}

void regex::Handle::search(const std::string& str, bloc::Collection& subs)
{
  std::smatch sub;
  std::regex_search(str, sub, _regex);
  for (int i = 0; i < sub.size(); ++i)
  {
    bloc::Tuple::container_t items;
    items.push_back(bloc::Value(bloc::Integer(std::distance(str.begin(), sub[i].first))));
    items.push_back(bloc::Value(bloc::Integer(std::distance(sub[i].first, sub[i].second))));
    items.push_back(bloc::Value(new bloc::Literal(sub[i].str())));
    subs.push_back(bloc::Value(new bloc::Tuple(std::move(items))));
  }
}

void regex::Handle::replace(const std::string& str, const std::string& format, std::string& out)
{
  std::regex_replace(std::back_inserter(out), str.begin(), str.end(), _regex, format);
}

} /* namespace import */
} /* namespace bloc */

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

#include "plugin_csv.h"
#include "csvparser.h"
#include <blocc/collection.h>
#include <blocc/tuple.h>

/*
 * Create the module CSVImport
 */
PLUGINCREATOR(CSVPlugin)

namespace bloc
{
namespace plugin
{
namespace csv
{

/*
 * The module interface
 */

/**********************************************************************/
/*  Constructors                                                      */
/**********************************************************************/
static PLUGIN_TYPE ctor_0_args[]  = {
  { "L", 0 }, // separator + encapsulator
};

static PLUGIN_TYPE ctor_1_args[]  = {
  { "I", 0 }, // separator
  { "I", 0 }, // encapsulator
};

static PLUGIN_CTOR ctors[] =
{
  { 0,      1,  ctor_0_args,
          "Create a new CSV parser for the given format.\n"
          "1  : String containing the character of the field seperator,\n"
          "     optionally followed by the character of the data encapsulator.\n"
          "     By default the encapsulator is quote."},
  { 1,      2,  ctor_1_args,
          "Create a new CSV parser for the given format.\n"
          "1  : Code point of the field seperator\n"
          "2  : Code point of the data encapsulator"},
};

enum Method
{
  Serialize0 = 0, SerializeB, SerializeN, SerializeL,
  Deserialize, Deserialize_next, In_error, Error_pos,
};

/**********************************************************************/
/*  Method arguments                                                  */
/*  mode:         type: decl,ndim                                     */
/**********************************************************************/
static PLUGIN_ARG serialize0_args[]  = {
  { PLUGIN_IN,    { "R", 0 } }, // tuple
};

static PLUGIN_ARG serializeL_args[]  = {
  { PLUGIN_IN,    { "L", 1 } }, // table of string
};

static PLUGIN_ARG serializeB_args[]  = {
  { PLUGIN_IN,    { "B", 1 } }, // table of boolean
};

static PLUGIN_ARG serializeN_args[]  = {
  { PLUGIN_IN,    { "N", 1 } }, // table of numeric
};

static PLUGIN_ARG deserialize_args[]  = {
  { PLUGIN_IN,    { "L", 0 } }, // a line
  { PLUGIN_INOUT, { "L", 1 } }, // table of fields
};

static PLUGIN_ARG deserialize_next_args[]  = {
  { PLUGIN_IN,    { "L", 0 } }, // the line to follow
  { PLUGIN_INOUT, { "L", 1 } }, // table of fields
};

/**********************************************************************/
/*  Methods list                                                      */
/*  id:       name:         ret: decl,ndim  args_count,args:          */
/**********************************************************************/
static PLUGIN_METHOD methods[] =
{
  { Serialize0,     "serialize",      { "L", 0 },     1, serialize0_args,
          "Serialize a tuple of data." },
  { SerializeL,     "serialize",      { "L", 0 },     1, serializeL_args,
          "Serialize a table of string." },
  { SerializeB,     "serialize",      { "L", 0 },     1, serializeB_args,
          "Serialize a table of boolean." },
  { SerializeN,     "serialize",      { "L", 0 },     1, serializeN_args,
          "Serialize a table of decimal." },
  { Deserialize,    "deserialize",    { "B", 0 },     2, deserialize_args,
          "Deserialize the first line of a record.\n"
  "It returns FALSE when the record is complete, otherwise TRUE when it needs\n"
  "more line. Next lines should be processed by calling deserialize_next()." },
  { Deserialize_next, "deserialize_next", { "B", 0 }, 2, deserialize_next_args,
          "Processing a new line of an incomplete record.\n"
  "It returns FALSE when the record is complete, otherwise TRUE when it still\n"
  "needs more line." },
  { In_error,       "in_error",       { "B", 0 },     0, nullptr,
          "Returns TRUE if an error occurred during the last deserialization." },
  { Error_pos,     "error_pos",       { "I", 0 },     0, nullptr,
          "Returns the position of the code point in error." },
};

/**
 * The state of file handle
 */
struct Handle {
  CSVParser _parser;

  ~Handle() { }
  Handle(char s, char e) : _parser(s, e) { }
  bool serialize(std::string& out, const CSVParser::container& data);
  bool deserialize(CSVParser::container& out, const std::string& line);
  bool deserialize_next(CSVParser::container& out, const std::string& line);
  bool in_error() const { return _parser.in_error(); }
  unsigned error_position() const { return _parser.error_position(); }
};

} /* namespace csv */

void CSVPlugin::declareInterface(PLUGIN_INTERFACE * interface)
{
  interface->name = "csv";
  interface->method_count = sizeof(csv::methods) / sizeof(PLUGIN_METHOD);
  interface->methods = csv::methods;
  interface->ctors_count = sizeof(csv::ctors) / sizeof(PLUGIN_CTOR);
  interface->ctors = csv::ctors;
}

void * CSVPlugin::createObject(int ctor_id, bloc::Context& ctx, const std::vector<bloc::Expression*>& args)
{
  csv::Handle * csv = nullptr;
  try
  {
    switch (ctor_id)
    {
    case 0: /* csv( separator + encapsulator ) */
    {
      bloc::Value& a0 = args[0]->value(ctx);
      if (a0.isNull() || a0.literal()->length() < 1)
        throw RuntimeError(EXC_RT_OTHER_S, "Specified format is invalid.");
      char s = a0.literal()->at(0);
      char e = '"';
      if (a0.literal()->length() > 1)
        e = a0.literal()->at(1);
      csv = new csv::Handle(s, e);
      break;
    }

    case 1: /* csv( separator , encapsulator ) */
    {
      bloc::Value& a0 = args[0]->value(ctx);
      bloc::Value& a1 = args[1]->value(ctx);
      if (a0.isNull() || a1.isNull())
        throw RuntimeError(EXC_RT_OTHER_S, "Invalid parameters.");
      char s = (char) *a0.integer();
      char e = (char) *a1.integer();
      csv = new csv::Handle(s, e);
      break;
    }

    default: /* default ctor */
      // using the standard format
      csv = new csv::Handle(',', '"');
      break;
    }
  }
  catch (...)
  {
    if (csv)
      delete csv;
    throw;
  }
  return csv;
}

void CSVPlugin::destroyObject(void * object)
{
  csv::Handle * csv = static_cast<csv::Handle*>(object);
  delete csv;
}

bloc::Value * CSVPlugin::executeMethod(
          bloc::Complex& object_this,
          int method_id,
          bloc::Context& ctx,
          const std::vector<bloc::Expression*>& args
          )
{
  csv::Handle * csv = static_cast<csv::Handle*>(object_this.instance());
  switch (method_id)
  {
  case csv::SerializeL:
  {
    bloc::Value& a0 = args[0]->value(ctx);
    if (a0.isNull())
      return new bloc::Value(static_cast<bloc::Literal*>(nullptr));
    bloc::Collection& c = *a0.collection();
    std::vector<std::string> data;
    for (bloc::Value& v : c)
    {
      if (v.isNull())
        data.push_back("");
      else
        data.push_back(*v.literal());
    }
    std::string * out = new std::string();
    csv->serialize(*out, data);
    return new bloc::Value(out);
  }

  case csv::SerializeB:
  {
    bloc::Value& a0 = args[0]->value(ctx);
    if (a0.isNull())
      return new bloc::Value(static_cast<bloc::Literal*>(nullptr));
    bloc::Collection& c = *a0.collection();
    std::vector<std::string> data;
    for (bloc::Value& v : c)
    {
      if (v.isNull())
        data.push_back("");
      else
        data.push_back(bloc::Value::readableBoolean(*v.boolean()));
    }
    std::string * out = new std::string();
    csv->serialize(*out, data);
    return new bloc::Value(out);
  }

  case csv::SerializeN:
  {
    bloc::Value& a0 = args[0]->value(ctx);
    if (a0.isNull())
      return new bloc::Value(static_cast<bloc::Literal*>(nullptr));
    bloc::Collection& c = *a0.collection();
    const bloc::Type& exp_type = c.table_type();
    std::vector<std::string> data;
    for (bloc::Value& v : c)
    {
      if (v.isNull())
        data.push_back("");
      else if (exp_type == bloc::Type::INTEGER)
        data.push_back(bloc::Value::readableInteger(*v.integer()));
      else if (exp_type == bloc::Type::NUMERIC)
        data.push_back(bloc::Value::readableNumeric(*v.numeric()));
      else
        throw RuntimeError(EXC_RT_NOT_NUMERIC);
    }
    std::string * out = new std::string();
    csv->serialize(*out, data);
    return new bloc::Value(out);
  }

  case csv::Serialize0:
  {
    bloc::Value& a0 = args[0]->value(ctx);
    if (a0.isNull())
      return new bloc::Value(static_cast<bloc::Literal*>(nullptr));
    bloc::Tuple& t = *a0.tuple();
    std::vector<std::string> data;
    for (bloc::Value& v : t)
    {
      if (v.isNull())
        data.push_back("");
      else
      {
        const bloc::Type& exp_type = v.type();
        switch (exp_type.major())
        {
        case bloc::Type::BOOLEAN:
          data.push_back(bloc::Value::readableBoolean(*v.boolean()));
          break;
        case bloc::Type::INTEGER:
          data.push_back(bloc::Value::readableInteger(*v.integer()));
          break;
        case bloc::Type::NUMERIC:
          data.push_back(bloc::Value::readableNumeric(*v.numeric()));
          break;
        case bloc::Type::LITERAL:
          data.push_back(*v.literal());
          break;
        case bloc::Type::IMAGINARY:
          data.push_back(bloc::Value::readableImaginary(*v.imaginary()));
          break;
        case bloc::Type::COMPLEX:
          data.push_back(bloc::Value::readableComplex(*v.complex()));
          break;
        case bloc::Type::TABCHAR:
          data.push_back(bloc::Value::readableTabchar(*v.tabchar()));
          break;
        default:
          throw RuntimeError(EXC_RT_NOT_IMPLEMENTED);
        }
      }
    }
    std::string * out = new std::string();
    csv->serialize(*out, data);
    return new bloc::Value(out);
  }

  case csv::Deserialize:
  {
    bloc::Value& a0 = args[0]->value(ctx);
    bloc::Value& a1 = args[1]->value(ctx);
    if (a0.isNull() || !args[1]->isVarName())
      throw RuntimeError(EXC_RT_OTHER_S, "Invalid arguments.");
    /* processing data */
    std::vector<std::string> data;
    bool next = csv->deserialize(data, *a0.literal());
    /* INOUT */
    bloc::Value v = bloc::Value(new bloc::Collection(bloc::Type(bloc::Type::LITERAL).levelUp()));
    bloc::Collection * c = v.collection();
    for (std::string& f : data)
      c->push_back(bloc::Value(new bloc::Literal(f)));
    ctx.storeVariable(*args[1]->symbol(), std::move(v));
    return new bloc::Value(bloc::Bool(next));
  }

  case csv::Deserialize_next:
  {
    bloc::Value& a0 = args[0]->value(ctx);
    bloc::Value& a1 = args[1]->value(ctx);
    if (a0.isNull() || !args[1]->isVarName() || a1.isNull())
      throw RuntimeError(EXC_RT_OTHER_S, "Invalid arguments.");
    bloc::Collection& c = *a1.collection();
    std::vector<std::string> data;
    if (c.begin() != c.end())
    {
      // copy last incomplete value
      size_t last = c.size() - 1;
      data.push_back(*(c.at(last).literal()));
      // pop it
      c.erase(c.begin() + last);
    }
    bool next = csv->deserialize_next(data, *a0.literal());
    for (std::string& f : data)
      c.push_back(bloc::Value(new bloc::Literal(f)));
    return new bloc::Value(bloc::Bool(next));
  }

  case csv::In_error:
    return new bloc::Value(bloc::Bool(csv->in_error()));

  case csv::Error_pos:
    return new bloc::Value(bloc::Integer(csv->error_position()));

  default:
    break;
  }
  return nullptr;
}

bool csv::Handle::serialize(std::string& out, const CSVParser::container& data)
{
  _parser.serialize(out, data);
  return true;
}

bool csv::Handle::deserialize(CSVParser::container& out, const std::string& line)
{
  return _parser.deserialize(out, line);
}

bool csv::Handle::deserialize_next(CSVParser::container& out, const std::string& line)
{
  return _parser.deserialize_next(out, line);
}

} /* namespace import */
} /* namespace bloc */

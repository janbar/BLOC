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

#include "import_date.h"
#include "builtin.h"
#include <blocc/debug.h>

#include <cstring>
#include <string>
#include <regex>
#include <cassert>

/*
 * Create the module DateImport
 */
IMPORTCREATOR(DateImport)

namespace bloc
{
namespace import
{
namespace date
{

static const char * time_units[] = { "second", "minute", "hour", "day", "month", "year" };

/*
 * The module interface
 */
enum Method
{
  Format = 0, Unixtime, Iso8601, Iso8601utc, Isodate, Difftime,
  Day, Month, Year, Hour, Minute, Second, Weekday, Yearday,
  Add, Add_unit, Trunc, Trunc_unit
};

/**********************************************************************/
/*  Constructors                                                      */
/**********************************************************************/
static IMPORT_TYPE ctor_0_args[]  = {
  { "L", 0 }, // string
  { "L", 0 }, // format
};

static IMPORT_TYPE ctor_1_args[]  = {
  { "C", 0 }, // date
};

static IMPORT_CTOR ctors[] =
{
  { 0,      2,  ctor_0_args },  /* new date( string, format ) */
  { 1,      1,  ctor_1_args },  /* new date( date ) */
};

/**********************************************************************/
/*  Method arguments                                                  */
/*  mode:         type: decl,ndim                                     */
/**********************************************************************/
static IMPORT_ARG format_args[]  = {
  { IMPORT_IN,    { "L", 0 } }, // format
};

static IMPORT_ARG diff_args[]  = {
  { IMPORT_IN,    { "C", 0 } }, // date0
};

static IMPORT_ARG num_args[]  = {
  { IMPORT_IN,    { "N", 0 } }, // decimal
};

static IMPORT_ARG add_args[]  = {
  { IMPORT_IN,    { "I", 0 } }, // integer
  { IMPORT_IN,    { "L", 0 } }, // format
};

static IMPORT_ARG trunc_args[]  = {
  { IMPORT_IN,    { "L", 0 } }, // format
};

/**********************************************************************/
/*  Methods list                                                      */
/*  id:       name:         ret: decl,ndim  args_count,args:          */
/**********************************************************************/
static IMPORT_METHOD methods[] =
{
  { Format,     "format",       { "L", 0 },     1, format_args, },
  { Unixtime,   "unixtime",     { "I", 0 },     0, nullptr, },
  { Iso8601,    "iso8601",      { "L", 0 },     0, nullptr, },
  { Iso8601utc, "iso8601utc",   { "L", 0 },     0, nullptr, },
  { Iso8601utc, "iso8601utc",   { "L", 0 },     0, nullptr, },
  { Isodate,    "isodate",      { "L", 0 },     0, nullptr, },
  { Difftime,   "difftime",     { "N", 0 },     1, diff_args, },
  { Add,        "add",          { "C", 0 },     1, num_args, },
  { Add_unit,   "add",          { "C", 0 },     2, add_args, },
  { Second,     "second",       { "I", 0 },     0, nullptr, },
  { Minute,     "minute",       { "I", 0 },     0, nullptr, },
  { Hour,       "hour",         { "I", 0 },     0, nullptr, },
  { Day,        "day",          { "I", 0 },     0, nullptr, },
  { Month,      "month",        { "I", 0 },     0, nullptr, },
  { Year,       "year",         { "I", 0 },     0, nullptr, },
  { Weekday,    "weekday",      { "I", 0 },     0, nullptr, },
  { Yearday,    "yearday",      { "I", 0 },     0, nullptr, },
  { Trunc,      "trunc",        { "C", 0 },     0, nullptr, },
  { Trunc_unit, "trunc",        { "C", 0 },     1, trunc_args, },
};

/**
 * The state of date handle
 */
struct Handle {
  struct tm _tm;

  ~Handle() = default;
  Handle() { memset(&_tm, '\0', sizeof(tm)); }
  void now()
  {
    memset(&_tm, '\0', sizeof(tm));
    _tm.tm_isdst = -1;
    time_t ts = time(nullptr);
    localtime_r(&ts, &_tm);
  }
  time_t unixtime() { return mktime(&_tm); }
};

} /* namespace date */

void DateImport::declareInterface(IMPORT_INTERFACE * interface)
{
  interface->name = "date";
  interface->method_count = sizeof(date::methods) / sizeof(IMPORT_METHOD);
  interface->methods = date::methods;
  interface->ctors_count = sizeof(date::ctors) / sizeof(IMPORT_CTOR);
  interface->ctors = date::ctors;
}

void * DateImport::createObject(int ctor_id, bloc::Context& ctx, const std::vector<bloc::Expression*>& args)
{
  date::Handle * dd = new date::Handle();
  try
  {
    switch(ctor_id)
    {
    case 0: /* date ( string, format ) */
    {
      struct tm _tm;
      memset(&_tm, '\0', sizeof(tm));
      _tm.tm_isdst = -1;
      if (strptime(args[0]->literal(ctx).c_str(), args[1]->literal(ctx).c_str(), &_tm) == nullptr)
        throw RuntimeError(EXC_RT_MESSAGE_S, "Invalid date format.");
      /* string is local time (no tz info) */
      time_t tt = mktime(&_tm);
      if (tt == INVALID_TIME)
        throw RuntimeError(EXC_RT_MESSAGE_S, "The system does not support this date range.");
      localtime_r(&tt, &dd->_tm);
      break;
    }

    case 1: /* date ( date ), i.e copy constructor */
    {
      /* complex handle MUST be stored until the end of processing */
      bloc::Complex c0 = args[0]->complex(ctx);
      *dd = *(static_cast<date::Handle*>(c0.instance()));
      break;
    }

    default:  /* default ctor */
      dd->now();
    }
  }
  catch (...)
  {
    delete dd;
    throw;
  }
  return dd;
}

void DateImport::destroyObject(void * object)
{
  date::Handle * dd = static_cast<date::Handle*>(object);
  delete dd;
}

bloc::Expression * DateImport::executeMethod(
          bloc::Complex object_this,
          int method_id,
          bloc::Context& ctx,
          const std::vector<bloc::Expression*>& args
          )
{
  date::Handle * dd = static_cast<date::Handle*>(object_this.instance());
  switch (method_id)
  {
  case date::Format:
  {
     char buf[64];
     size_t n = strftime(buf, 64, args[0]->literal(ctx).c_str(), &dd->_tm);
     return new bloc::LiteralExpression(std::string(buf, n));
  }

  case date::Unixtime:
  {
    return new IntegerExpression((int64_t) dd->unixtime());
  }

  case date::Iso8601:
  {
     char buf[64];
     size_t n = strftime(buf, 64, "%Y-%m-%dT%H:%M:%S", &dd->_tm);
     return new bloc::LiteralExpression(std::string(buf, n));
  }

  case date::Iso8601utc:
  {
     char buf[64];
     struct tm _tm;
     time_t tt = mktime(&dd->_tm);
     gmtime_r(&tt, &_tm);
     size_t n = strftime(buf, 64, "%Y-%m-%dT%H:%M:%SZ", &_tm);
     return new bloc::LiteralExpression(std::string(buf, n));
  }

  case date::Isodate:
  {
     char buf[64];
     size_t n = strftime(buf, 64, "%Y-%m-%d", &dd->_tm);
     return new bloc::LiteralExpression(std::string(buf, n));
  }

  case date::Difftime:
  {
    /* complex handle MUST be stored until the end of processing */
    bloc::Complex c0 = args[0]->complex(ctx);
    date::Handle * dd0 = static_cast<date::Handle*>(c0.instance());
    if (dd0 == nullptr)
      throw RuntimeError(EXC_RT_MEMB_ARG_TYPE_S, date::methods[date::Difftime].name);
    return new bloc::NumericExpression(difftime(dd->unixtime(), dd0->unixtime()));
  }

  case date::Add:
  {
    double add = args[0]->numeric(ctx);
    if (add > 86400.0 || add < -86400.0)
      throw RuntimeError(EXC_RT_OUT_OF_RANGE);
    int day = (int) add;
    int sec = (int) (86400.0 * (add - day));
    struct tm _tm = dd->_tm;
    _tm.tm_mday += day;
    _tm.tm_sec += sec;
    _tm.tm_isdst = -1; /* reset dst */
    time_t tt = mktime(&_tm);
    if (tt == INVALID_TIME)
      throw RuntimeError(EXC_RT_MESSAGE_S, "The system does not support this date range.");
    dd->_tm = _tm;
    return new ComplexExpression(object_this);
  }

  case date::Add_unit:
  {
    int64_t add = args[0]->integer(ctx);
    std::string& in = args[1]->literal(ctx);
    int unit = -1;
    for (int i = 0; i < (sizeof(date::time_units) / sizeof(const char*)); ++i)
    {
      if (in != date::time_units[i])
        continue;
      unit = i;
    }
    if (unit < 0)
      throw RuntimeError(EXC_RT_MESSAGE_S,"Invalid name for time unit.");

    struct tm _tm = dd->_tm;
    if (unit == 0 && add <= 86400 && add >= -86400)
      _tm.tm_sec += add;
    else if (unit == 1 && add <= 86400 && add >= -86400)
      _tm.tm_min += add;
    else if (unit == 2 && add <= 86400 && add >= -86400)
      _tm.tm_hour += add;
    else if (unit == 3 && add <= 86400 && add >= -86400)
      _tm.tm_mday += add;
    else if (unit == 4 && add <= 1440 && add >= -1440)
      _tm.tm_mon += add;
    else if (unit == 5 && add <= 1440 && add >= -1440)
      _tm.tm_year += add;
    else
      throw RuntimeError(EXC_RT_OUT_OF_RANGE);
    _tm.tm_isdst = -1; /* reset dst */

    time_t tt = mktime(&_tm);
    if (tt == INVALID_TIME)
      throw RuntimeError(EXC_RT_MESSAGE_S, "The system does not support this date range.");
    dd->_tm = _tm;
    return new ComplexExpression(object_this);
  }

  case date::Trunc:
  {
    /* trunc day */
    dd->_tm.tm_isdst = -1; /* reset dst */
    dd->_tm.tm_sec = 0;
    dd->_tm.tm_min = 0;
    dd->_tm.tm_hour = 0;
    mktime(&dd->_tm);
    return new ComplexExpression(object_this);
  }

  case date::Trunc_unit:
  {
    std::string& in = args[0]->literal(ctx);
    int unit = -1;
    for (int i = 0; i < (sizeof(date::time_units) / sizeof(const char*)); ++i)
    {
      if (in != date::time_units[i])
        continue;
      unit = i;
    }
    if (unit < 0)
      throw RuntimeError(EXC_RT_MESSAGE_S,"Invalid name for time unit.");

    if (unit > 0)
      dd->_tm.tm_sec = 0;
    if (unit > 1)
      dd->_tm.tm_min = 0;
    if (unit > 2)
    {
      dd->_tm.tm_hour = 0;
      dd->_tm.tm_isdst = -1; /* reset dst */
    }
    if (unit > 3)
      dd->_tm.tm_mday = 1; /* first day of the month */
    if (unit > 4)
      dd->_tm.tm_mon = 0; /* first month of the yeay */

    mktime(&dd->_tm);
    return new ComplexExpression(object_this);
  }

  case date::Second:
    return new IntegerExpression(dd->_tm.tm_sec);
  case date::Minute:
    return new IntegerExpression(dd->_tm.tm_min);
  case date::Hour:
    return new IntegerExpression(dd->_tm.tm_hour);
  case date::Day:
    return new IntegerExpression(dd->_tm.tm_mday);
  case date::Month:
    return new IntegerExpression(dd->_tm.tm_mon + 1);
  case date::Year:
    return new IntegerExpression(dd->_tm.tm_year + 1900);
  case date::Weekday:
    /* the int value follows the ISO-8601 standard, from 1 (monday) to 7 (sunday) */
    return new IntegerExpression((dd->_tm.tm_wday == 0 ? 7 : dd->_tm.tm_wday));
  case date::Yearday:
    /* [ 1 - 366 ] */
    return new IntegerExpression(dd->_tm.tm_yday + 1);

  }
  return nullptr;
}

} /* namespace import */
} /* namespace bloc */

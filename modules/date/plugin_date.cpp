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

#include "plugin_date.h"
#include "builtin.h"
#include <blocc/debug.h>

#include <cstring>
#include <string>
#include <regex>
#include <cassert>

/*
 * Create the module DateImport
 */
PLUGINCREATOR(DatePlugin)

namespace bloc
{
namespace plugin
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
static PLUGIN_TYPE ctor_0_args[]  = {
  { "C", 0 }, // date
};

static PLUGIN_TYPE ctor_1_args[]  = {
  { "L", 0 }, // string
  { "L", 0 }, // format
};

static PLUGIN_TYPE ctor_2_args[]  = {
  { "L", 0 }, // string formatted as ISO8601
};

static PLUGIN_CTOR ctors[] =
{
  { 0,      1,  ctor_0_args,
          "Build a new calendar as a copy of the given one." },
  { 1,      2,  ctor_1_args,
          "Build a new calendar from the given string and format." },
  { 2,      1,  ctor_2_args,
          "Build a new calendar from the given ISO date or time string." },
};

/**********************************************************************/
/*  Method arguments                                                  */
/*  mode:         type: decl,ndim                                     */
/**********************************************************************/
static PLUGIN_ARG format_args[]  = {
  { PLUGIN_IN,    { "L", 0 } }, // format
};

static PLUGIN_ARG diff_args[]  = {
  { PLUGIN_IN,    { "C", 0 } }, // date0
};

static PLUGIN_ARG num_args[]  = {
  { PLUGIN_IN,    { "N", 0 } }, // decimal
};

static PLUGIN_ARG add_args[]  = {
  { PLUGIN_IN,    { "I", 0 } }, // integer
  { PLUGIN_IN,    { "L", 0 } }, // format
};

static PLUGIN_ARG trunc_args[]  = {
  { PLUGIN_IN,    { "L", 0 } }, // format
};

/**********************************************************************/
/*  Methods list                                                      */
/*  id:       name:         ret: decl,ndim  args_count,args:          */
/**********************************************************************/
static PLUGIN_METHOD methods[] =
{
  { Format,     "format",       { "L", 0 },     1, format_args,
          "Formats calendar time according to the directives in the given format string." },
  { Unixtime,   "unixtime",     { "I", 0 },     0, nullptr,
          "Converts calendar time to time since epoch." },
  { Iso8601,    "iso8601",      { "L", 0 },     0, nullptr,
          "Formats calendar time according to ISO8601 format string." },
  { Iso8601utc, "iso8601utc",   { "L", 0 },     0, nullptr,
          "Formats calendar time UTC according to ISO8601 format string." },
  { Isodate,    "isodate",      { "L", 0 },     0, nullptr,
          "Formats calendar according to ISO date format string." },
  { Difftime,   "difftime",     { "N", 0 },     1, diff_args,
          "Computes the difference between times in seconds." },
  { Add,        "add",          { "C", 0 },     1, num_args,
          "Add or remove day(s) to the calendar." },
  { Add_unit,   "add",          { "C", 0 },     2, add_args,
          "Add or remove unit(s) to the calendar."
          "\nUnit could by among second, minute, hour, day, month, or year." },
  { Second,     "second",       { "I", 0 },     0, nullptr,
          "Returns second of calendar." },
  { Minute,     "minute",       { "I", 0 },     0, nullptr,
          "Returns minute of calendar." },
  { Hour,       "hour",         { "I", 0 },     0, nullptr,
          "Returns hour of calendar." },
  { Day,        "day",          { "I", 0 },     0, nullptr,
          "Returns day of calendar." },
  { Month,      "month",        { "I", 0 },     0, nullptr,
          "Returns month of calendar." },
  { Year,       "year",         { "I", 0 },     0, nullptr,
          "Returns year of calendar." },
  { Weekday,    "weekday",      { "I", 0 },     0, nullptr,
          "Returns week day of calendar. Monday=1 ... Sunday=7." },
  { Yearday,    "yearday",      { "I", 0 },     0, nullptr,
          "Returns year day of calendar." },
  { Trunc,      "trunc",        { "C", 0 },     0, nullptr,
          "Truncate time to the calendar." },
  { Trunc_unit, "trunc",        { "C", 0 },     1, trunc_args,
          "Truncate the calendar to a specific unit."
          "\nUnit could by among minute, hour, day, month, or year." },
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

static const char * ISO8601UTC = "%Y-%m-%dT%H:%M:%SZ";
static unsigned ISO8601UTC_LEN = 20;
static const char * ISO8601LOC = "%Y-%m-%dT%H:%M:%S";
static unsigned ISO8601LOC_LEN = 19;
static const char * ISODATE = "%Y-%m-%d";
static unsigned ISODATE_LEN = 10;

} /* namespace date */

void DatePlugin::declareInterface(PLUGIN_INTERFACE * interface)
{
  interface->name = "date";
  interface->method_count = sizeof(date::methods) / sizeof(PLUGIN_METHOD);
  interface->methods = date::methods;
  interface->ctors_count = sizeof(date::ctors) / sizeof(PLUGIN_CTOR);
  interface->ctors = date::ctors;
}

void * DatePlugin::createObject(int ctor_id, bloc::Context& ctx, const std::vector<bloc::Expression*>& args)
{
  date::Handle * dd = new date::Handle();
  try
  {
    switch(ctor_id)
    {
    case 0: /* date ( date ), i.e copy constructor */
    {
      /* the complex handle to copy */
      bloc::Value& a0 = args[0]->value(ctx);
      if (a0.isNull())
        throw RuntimeError(EXC_RT_OTHER_S, "Invalid arguments.");
      *dd = *(static_cast<date::Handle*>(a0.complex()->instance()));
      break;
    }

    case 1: /* date ( string, format ) */
    {
      struct tm _tm;
      memset(&_tm, '\0', sizeof(tm));
      _tm.tm_isdst = -1;
      bloc::Value& a0 = args[0]->value(ctx);
      bloc::Value& a1 = args[1]->value(ctx);
      if (a0.isNull() || a1.isNull())
        throw RuntimeError(EXC_RT_OTHER_S, "Invalid arguments.");
      if (strptime(args[0]->value(ctx).literal()->c_str(), args[1]->value(ctx).literal()->c_str(), &_tm) == nullptr)
        throw RuntimeError(EXC_RT_OTHER_S, "Invalid date format.");
      /* string is local time (no tz info) */
      time_t tt = mktime(&_tm);
      if (tt == INVALID_TIME)
        throw RuntimeError(EXC_RT_OTHER_S, "The system does not support this date range.");
      localtime_r(&tt, &dd->_tm);
      break;
    }

    case 2: /* date ( string iso ) */
    {
      struct tm _tm;
      memset(&_tm, '\0', sizeof(tm));
      _tm.tm_isdst = -1;
      bloc::Value& a0 = args[0]->value(ctx);
      if (a0.isNull())
        throw RuntimeError(EXC_RT_OTHER_S, "Invalid arguments.");
      bloc::Literal * str = a0.literal();
      const char * format;
      if (str->size() == date::ISO8601UTC_LEN)
        format = date::ISO8601UTC;
      else if (str->size() == date::ISO8601LOC_LEN)
        format = date::ISO8601LOC;
      else if (str->size() == date::ISODATE_LEN)
        format = date::ISODATE;
      else
        throw RuntimeError(EXC_RT_OTHER_S, "Invalid date format.");
      if (strptime(str->c_str(), format, &_tm) == nullptr)
        throw RuntimeError(EXC_RT_OTHER_S, "Invalid date format.");
      time_t tt;
      if (format == date::ISO8601UTC)
        tt = timegm(&_tm); /* utc time */
      else
        tt = mktime(&_tm); /* local time */
      if (tt == INVALID_TIME)
        throw RuntimeError(EXC_RT_OTHER_S, "The system does not support this date range.");
      localtime_r(&tt, &dd->_tm);
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

void DatePlugin::destroyObject(void * object)
{
  date::Handle * dd = static_cast<date::Handle*>(object);
  delete dd;
}

bloc::Value * DatePlugin::executeMethod(
          bloc::Complex& object_this,
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
     bloc::Value& a0 = args[0]->value(ctx);
     if (a0.isNull())
       throw RuntimeError(EXC_RT_OTHER_S, "Invalid arguments.");
     size_t n = strftime(buf, 64, a0.literal()->c_str(), &dd->_tm);
     return new bloc::Value(new bloc::Literal(buf, n));
  }

  case date::Unixtime:
  {
    return new Value(Integer(dd->unixtime()));
  }

  case date::Iso8601:
  {
     char buf[64];
     size_t n = strftime(buf, 64, "%Y-%m-%dT%H:%M:%S", &dd->_tm);
     return new bloc::Value(new bloc::Literal(buf, n));
  }

  case date::Iso8601utc:
  {
     char buf[64];
     struct tm _tm;
     time_t tt = mktime(&dd->_tm);
     gmtime_r(&tt, &_tm);
     size_t n = strftime(buf, 64, "%Y-%m-%dT%H:%M:%SZ", &_tm);
     return new bloc::Value(new bloc::Literal(buf, n));
  }

  case date::Isodate:
  {
     char buf[64];
     size_t n = strftime(buf, 64, "%Y-%m-%d", &dd->_tm);
     return new bloc::Value(new bloc::Literal(buf, n));
  }

  case date::Difftime:
  {
    bloc::Value& a0 = args[0]->value(ctx);
    if (a0.isNull())
      throw RuntimeError(EXC_RT_OTHER_S, "Invalid arguments.");
    date::Handle * dd0 = static_cast<date::Handle*>(a0.complex()->instance());
    if (dd0 == nullptr)
      throw RuntimeError(EXC_RT_MEMB_ARG_TYPE_S, date::methods[date::Difftime].name);
    return new bloc::Value(bloc::Numeric(difftime(dd->unixtime(), dd0->unixtime())));
  }

  case date::Add:
  {
    bloc::Value& a0 = args[0]->value(ctx);
    if (a0.isNull())
      throw RuntimeError(EXC_RT_OTHER_S, "Invalid arguments.");
    double add = *a0.numeric();
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
      throw RuntimeError(EXC_RT_OTHER_S, "The system does not support this date range.");
    dd->_tm = _tm;
    return new bloc::Value(new bloc::Complex(object_this));
  }

  case date::Add_unit:
  {
    bloc::Value& a0 = args[0]->value(ctx);
    bloc::Value& a1 = args[1]->value(ctx);
    if (a0.isNull() || a1.isNull())
      throw RuntimeError(EXC_RT_OTHER_S, "Invalid arguments.");
    int64_t add = *a0.integer();
    std::string * in = a1.literal();
    int unit = -1;
    for (int i = 0; i < (sizeof(date::time_units) / sizeof(const char*)); ++i)
    {
      if (*in != date::time_units[i])
        continue;
      unit = i;
    }
    if (unit < 0)
      throw RuntimeError(EXC_RT_OTHER_S,"Invalid name for time unit.");

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
      throw RuntimeError(EXC_RT_OTHER_S, "The system does not support this date range.");
    dd->_tm = _tm;
    return new bloc::Value(new bloc::Complex(object_this));
  }

  case date::Trunc:
  {
    /* trunc day */
    dd->_tm.tm_isdst = -1; /* reset dst */
    dd->_tm.tm_sec = 0;
    dd->_tm.tm_min = 0;
    dd->_tm.tm_hour = 0;
    mktime(&dd->_tm);
    return new bloc::Value(new bloc::Complex(object_this));
  }

  case date::Trunc_unit:
  {
    bloc::Value& a0 = args[0]->value(ctx);
    if (a0.isNull())
      throw RuntimeError(EXC_RT_OTHER_S, "Invalid arguments.");
    std::string * in = a0.literal();
    int unit = -1;
    for (int i = 0; i < (sizeof(date::time_units) / sizeof(const char*)); ++i)
    {
      if (*in != date::time_units[i])
        continue;
      unit = i;
    }
    if (unit < 0)
      throw RuntimeError(EXC_RT_OTHER_S,"Invalid name for time unit.");

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
    return new bloc::Value(new bloc::Complex(object_this));
  }

  case date::Second:
    return new bloc::Value(Integer(dd->_tm.tm_sec));
  case date::Minute:
    return new bloc::Value(Integer(dd->_tm.tm_min));
  case date::Hour:
    return new bloc::Value(Integer(dd->_tm.tm_hour));
  case date::Day:
    return new bloc::Value(Integer(dd->_tm.tm_mday));
  case date::Month:
    return new bloc::Value(Integer(dd->_tm.tm_mon + 1));
  case date::Year:
    return new bloc::Value(Integer(dd->_tm.tm_year + 1900));
  case date::Weekday:
    /* the int value follows the ISO-8601 standard, from 1 (monday) to 7 (sunday) */
    return new bloc::Value(Integer((dd->_tm.tm_wday == 0 ? 7 : dd->_tm.tm_wday)));
  case date::Yearday:
    /* [ 1 - 366 ] */
    return new bloc::Value(Integer(dd->_tm.tm_yday + 1));

  }
  return nullptr;
}

} /* namespace import */
} /* namespace bloc */

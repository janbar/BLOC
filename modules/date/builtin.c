/*
 *      Copyright (C) 2007-2012 Free Software Foundation, Inc.
 *      Copyright (C) 2014-2022 Jean-Luc Barriere
 *
 *  This Program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2, or (at your option)
 *  any later version.
 *
 *  This Program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; see the file COPYING.  If not, write to
 *  the Free Software Foundation, 51 Franklin Street, Fifth Floor, Boston,
 *  MA 02110-1301 USA
 *  http://www.gnu.org/copyleft/gpl.html
 *
 */

#include "builtin.h"
#include "langinfo.h"

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <stddef.h>
#include <string.h>

#ifdef _MSC_VER
#define strncasecmp _strnicmp
#define strcasecmp _stricmp
#endif

time_t __timegm(struct tm *utctime_tm)
{
  time_t time;
  struct tm adj_tm, chk_tm;

  adj_tm.tm_year = utctime_tm->tm_year;
  adj_tm.tm_mon = utctime_tm->tm_mon;
  adj_tm.tm_mday = utctime_tm->tm_mday;
  adj_tm.tm_hour = utctime_tm->tm_hour;
  adj_tm.tm_min = utctime_tm->tm_min;
  adj_tm.tm_sec = utctime_tm->tm_sec;
  adj_tm.tm_isdst = -1;

  for (;;)
  {
    time = mktime(&adj_tm);
    if (time == INVALID_TIME)
      return time;
    if (NULL == gmtime_r(&time, &chk_tm))
      return INVALID_TIME;
    if (chk_tm.tm_min == utctime_tm->tm_min &&
            chk_tm.tm_hour == utctime_tm->tm_hour &&
            chk_tm.tm_mday == utctime_tm->tm_mday &&
            chk_tm.tm_mon == utctime_tm->tm_mon &&
            chk_tm.tm_year == utctime_tm->tm_year)
      break;
    adj_tm.tm_min += utctime_tm->tm_min - chk_tm.tm_min;
    adj_tm.tm_hour += utctime_tm->tm_hour - chk_tm.tm_hour;
    adj_tm.tm_mday += utctime_tm->tm_mday - chk_tm.tm_mday;
    adj_tm.tm_mon += utctime_tm->tm_mon - chk_tm.tm_mon;
    adj_tm.tm_year += utctime_tm->tm_year - chk_tm.tm_year;
  }
  return time;
}

char *__strptime(const char *s, const char *f, struct tm *tm)
{
  int i, w, neg, adj, min, range, *dest, dummy;
  const char *ex;
  size_t len;
  int want_century = 0, century = 0, relyear = 0;
  while (*f) {
    if (*f != '%') {
      if (isspace(*f)) for (; *s && isspace(*s); s++);
      else if (*s != *f) return 0;
      else s++;
      f++;
      continue;
    }
    f++;
    if (*f == '+') f++;
    if (isdigit(*f)) {
      char *new_f;
      w=strtoul(f, &new_f, 10);
      f = new_f;
    } else {
      w=-1;
    }
    adj=0;
    switch (*f++) {
    case 'a': case 'A':
      dest = &tm->tm_wday;
      min = ABDAY_1;
      range = 7;
      goto symbolic_range;
    case 'b': case 'B': case 'h':
      dest = &tm->tm_mon;
      min = ABMON_1;
      range = 12;
      goto symbolic_range;
    case 'c':
      s = __strptime(s, nl_langinfo(D_T_FMT), tm);
      if (!s) return 0;
      break;
    case 'C':
      dest = &century;
      if (w<0) w=2;
      want_century |= 2;
      goto numeric_digits;
    case 'd': case 'e':
      dest = &tm->tm_mday;
      min = 1;
      range = 31;
      goto numeric_range;
    case 'D':
      s = __strptime(s, "%m/%d/%y", tm);
      if (!s) return 0;
      break;
    case 'H':
      dest = &tm->tm_hour;
      min = 0;
      range = 24;
      goto numeric_range;
    case 'I':
      dest = &tm->tm_hour;
      min = 1;
      range = 12;
      goto numeric_range;
    case 'j':
      dest = &tm->tm_yday;
      min = 1;
      range = 366;
      adj = 1;
      goto numeric_range;
    case 'm':
      dest = &tm->tm_mon;
      min = 1;
      range = 12;
      adj = 1;
      goto numeric_range;
    case 'M':
      dest = &tm->tm_min;
      min = 0;
      range = 60;
      goto numeric_range;
    case 'n': case 't':
      for (; *s && isspace(*s); s++);
      break;
    case 'p':
      ex = nl_langinfo(AM_STR);
      len = strlen(ex);
      if (!strncasecmp(s, ex, len)) {
        tm->tm_hour %= 12;
        s += len;
        break;
      }
      ex = nl_langinfo(PM_STR);
      len = strlen(ex);
      if (!strncasecmp(s, ex, len)) {
        tm->tm_hour %= 12;
        tm->tm_hour += 12;
        s += len;
        break;
      }
      return 0;
    case 'r':
      s = __strptime(s, nl_langinfo(T_FMT_AMPM), tm);
      if (!s) return 0;
      break;
    case 'R':
      s = __strptime(s, "%H:%M", tm);
      if (!s) return 0;
      break;
    case 'S':
      dest = &tm->tm_sec;
      min = 0;
      range = 61;
      goto numeric_range;
    case 'T':
      s = __strptime(s, "%H:%M:%S", tm);
      if (!s) return 0;
      break;
    case 'U':
    case 'W':
      /* Throw away result, for now. (FIXME?) */
      dest = &dummy;
      min = 0;
      range = 54;
      goto numeric_range;
    case 'w':
      dest = &tm->tm_wday;
      min = 0;
      range = 7;
      goto numeric_range;
    case 'x':
      s = __strptime(s, nl_langinfo(D_FMT), tm);
      if (!s) return 0;
      break;
    case 'X':
      s = __strptime(s, nl_langinfo(T_FMT), tm);
      if (!s) return 0;
      break;
    case 'y':
      dest = &relyear;
      w = 2;
      want_century |= 1;
      goto numeric_digits;
    case 'Y':
      dest = &tm->tm_year;
      if (w<0) w=4;
      adj = 1900;
      want_century = 0;
      goto numeric_digits;
    case '%':
      if (*s++ != '%') return 0;
      break;
    default:
      return 0;
    numeric_range:
      if (!isdigit(*s)) return 0;
      *dest = 0;
      for (i=1; i<=min+range && isdigit(*s); i*=10)
        *dest = *dest * 10 + *s++ - '0';
      if (*dest - min >= (unsigned)range) return 0;
      *dest -= adj;
      goto update;
    numeric_digits:
      neg = 0;
      if (*s == '+') s++;
      else if (*s == '-') neg=1, s++;
      if (!isdigit(*s)) return 0;
      for (*dest=i=0; i<w && isdigit(*s); i++)
        *dest = *dest * 10 + *s++ - '0';
      if (neg) *dest = -*dest;
      *dest -= adj;
      goto update;
    symbolic_range:
      for (i=2*range-1; i>=0; i--) {
        ex = nl_langinfo(min+i);
        len = strlen(ex);
        if (strncasecmp(s, ex, len)) continue;
        s += len;
        *dest = i % range;
        break;
      }
      if (i<0) return 0;
      goto update;
    update:
      //FIXME
      ;
    }
  }
  if (want_century) {
    tm->tm_year = relyear;
    if (want_century & 2) tm->tm_year += century * 100 - 1900;
    else if (tm->tm_year <= 68) tm->tm_year += 100;
  }
  return (char *)s;
}

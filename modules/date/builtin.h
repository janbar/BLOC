/*
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

#ifndef BUILTIN_H
#define	BUILTIN_H

#if defined __cplusplus
extern "C" {
#endif

#include <time.h>
#include <string.h>

#ifndef CC_INLINE
#define CC_INLINE
#endif

#define INVALID_TIME ((time_t)(-1))

#if !defined(HAVE_TIMEGM) && !defined(timegm)
#define timegm __timegm
extern time_t __timegm(struct tm *utctime_tm);
#endif

#if !defined(HAVE_LOCALTIME_R) && !defined(localtime_r)
#define localtime_r __localtime_r
static CC_INLINE struct tm *__localtime_r(const time_t *clock, struct tm *result)
{
  struct tm *data;
  if (!clock || !result)
    return NULL;
  data = localtime(clock);
  if (!data)
    return NULL;
  memcpy(result, data, sizeof(*result));
  return result;
}
#endif

#if !defined(HAVE_GMTIME_R) && !defined(gmtime_r)
#define gmtime_r __gmtime_r
static CC_INLINE struct tm *__gmtime_r(const time_t *clock, struct tm *result)
{
  struct tm *data;
  if (!clock || !result)
    return NULL;
  data = gmtime(clock);
  if (!data)
    return NULL;
  memcpy(result, data, sizeof(*result));
  return result;
}
#endif

#ifdef strptime
#undef strptime
#endif
#define strptime __strptime
char *__strptime(const char *s, const char *f, struct tm *tm);

#ifdef __cplusplus
}
#endif

#endif	/* BUILTIN_H */

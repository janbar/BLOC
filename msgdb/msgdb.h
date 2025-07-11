/*
 *      Copyright (C) 2025 Jean-Luc Barriere
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

#ifndef MSGDB_H
#define MSGDB_H

#ifdef __cplusplus
extern "C"
{
#endif

#if (defined(_WIN32) || defined(_WIN64) || defined(_M_ARM64))
#define LIBMSGDB_DLL_EXPORT __declspec(dllexport)
#define LIBMSGDB_DLL_IMPORT __declspec(dllimport)
#define LIBMSGDB_DLL_LOCAL
#else
#if __GNUC__ >= 4
#define LIBMSGDB_DLL_EXPORT __attribute__ ((visibility ("default")))
#define LIBMSGDB_DLL_IMPORT __attribute__ ((visibility ("default")))
#define LIBMSGDB_DLL_LOCAL __attribute__ ((visibility ("hidden")))
#else
#define LIBMSGDB_DLL_EXPORT
#define LIBMSGDB_DLL_IMPORT
#define LIBMSGDB_DLL_LOCAL
#endif
#endif

#ifdef LIB_DLL_EXPORTS /* defined if we are building the dll (instead of using it) */
#define LIBMSGDB_API LIBMSGDB_DLL_EXPORT
#else
#define LIBMSGDB_API LIBMSGDB_DLL_IMPORT
#endif

#include "msgdb_common.h"

LIBMSGDB_API const char *
msgdb_setup(const char * lang);

LIBMSGDB_API const MSGDB_KEYWORD_ROW *
msgdb_get_text(const char * section, const char * keyword);

LIBMSGDB_API const MSGDB_SECTION_ROW *
msgdb_get_section(const char * section);

LIBMSGDB_API const MSGDB_LANG_ROW *
msgdb_get_lang(const char * lang);

#ifdef __cplusplus
}
#endif

#endif /* MSGDB_H */


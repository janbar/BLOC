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

#ifndef MSGDB_COMMON_H
#define MSGDB_COMMON_H

typedef struct {
  const char              *keyword;
  unsigned                text_len;
  const unsigned char     *text;
} MSGDB_KEYWORD_ROW;

typedef struct {
  const char              *section;
  const MSGDB_KEYWORD_ROW  *keyword_table;
} MSGDB_SECTION_ROW;

typedef struct {
  const char              *lang;
  const MSGDB_SECTION_ROW  *section_table;
} MSGDB_LANG_ROW;

#endif /* MSGDB_COMMON_H */


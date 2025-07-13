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

#include "msgdb.h"
#include "msgdb_lang.h"

#include <stdlib.h>
#include <string.h>

static char msgdb_default_lang[4] = { 'e' , 'n', 0x0, 0x0 };
static char msgdb_current_lang[4] = { 0x0 , 0x0, 0x0, 0x0 };

LIBMSGDB_API const char *
msgdb_setup(const char * lang)
{
  const MSGDB_LANG_ROW * rl = msgdb_get_lang(lang);
  if (rl != NULL)
    strncpy(msgdb_current_lang, rl->lang, sizeof(msgdb_current_lang));
  else
    strncpy(msgdb_current_lang, msgdb_default_lang, sizeof(msgdb_current_lang));
  if (lang && strcmp(lang, msgdb_current_lang) == 0)
    return lang;
  return msgdb_current_lang;
}

const MSGDB_KEYWORD_ROW *
msgdb_get_text(const char * section, const char * keyword)
{
  const MSGDB_SECTION_ROW * rs = msgdb_get_section(section);
  if (keyword && rs)
  {
    const MSGDB_KEYWORD_ROW * rk = rs->keyword_table;
    while (rk && rk->keyword)
    {
      if (strcmp(keyword, rk->keyword) == 0)
        return rk;
      rk += 1;
    }
  }
  return NULL;
}

const MSGDB_SECTION_ROW *
msgdb_get_section(const char * section)
{
  const MSGDB_LANG_ROW * rl = msgdb_get_lang(msgdb_current_lang);
  if (section && rl)
  {
    const MSGDB_SECTION_ROW * rs = rl->section_table;
    while (rs && rs->section)
    {
      if (strcmp(section, rs->section) == 0)
        return rs;
      rs += 1;
    }
  }
  return NULL;
}

const MSGDB_LANG_ROW *
msgdb_get_lang(const char * lang)
{
  const MSGDB_LANG_ROW * rl = msgdb_lang;
  const MSGDB_LANG_ROW * def = NULL;
  while (rl && rl->lang)
  {
    if (lang && strcmp(lang, rl->lang) == 0)
      return rl;
    if (strcmp(msgdb_default_lang, rl->lang) == 0)
      def = rl;
    rl += 1;
  }
  return def;
}

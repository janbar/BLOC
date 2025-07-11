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

#ifndef CLI_MSGDB_H
#define CLI_MSGDB_H

#include <string>

class MsgDB
{
  struct Interface;
  Interface * _msgdb = nullptr;

public:
  typedef struct {
    const char              *keyword;
    unsigned                text_len;
    const unsigned char     *text;
  } msgdb_keyword;

  typedef struct {
    const char              *section;
    const msgdb_keyword      *keyword_table;
  } msgdb_section;

  typedef struct {
    const char              *lang;
    const msgdb_section      *section_table;
  } msgdb_lang;

  MsgDB() { }
  ~MsgDB();
  bool initialize();
  void setup(const std::string& lang);
  const msgdb_keyword * getText(const std::string& section, const std::string& keyword);
  const msgdb_section * getSection(const std::string& section);
  const msgdb_lang * getLang(const std::string& lang);
};

#endif /* CLI_MSGDB_H */


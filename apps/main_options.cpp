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

#include "main_options.h"

#include <cstring>
#include <string>
#include <vector>

bool cmdOption(const char * str, const std::string& option, std::string * value)
{
  if (::strncmp(str, option.c_str(), option.length()) == 0)
  {
    if (value != nullptr && str[option.length()] == '=')
      value->assign(&str[option.length() + 1]);
    return true;
  }
  return false;
}

const char * getCmd(char **begin, char **end, MainOptions& options, std::vector<std::string>& prog)
{
  prog.clear();
  bool cmd = false;
  for (char** it = begin; it != end; ++it)
  {
    if (cmd || **it != '-' || ::strlen(*it) == 1)
    {
      cmd = true;
      prog.push_back(*it);
    }
    else
    {
      if (cmdOption(*it, "--debug", &options.dbg_hints))
        options.debug = true;
      else if (cmdOption(*it, "--parse", nullptr))
        options.parse = true;
      else if (cmdOption(*it, "--cli", nullptr) || cmdOption(*it, "-i", nullptr))
        options.docli = true;
      else if (cmdOption(*it, "--color", nullptr))
        options.color = true;
      else if (cmdOption(*it, "--expr", nullptr) || cmdOption(*it, "-e", nullptr))
        options.doexp = true;
      else if (cmdOption(*it, "--out", &options.file_sout))
        continue;
      else
        return *it;
    }
  }
  return nullptr;
}

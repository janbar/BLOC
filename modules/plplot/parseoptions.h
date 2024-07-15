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

#ifndef PARSEOPTIONS_H
#define PARSEOPTIONS_H

#include <string>
#include <vector>

#define parseoptions __parseoptions
inline void __parseoptions(const std::string& cmd, std::vector<std::pair<std::string, std::string> >& options)
{
  std::string::const_iterator it = cmd.cbegin();
  bool opt = false;
  char enc = 0;
  while (it != cmd.end())
  {
    if (!opt)
    {
      if (*it == '-')
      {
        options.push_back(std::make_pair("", ""));
        while (it != cmd.end() && *it > 0x20)
        {
          options.back().first.push_back(*it);
          ++it;
        }
        opt = true;
      }
    }
    else if (enc)
    {
      if (*it == enc)
        enc = 0;
      else
        options.back().second.push_back(*it);
    }
    else if (*it == '-')
    {
      opt = false;
      continue;
    }
    else if (*it > 0x20)
    {
      if (*it == 0x22 || *it == 0x27)
        enc = *it;
      else
        options.back().second.push_back(*it);
    }
    ++it;
  }
}

#endif /* PARSEOPTIONS_H */


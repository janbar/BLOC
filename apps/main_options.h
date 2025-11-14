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

#ifndef MAIN_OPTIONS_H
#define MAIN_OPTIONS_H

#include <string>
#include <vector>
#include <blocc/value.h>

struct MainOptions
{
  bool debug = false;                   /* enable debug messages */
  bool parse = false;                   /* only parse, the program won't be run */
  bool docli = false;                   /* run CLI, arguments to follow will be loaded into the context */
  bool color = false;                   /* enable colored output */
  bool doexp = false;                   /* execute the expression to follow */
  std::string dbg_hints;                /* debug hints */
  std::string file_sout;                /* forward output stream */
};

extern bool cmdOption(const char * str, const std::string& option, std::string * value);

extern const char * getCmd(char **begin, char **end, MainOptions& options, std::vector<std::string>& prog);

#endif /* MAIN_OPTIONS_H */


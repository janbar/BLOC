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

#ifndef READ_FILE_H
#define READ_FILE_H

#include <blocc/parser.h>

class ReadFile : public bloc::Parser::StreamReader
{
  FILE * _file;
public:
  explicit ReadFile(FILE * file) : _file(file) { }
  int read(bloc::Parser *, char * buf, int max_size) override;
};

#endif /* READ_FILE_H */


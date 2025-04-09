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

#ifndef STRING_READER_H_
#define STRING_READER_H_

#include "parser.h"

#include <string>

namespace bloc
{

/**
 * Helper class that provides stream reader from string
 */
class StringReader : public Parser::StreamReader
{
  std::string _text;
  size_t _pos = 0;

public:
  virtual ~StringReader() = default;
  StringReader() { }
  explicit StringReader(const std::string& text) : _text(text) { }

  StringReader& reset(const std::string& text) { _text.assign(text); _pos = 0; return *this; }
  StringReader& append(const std::string& text) { _text.append(text); return *this; }

  int read(Parser *, char * buf, int max_size) override;
};

}

#endif /* STRING_READER_H_ */


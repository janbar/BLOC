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

#ifndef EXCEPTION_H_
#define EXCEPTION_H_

#include <stdexcept>
#include <string>

namespace bloc
{

class Error : public std::exception
{
  private:
    const char * _message;
    std::string _arg;

public:
    explicit Error(const char * what) : _message(what)  { }
    Error(const char * what, const char * arg) : _message(what), _arg(arg) { }

    const char * what() const noexcept override
    {
      static char buf[256];
      snprintf(buf, sizeof(buf), _message, _arg.c_str());
      return buf;
    }
};

}

#endif /* EXCEPTION_H_ */


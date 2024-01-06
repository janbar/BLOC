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

#ifndef STACK_H_
#define STACK_H_

#include <vector>

namespace bloc
{

template<class T> class Stack {

public:

  Stack() {}
  virtual ~Stack() {}

  void reserve(unsigned sz) { _stack.reserve(sz); }

  bool empty() const { return (_stack.empty()); }

  size_t size() const { return _stack.size(); }

  void stack(const T& s)
  {
    this->_stack.push_back(s);
  }

  T top() const
  {
    return _stack.back();
  }

  void unstack()
  {
    if (!empty())
      _stack.pop_back();
  }

private:

  std::vector<T> _stack;
};

}

#endif /* STACK_H_ */

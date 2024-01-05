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

#ifndef TUPLE_DECL_H_
#define TUPLE_DECL_H_

#include "intrinsic_type.h"
#include "declspec.h"

#include <vector>

namespace bloc
{

class TupleDecl
{
public:

  struct Decl : public std::vector<Type>
  {
    virtual ~Decl() = default;
    Decl() : std::vector<Type>() { }
    Decl(size_t n, const Type& t) : std::vector<Type>(n, t) { }
    std::string tupleName() const;
    Type make_type(Type::TypeLevel level) const;
  };

  LIBBLOC_API static const Decl no_decl;

  virtual ~TupleDecl() = default;
  virtual const Decl& tuple_decl() const = 0;
};

}

#endif /* TUPLE_DECL_H_ */


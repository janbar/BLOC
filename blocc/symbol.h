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

#ifndef SYMBOL_H_
#define SYMBOL_H_

#include "intrinsic_type.h"
#include "tuple_decl.h"

namespace bloc
{

class Symbol : public Type, public TupleDecl
{
public:
  const unsigned id;
  const std::string name;

  constexpr static char SAFETY_QUALIFIER = '$';

  Symbol(unsigned id, const std::string& name, const Type& type)
  : Type(type), id(id), name(name) { }

  Symbol(unsigned id, const std::string& name, const Decl& decl, Type::TypeLevel level)
  : Type(decl.make_type(level)), id(id), name(name), _decl(decl) { }

  Symbol(const Symbol&) = default;

  void safety(bool b) const { _safety = b; }
  bool safety() const { return _safety; }

  void upgrade(const Type& to_type);

  void upgrade(const Decl& to_decl, Type::TypeLevel level);

  const Decl& tuple_decl() const override { return _decl; }

private:
  Decl _decl;
  mutable bool _safety = false;
};

}

#endif /* SYMBOL_H_ */

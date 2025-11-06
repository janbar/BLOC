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
  unsigned _id;
  std::string _name;

public:

  constexpr static char SAFETY_QUALIFIER = '$';

  Symbol(unsigned id, const std::string& name, const Type& type)
  : Type(type), _id(id), _name(name) { }

  Symbol(unsigned id, const std::string& name, const Decl& decl, Type::TypeLevel level)
  : Type(decl.make_type(level)), _id(id), _name(name), _decl(decl) { }

  Symbol(const Symbol&) = default;
  Symbol& operator=(const Symbol&) = default;

  Symbol(Symbol&& s) noexcept
  : Type(s._major, s._minor, s._level)
  , _decl(std::move(s._decl))
  , _id(s._id)
  , _name(std::move(s._name))
  , _safety(s._safety)
  , _locked(s._locked)
  { }

  Symbol& operator=(Symbol&& s) noexcept
  {
    if (this == &s)
      return *this;
    Type::_major = s._major;
    Type::_minor = s._minor;
    Type::_level = s._level;
    _decl = std::move(s._decl);
    _id = s._id;
    _name = std::move(s._name);
    _safety = s._safety;
    _locked = s._locked;
    return *this;
  }

  unsigned id() const { return _id; }

  const std::string& name() const { return _name; }

  /**
   * Enable immutable type constraint.
   * When this constraint is enabled, the major type cannot be modified.
   * @param b true to enable else false
   */
  void safety(bool b) const { _safety = b; }

  /**
   * Enable constness constraint.
   * When this constraint is enabled, neither the payload nor the type
   * can be modified.
   * @param b true to enable else false
   */
  void locked(bool b) const { _locked = b; }

  /**
   * Test type is immutable.
   * Constness also reinforces the constraint.
   * @return true or false
   */
  bool safety() const { return _safety || _locked; }

  /**
   * Test constness constraint.
   * @return true or false
   */
  bool locked() const { return _locked; }

  void upgrade(const Type& to_type);

  void upgrade(const Decl& to_decl, Type::TypeLevel level);

  const Decl& tuple_decl() const override { return _decl; }

  /**
   * The safety check result returned by check_safety().
   */
  enum SafetyCheck {
    SAFE_KO = 0, /* not safe */
    SAFE_EQU,    /* safer    */
    SAFE_UPG,    /* feasible */
  };
  /**
   * Try to valid an opaque or qualified type against this symbol in safe mode,
   * and during parsing stage.
   */
  SafetyCheck check_safety(const Type& type) const noexcept;

private:
  Decl _decl;
  mutable bool _safety = false;
  mutable bool _locked = false;
};

}

#endif /* SYMBOL_H_ */

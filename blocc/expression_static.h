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

#ifndef EXPRESSION_STATIC_H
#define EXPRESSION_STATIC_H

#include "expression.h"

namespace bloc
{

/**
 * This class implements the expression that wrap a value with a fixed type,
 * and so it could be stored in memory. Access to wrapped value is permitted
 * to improve the performance of internal operations store/load.
 */
class StaticExpression : public Expression
{
protected:
  bool _safety = false;

public:

  virtual ~StaticExpression() { }

  StaticExpression() : Expression() { }

  void safety(bool b) { _safety = b; }
  bool safety() { return _safety;
  }

  /**
   * Returns the reference to the expression type. The type can be stored in
   * member variable or defined as static constant, i.e:
   *
   * static const Type& _name_ = Type(...)
   */
  virtual const Type& refType() const = 0;

  /**
   * The following functions provide access to reference of wrapped value.
   * Use should be limited to internal operation.
   */
  virtual bool& refBoolean()
  {
    throw RuntimeError(EXC_RT_NOT_BOOLEAN);
  }

  virtual int64_t& refInteger()
  {
    throw RuntimeError(EXC_RT_NOT_INTEGER);
  }

  virtual double& refNumeric()
  {
    throw RuntimeError(EXC_RT_NOT_NUMERIC);
  }

  virtual std::string& refLiteral()
  {
    throw RuntimeError(EXC_RT_NOT_LITERAL);
  }

  virtual TabChar& refTabchar()
  {
    throw RuntimeError(EXC_RT_NOT_TABCHAR);
  }

  virtual Collection& refCollection()
  {
    throw RuntimeError(EXC_RT_NOT_COLLECT);
  }

  virtual Tuple& refTuple()
  {
    throw RuntimeError(EXC_RT_NOT_ROWTYPE);
  }

  virtual Complex& refComplex()
  {
    throw RuntimeError(EXC_RT_NOT_COMPLEX);
  }

  virtual void swap(StaticExpression& e) noexcept(false) = 0;
  virtual StaticExpression * swapNew() = 0;
  virtual StaticExpression * cloneNew() const = 0;
};

}

#endif /* EXPRESSION_STATIC_H */

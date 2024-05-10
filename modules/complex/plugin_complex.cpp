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

#include "plugin_complex.h"
#include <blocc/debug.h>

#include <cmath>
#include <cstring>
#include <string>
#include <cassert>

/*
 * Create the module ComplexPlugin
 */
PLUGINCREATOR(ComplexPlugin)

namespace bloc
{
namespace plugin
{
namespace complex
{

/*
 * The module interface
 */
enum Method
{
  Re = 0, Im, Abs, Modulus, Phase, Add, Add_re, Sub, Sub_re, Mul, Mul_re,
  Div, Div_re, Conjugate, Inv, Reciprocal, Exp, Sqrt, Sin, Cos, Tan, IsZero,
  Vector
};

/**********************************************************************/
/*  Constructors                                                      */
/**********************************************************************/
static PLUGIN_TYPE ctor_0_args[]  = {
  { "N", 0 }, // from real
};

static PLUGIN_TYPE ctor_1_args[]  = {
  { "C", 0 }, // copy constructor
};

static PLUGIN_TYPE ctor_2_args[]  = {
  { "N", 0 }, // re
  { "N", 0 }, // im
};

static PLUGIN_CTOR ctors[] =
{
  { 0,      1,  ctor_0_args,
          "Build a new complex from the given real value."
          "\nPassing null, it provides the complex zero."},
  { 1,      1,  ctor_1_args,
          "Build a new complex as a copy of the given one." },
  { 2,      2,  ctor_2_args,
          "Build a new complex from the given real and imaginary values." },
};

/**********************************************************************/
/*  Method arguments                                                  */
/*  mode:         type: decl,ndim                                     */
/**********************************************************************/
static PLUGIN_ARG c_args[]  = {
  { PLUGIN_IN,    { "C", 0 } }, // complex
};

static PLUGIN_ARG n_args[]  = {
  { PLUGIN_IN,    { "N", 0 } }, // real
};

static PLUGIN_ARG n2_args[]  = {
  { PLUGIN_IN,    { "N", 0 } }, // real
  { PLUGIN_IN,    { "N", 0 } }, // real
};

/**********************************************************************/
/*  Methods list                                                      */
/*  id:       name:         ret: decl,ndim  args_count,args:          */
/**********************************************************************/
static PLUGIN_METHOD methods[] =
{
  { Re,         "re",         { "N", 0 },     0, nullptr,
          "The real value of the complex." },
  { Im,         "im",         { "N", 0 },     0, nullptr,
          "The imaginary value of the complex." },
  { Abs,        "abs",        { "N", 0 },     0, nullptr,
          "The modulus value of the complex." },
  { Modulus,    "modulus",    { "N", 0 },     0, nullptr,
          "The modulus value of the complex." },
  { Phase,      "phase",      { "N", 0 },     0, nullptr,
          "The phase of the complex." },
  { Add,        "add",        { "C", 0 },     1, c_args,
          "Compute addition of complex." },
  { Add_re,     "add",        { "C", 0 },     1, n_args,
          "Compute addition of real" },
  { Sub,        "sub",        { "C", 0 },     1, c_args,
          "Compute substraction of complex." },
  { Sub_re,     "sub",        { "C", 0 },     1, n_args,
          "Compute substraction of real" },
  { Mul,        "mul",        { "C", 0 },     1, c_args,
          "Compute factor by complex." },
  { Mul_re,     "mul",        { "C", 0 },     1, n_args,
          "Compute factor by real." },
  { Div,        "div",        { "C", 0 },     1, c_args,
          "Compute division by complex." },
  { Div_re,     "div",        { "C", 0 },     1, n_args,
          "Compute division by real." },
  { Conjugate,  "conjugate",  { "C", 0 },     0, nullptr,
          "Compute the Conjugate (a-bi)." },
  { Inv,        "inv",        { "C", 0 },     0, nullptr,
          "Compute the reciprocal." },
  { Reciprocal, "reciprocal", { "C", 0 },     0, nullptr,
          "Compute the reciprocal." },
  { Exp,        "exp",        { "C", 0 },     0, nullptr,
          "Compute exponent." },
  { Sqrt,       "sqrt",       { "C", 0 },     0, nullptr,
          "Compute square root." },
  { Sin,        "sin",        { "C", 0 },     0, nullptr,
          "Compute sinus." },
  { Cos,        "cos",        { "C", 0 },     0, nullptr,
          "Compute cosinus." },
  { Tan,        "tan",        { "C", 0 },     0, nullptr,
          "Compute tangent." },
  { IsZero,     "iszero",     { "B", 0 },     0, nullptr,
          "Returns true if the complex is zero." },
  { Vector,     "vector",     { "C", 0 },     2, n2_args,
          "Initialize the complex from the given vector (magnitude, phase)." },
};

/**
 * The state of complex handle
 */
struct Handle {
  double _re;
  double _im;

  ~Handle() = default;
  Handle() : _re(0), _im(0) { }
  Handle(double re, double im) : _re(re), _im(im) { }

  double modulus()
  {
    return sqrt(pow(_re, 2) + pow(_im, 2));
  }
  double phase()
  {
    return atan2(_im, _re);
  }
  Handle& add(const Handle& h)
  {
    _re += h._re;
    _im += h._im;
    return *this;
  }
  Handle& add(double re)
  {
    _re += re;
    return *this;
  }
  Handle& sub(const Handle& h)
  {
    _re -= h._re;
    _im -= h._im;
    return *this;
  }
  Handle& sub(double re)
  {
    _re -= re;
   return *this;
  }
  Handle& mul(const Handle& h)
  {
    *this = Handle(_re * h._re - _im * h._im , _re * h._im + _im * h._re);
    return *this;
  }
  Handle& mul(double re)
  {
    *this = Handle(_re * re , _im * re);
    return *this;
  }
  Handle& div(const Handle& h)
  {
    if (h._im == _im && h._re == _re)
    {
      _re = 1.0;
      _im = 0.0;
    }
    else if (h._im == (-_im) && h._re == (-_re))
    {
      _re = -1.0;
      _im = 0.0;
    }
    else
    {
      Handle h1 = h;
      mul(h1.reciprocal());
    }
    return *this;
  }
  Handle& div(double re)
  {
    *this = Handle(_re / re , _im / re);
    return *this;
  }
  Handle& conjugate()
  {
    _im = (-_im);
    return *this;
  }
  Handle& reciprocal()
  {
    double sc = pow(_re, 2.0) + pow(_im, 2.0);
    *this = Handle(_re / sc , (-_im) / sc);
    return *this;
  }
  Handle& exponent()
  {
    *this = Handle(exp(_re) * cos(_im) , exp(_re) * sin(_im));
    return *this;
  }
  Handle& square_root()
  {
    double r = sqrt(modulus());
    double t = atan2(_im, _re) / 2;
    *this = Handle(r * cos(t) , r * sin(t));
    return *this;
  }
  Handle& sinus()
  {
    *this = Handle(sin(_re) * cosh(_im) , cos(_re) * sinh(_im));
    return *this;
  }
  Handle& cosinus()
  {
    *this = Handle(cos(_re) * cosh(_im) , sin(_re) * sinh(_im));
    return *this;
  }
  Handle& tangent()
  {
    Handle h1 = *this;
    sinus().div(h1.cosinus());
    return *this;
  }
  bool isZero()
  {
    return (_re == 0 && _im == 0);
  }
  Handle& vector(double magnitude, double phase)
  {
    _re = magnitude * cos(phase);
    _im = magnitude * sin(phase);
    return *this;
  }
};

} /* namespace complex */

void ComplexPlugin::declareInterface(PLUGIN_INTERFACE * interface)
{
  interface->name = "complex";
  interface->method_count = sizeof(complex::methods) / sizeof(PLUGIN_METHOD);
  interface->methods = complex::methods;
  interface->ctors_count = sizeof(complex::ctors) / sizeof(PLUGIN_CTOR);
  interface->ctors = complex::ctors;
}

void * ComplexPlugin::createObject(int ctor_id, bloc::Context& ctx, const std::vector<bloc::Expression*>& args)
{
  complex::Handle * c = new complex::Handle();
  try
  {
    switch(ctor_id)
    {
    case 0:
    {
      bloc::Value& a0 = args[0]->value(ctx);
      if (!a0.isNull())
      {
        if (a0.type() == bloc::Type::INTEGER)
          c->_re = *a0.integer();
        else
          c->_re = *a0.numeric();
      }
      break;
    }

    case 1:
    {
      /* the complex handle to copy */
      complex::Handle * h = nullptr;
      bloc::Value& a0 = args[0]->value(ctx);
      if (a0.isNull() || (h = static_cast<complex::Handle*>(a0.complex()->instance())) == nullptr)
        throw RuntimeError(EXC_RT_OTHER_S, "Invalid arguments.");
      *c = *h;
      break;
    }

    case 2:
    {
      bloc::Value& a0 = args[0]->value(ctx);
      bloc::Value& a1 = args[1]->value(ctx);
      if (a0.isNull() || a1.isNull())
        throw RuntimeError(EXC_RT_OTHER_S, "Invalid arguments.");
      if (a0.type() == bloc::Type::INTEGER)
        c->_re = *a0.integer();
      else
        c->_re = *a0.numeric();
      if (a1.type() == bloc::Type::INTEGER)
        c->_im = *a1.integer();
      else
        c->_im = *a1.numeric();
      break;
    }

    default:  /* default ctor */
      break;
    }
  }
  catch (...)
  {
    delete c;
    throw;
  }
  return c;
}

void ComplexPlugin::destroyObject(void * object)
{
  complex::Handle * c = static_cast<complex::Handle*>(object);
  delete c;
}

bloc::Value * ComplexPlugin::executeMethod(
          bloc::Complex& object_this,
          int method_id,
          bloc::Context& ctx,
          const std::vector<bloc::Expression*>& args
          )
{
  complex::Handle * c = static_cast<complex::Handle*>(object_this.instance());
  switch (method_id)
  {

  case complex::Re:
  {
    return new bloc::Value(bloc::Numeric(c->_re));
  }
  case complex::Im:
  {
    return new bloc::Value(bloc::Numeric(c->_im));
  }
  case complex::Abs:
  case complex::Modulus:
  {
    return new bloc::Value(bloc::Numeric(c->modulus()));
  }
  case complex::Phase:
  {
    return new bloc::Value(bloc::Numeric(c->phase()));
  }
  case complex::Add:
  {
     bloc::Value& a0 = args[0]->value(ctx);
     if (a0.isNull())
       throw RuntimeError(EXC_RT_OTHER_S, "Invalid arguments.");
    complex::Handle * c0 = static_cast<complex::Handle*>(a0.complex()->instance());
    if (c0 == nullptr)
      throw RuntimeError(EXC_RT_MEMB_ARG_TYPE_S, complex::methods[complex::Add].name);
    c->add(*c0);
    return new bloc::Value(new bloc::Complex(object_this));
  }
  case complex::Add_re:
  {
    bloc::Value& a0 = args[0]->value(ctx);
    if (a0.isNull())
      throw RuntimeError(EXC_RT_OTHER_S, "Invalid arguments.");
    if (a0.type() == bloc::Type::INTEGER)
      c->add(*a0.integer());
    else
      c->add(*a0.numeric());
    return new bloc::Value(new bloc::Complex(object_this));
  }
  case complex::Sub:
  {
     bloc::Value& a0 = args[0]->value(ctx);
     if (a0.isNull())
       throw RuntimeError(EXC_RT_OTHER_S, "Invalid arguments.");
    complex::Handle * c0 = static_cast<complex::Handle*>(a0.complex()->instance());
    if (c0 == nullptr)
      throw RuntimeError(EXC_RT_MEMB_ARG_TYPE_S, complex::methods[complex::Sub].name);
    c->sub(*c0);
    return new bloc::Value(new bloc::Complex(object_this));
  }
  case complex::Sub_re:
  {
     bloc::Value& a0 = args[0]->value(ctx);
     if (a0.isNull())
       throw RuntimeError(EXC_RT_OTHER_S, "Invalid arguments.");
    if (a0.type() == bloc::Type::INTEGER)
      c->sub(*a0.integer());
    else
      c->sub(*a0.numeric());
    return new bloc::Value(new bloc::Complex(object_this));
  }
  case complex::Mul:
  {
     bloc::Value& a0 = args[0]->value(ctx);
     if (a0.isNull())
       throw RuntimeError(EXC_RT_OTHER_S, "Invalid arguments.");
    complex::Handle * c0 = static_cast<complex::Handle*>(a0.complex()->instance());
    if (c0 == nullptr)
      throw RuntimeError(EXC_RT_MEMB_ARG_TYPE_S, complex::methods[complex::Mul].name);
    c->mul(*c0);
    return new bloc::Value(new bloc::Complex(object_this));
  }
  case complex::Mul_re:
  {
     bloc::Value& a0 = args[0]->value(ctx);
     if (a0.isNull())
       throw RuntimeError(EXC_RT_OTHER_S, "Invalid arguments.");
    if (a0.type() == bloc::Type::INTEGER)
      c->mul(*a0.integer());
    else
      c->mul(*a0.numeric());
    return new bloc::Value(new bloc::Complex(object_this));
  }
  case complex::Div:
  {
     bloc::Value& a0 = args[0]->value(ctx);
     if (a0.isNull())
       throw RuntimeError(EXC_RT_OTHER_S, "Invalid arguments.");
    complex::Handle * c0 = static_cast<complex::Handle*>(a0.complex()->instance());
    if (c0 == nullptr)
      throw RuntimeError(EXC_RT_MEMB_ARG_TYPE_S, complex::methods[complex::Div].name);
    c->div(*c0);
    return new bloc::Value(new bloc::Complex(object_this));
  }
  case complex::Div_re:
  {
     bloc::Value& a0 = args[0]->value(ctx);
     if (a0.isNull())
       throw RuntimeError(EXC_RT_OTHER_S, "Invalid arguments.");
    if (a0.type() == bloc::Type::INTEGER)
      c->div(*a0.integer());
    else
      c->div(*a0.numeric());
    return new bloc::Value(new bloc::Complex(object_this));
  }
  case complex::Conjugate:
  {
    c->conjugate();
    return new bloc::Value(new bloc::Complex(object_this));
  }
  case complex::Inv:
  case complex::Reciprocal:
  {
    c->reciprocal();
    return new bloc::Value(new bloc::Complex(object_this));
  }
  case complex::Exp:
  {
    c->exponent();
    return new bloc::Value(new bloc::Complex(object_this));
  }
  case complex::Sqrt:
  {
    c->square_root();
    return new bloc::Value(new bloc::Complex(object_this));
  }
  case complex::Sin:
  {
    c->sinus();
    return new bloc::Value(new bloc::Complex(object_this));
  }
  case complex::Cos:
  {
    c->cosinus();
    return new bloc::Value(new bloc::Complex(object_this));
  }
  case complex::Tan:
  {
    c->tangent();
    return new bloc::Value(new bloc::Complex(object_this));
  }
  case complex::IsZero:
  {
    return new bloc::Value(bloc::Bool(c->isZero()));
  }
  case complex::Vector:
  {
    bloc::Value& a0 = args[0]->value(ctx);
    bloc::Value& a1 = args[1]->value(ctx);
    if (a0.isNull() || a1.isNull())
      throw RuntimeError(EXC_RT_OTHER_S, "Invalid arguments.");
    double magnitude = 0.0;
    double phase = 0.0;
    if (a0.type() == bloc::Type::INTEGER)
      magnitude = *a0.integer();
    else
      magnitude = *a0.numeric();
    if (a1.type() == bloc::Type::INTEGER)
      phase = *a1.integer();
    else
      phase = *a1.numeric();
    c->vector(magnitude, phase);
    return new bloc::Value(new bloc::Complex(object_this));
  }
  }
  return nullptr;
}

} /* namespace import */
} /* namespace bloc */

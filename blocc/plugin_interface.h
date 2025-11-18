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

#ifndef PLUGIN_INTERFACE_H_
#define PLUGIN_INTERFACE_H_

#ifdef __cplusplus
extern "C"
{
#endif

#define PLUGIN_VERSION  251109

  typedef void* PLUGIN_HANDLE;

  /**
   * The string declares the scalar type (one letter) or a tuple as a
   * sequence of letters that represent the types of each element.
   * Note that R declares an opaque tuple and cannot be used in a tuple
   * declaration.
   *
   *    B   -->   Boolean
   *    I   -->   Integer
   *    N   -->   Numeric
   *    C   -->   Imaginary number
   *    L   -->   Literal (C-String)
   *    O   -->   Object (self or of the same type)
   *    X   -->   Bytes array
   *    R   -->   Opaque tuple
   *
   * Examples: 'I' or 'R' or 'ILLNBB'
   */
  typedef const char * PLUGIN_DECL;

  /**
   * The modes of a parameter, indicating whether the parameter only
   * transmits data or whether it also returns data.
   */
  typedef enum
  {
    PLUGIN_IN    = 1,     /* read only */
    PLUGIN_INOUT = 2,     /* read, write no copy */
  } PLUGIN_ARG_MODE;

  /**
   * The value type, including the definition (decl) and the number of
   * dimensions (ndim): 0 for a single value, or 1 or more for a table.
   */
  typedef struct
  {
    PLUGIN_DECL     decl; /* scalar type or tuple declaration */
    unsigned char   ndim; /* 0 for single, or the table dimension */
  } PLUGIN_TYPE;

  /**
   * The description of CTOR.
   * A CTOR declares a list of input type
   */
  typedef struct
  {
    int id;
    unsigned args_count;  /* argument count */
    PLUGIN_TYPE * args;   /* list of argument type */
    const char * brief;   /* documentation relating to the CTOR */
  } PLUGIN_CTOR;

  /**
   * The declaration of method parameter.
   * A method parameter has a mode (IN or INOUT) and a type.
   */
  typedef struct
  {
    PLUGIN_ARG_MODE mode; /* argument mode: IN/OUT */
    PLUGIN_TYPE     type; /* argument type */
  } PLUGIN_ARG;

  /**
   * The description of method.
   * A method has a name, declares a type returned, and a list
   * of parameter.
   */
  typedef struct
  {
    int id;
    const char * name;    /* name of the method */
    PLUGIN_TYPE ret;      /* returned type */
    unsigned args_count;  /* arguments list count */
    PLUGIN_ARG * args;    /* arguments list */
    const char * brief;   /* documentation relating to the method */
  } PLUGIN_METHOD;

  /**
   * The object interface definition.
   * The object name, its methods and constructors.
   */
  typedef struct
  {
    const char * name;            /* name this object */
    unsigned method_count;        /* methods list count */
    PLUGIN_METHOD * methods;      /* methods list */
    unsigned ctors_count;         /* ctor list count */
    PLUGIN_CTOR * ctors;          /* ctor list */
  } PLUGIN_INTERFACE;

  typedef PLUGIN_HANDLE (*DLSYM_CREATE)();
  typedef int (*DLSYM_VERSION)();

#ifdef __cplusplus
}
#endif

#endif /* PLUGIN_INTERFACE_H_ */

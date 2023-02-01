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

#define PLUGIN_VERSION  230131

  typedef void* PLUGIN_HANDLE;

  /**
   * The string declares the intrinsic type (1 letter) or a tuple as sequence
   * of letter for each item.
   *
   *    B   -->   Boolean
   *    I   -->   Integer
   *    N   -->   Numeric
   *    L   -->   Literal (C-String)
   *    C   -->   Complex (Object like this)
   *    X   -->   Bytes array
   *
   */
  typedef const char * PLUGIN_DECL;

  typedef enum
  {
    PLUGIN_IN    = 1,     /* read only */
    PLUGIN_INOUT = 2,     /* read, write without copy */
  } PLUGIN_ARG_MODE;

  /**
   * The definition of type (single or array)
   */
  typedef struct
  {
    PLUGIN_DECL     decl; /* type or tuple declaration */
    unsigned char   ndim; /* 0 for type, n for n-dimensional array of type */
  } PLUGIN_TYPE;

  /**
   * The definition of ctor
   * A CTOR declares a list of input type
   */
  typedef struct
  {
    int id;
    unsigned args_count;  /* arguments list count */
    PLUGIN_TYPE * args;   /* arguments list */
  } PLUGIN_CTOR;

  /**
   * The definition of method argument
   * A method argument has a mode (IN/OUT) and a type
   */
  typedef struct
  {
    PLUGIN_ARG_MODE mode; /* argument mode: IN/OUT */
    PLUGIN_TYPE     type; /* argument type */
  } PLUGIN_ARG;

  /**
   * The definition of method
   * A method has a name, it declares a type returned,
   * and a list of argument
   */
  typedef struct
  {
    int id;
    const char * name;    /* name of the method */
    PLUGIN_TYPE ret;      /* returned type */
    unsigned args_count;  /* arguments list count */
    PLUGIN_ARG * args;    /* arguments list */
  } PLUGIN_METHOD;

  /**
   * The object interface
   */
  typedef struct
  {
    const char * name;            /* name this complex object */
    unsigned method_count;        /* methods list count */
    PLUGIN_METHOD * methods;      /* methods list */
    unsigned ctors_count;         /* ctor list count*/
    PLUGIN_CTOR * ctors;          /* ctor list */
  } PLUGIN_INTERFACE;

  typedef PLUGIN_HANDLE (*DLSYM_CREATE)();
  typedef int (*DLSYM_VERSION)();

#ifdef __cplusplus
}
#endif

#endif /* PLUGIN_INTERFACE_H_ */

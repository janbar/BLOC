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

#ifndef IMPORT_INTERFACE_H_
#define IMPORT_INTERFACE_H_

#ifdef __cplusplus
extern "C"
{
#endif

#define IMPORT_VERSION  221200

  typedef void* IMPORT_HANDLE;

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
  typedef const char * IMPORT_DECL;

  typedef enum
  {
    IMPORT_IN    = 1,     /* read only */
    IMPORT_INOUT = 2,     /* read, write without copy */
  } IMPORT_ARG_MODE;

  /**
   * The definition of type (single or array)
   */
  typedef struct
  {
    IMPORT_DECL     decl; /* type or tuple declaration */
    unsigned char   ndim; /* 0 for type, n for n-dimensional array of type */
  } IMPORT_TYPE;

  /**
   * The definition of argument
   */
  typedef struct
  {
    IMPORT_ARG_MODE mode; /* argument mode: IN/OUT */
    IMPORT_TYPE     type; /* argument type */
  } IMPORT_ARG;

  /**
   * The definition of method
   */
  typedef struct
  {
    int id;
    const char * name;    /* name of the method */
    IMPORT_TYPE ret;      /* returned type */
    unsigned args_count;  /* arguments list count */
    IMPORT_ARG * args;    /* arguments list */
  } IMPORT_METHOD;

  /**
   * The object interface
   */
  typedef struct
  {
    const char * name;            /* name this complex object */
    unsigned method_count;        /* methods list count */
    IMPORT_METHOD * methods;      /* methods list */
    unsigned ctor_args_count;     /* arguments list count of the constructor */
    IMPORT_ARG * ctor_args;       /* arguments list of the constructor */
  } IMPORT_INTERFACE;

  typedef IMPORT_HANDLE (*DLSYM_CREATE)();
  typedef int (*DLSYM_VERSION)();

#ifdef __cplusplus
}
#endif

#endif /* IMPORT_INTERFACE_H_ */

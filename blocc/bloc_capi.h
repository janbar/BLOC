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

#ifndef BLOC_CAPI_H
#define BLOC_CAPI_H

#include "declspec.h"

#include <stdio.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct bloc_context     bloc_context;
typedef struct bloc_symbol      bloc_symbol;
typedef struct bloc_expression  bloc_expression;
typedef struct bloc_executable  bloc_executable;
typedef struct bloc_value       bloc_value;
typedef struct bloc_array       bloc_array;
typedef struct bloc_row         bloc_row;

/**
 * The enumeration of scalar types.
 */
typedef enum
{
  NO_TYPE   = 0,
  BOOLEAN   = 1,
  INTEGER,
  NUMERIC,
  LITERAL,
  COMPLEX,
  TABCHAR,
  ROWTYPE,
  POINTER,
  IMAGINARY,
} bloc_type_major;

/**
 * The structure repensenting the type of value.
 * `ndim` is the number of array dimensions (0 for scalar).
 */
typedef struct
{
  bloc_type_major major;
  unsigned ndim;
} bloc_type;

typedef char bloc_bool;
#define bloc_true  1
#define bloc_false 0

/**
 * The structure representing a complex number.
 * `a` is the real part, `b` is the imaginary part.
 */
typedef struct {
  double a;
  double b;
} bloc_pair;

/**
 * Returns a human-readable string describing the last error.
 * The returned string is owned by the library and must not be freed.
 * @return pointer to an error message string, or NULL if no error
 */
LIBBLOC_API const char*
bloc_strerror();

/**
 * Returns the last error code set by the library.
 * The meaning of the code is library-specific.
 * @return integer error code (0 if no error)
 */
LIBBLOC_API int
bloc_errno();

/**
 * Returns the library runtime version string.
 * The returned string is owned by the library and must not be freed.
 * @return version string
 */
LIBBLOC_API const char*
bloc_version();

/**
 * Returns the version header string.
 * The returned string is owned by the library and must not be freed.
 * @return header version string
 */
LIBBLOC_API const char*
bloc_version_header();

/**
 * Returns the compatibility level of the library.
 * @return compatibility level
 */
LIBBLOC_API int
bloc_compatible();

/**
 * Authorize the use of the given plugin.
 * WARNING: Some plugins may modify the system environment, even execute
 * system commands.
 * @param name the plugin name
 */
LIBBLOC_API void
bloc_unban_plugin(const char * name);

/**
 * Remove all permissions on plugins
 */
LIBBLOC_API void
bloc_clear_plugin_permissions();

/**
 * Unload all imported modules; It should be called on program exit.
 */
LIBBLOC_API void
bloc_deinit_plugins();

/* */


/**
 * Frees a previously created `bloc_context` and releases associated
 * resources. After this call the `ctx` pointer must not be used.
 * @param ctx the context to free
 */
LIBBLOC_API void
bloc_free_context(bloc_context *ctx);

/**
 * Create and initialize a new `bloc_context`.
 * The returned context must be released with `bloc_free_context()`.
 * @param fd_out file descriptor used for standard output
 * @param fd_err file descriptor used for error output
 * @return pointer to a new `bloc_context`, or NULL on failure
 */
LIBBLOC_API bloc_context*
bloc_create_context(int fd_out, int fd_err);

/**
 * Purge context state: free temporary data and reset transient state.
 * @param ctx the context to purge
 */
LIBBLOC_API void
bloc_ctx_purge(bloc_context *ctx);

/**
 * Register a new symbol in the context's symbol table.
 * @param ctx the context
 * @param name the symbol name
 * @param type the symbol type
 * @return pointer to the registered `bloc_symbol`, or NULL on error
 */
LIBBLOC_API bloc_symbol*
bloc_ctx_register_symbol(bloc_context *ctx, const char *name, bloc_type type);

/**
 * Store a value into a previously registered symbol.
 * If the value is allocated dynamically, the payload is moved into the
 * symbol; Otherwise it is copied. The value pointed to by `v` remains
 * owned by the caller, and it must be freed if needed.
 * @param ctx the context
 * @param symbol the target symbol
 * @param v the value to store (ownership remains with the caller)
 * @return `bloc_true` on success, `bloc_false` on failure
 */
LIBBLOC_API bloc_bool
bloc_ctx_store_variable(bloc_context *ctx, const bloc_symbol *symbol, bloc_value *v);

/**
 * Lookup a symbol by name in the given context.
 * @param ctx the context
 * @param name the symbol name to find
 * @return pointer to the `bloc_symbol` if found, otherwise NULL
 */
LIBBLOC_API bloc_symbol*
bloc_ctx_find_symbol(bloc_context *ctx, const char *name);

/**
 * Load the value associated with a symbol.
 * The returned `bloc_value` is owned by the library and must not be freed
 * by the caller.
 * @param ctx the context
 * @param symbol the symbol to load
 * @return pointer to a `bloc_value` associated with the symbol
 */
LIBBLOC_API bloc_value*
bloc_ctx_load_variable(bloc_context *ctx, const bloc_symbol *symbol);

/**
 * Enable or disable tracing for the context.
 * Tracing typically prints diagnostic information to the context's output.
 * @param ctx the context
 * @param yesno `bloc_true` to enable tracing, `bloc_false` to disable
 */
LIBBLOC_API void
bloc_ctx_enable_trace(bloc_context *ctx, bloc_bool yesno);

/**
 * Query whether tracing is enabled for the context.
 * @param ctx the context
 * @return `bloc_true` if tracing is enabled, otherwise `bloc_false`
 */
LIBBLOC_API bloc_bool
bloc_ctx_trace(bloc_context *ctx);

/**
 * Purge the context's working memory used for intermediate results.
 * @param ctx the context
 */
LIBBLOC_API void
bloc_ctx_purge_working_mem(bloc_context *ctx);

/**
 * Returns the `FILE*` used for standard output by the context.
 * @param ctx the context
 * @return pointer to `FILE` for context output, or NULL
 */
LIBBLOC_API FILE*
bloc_ctx_out(bloc_context *ctx);

/**
 * Returns the `FILE*` used for error output by the context.
 * @param ctx the context
 * @return pointer to `FILE` for context errors, or NULL
 */
LIBBLOC_API FILE*
bloc_ctx_err(bloc_context *ctx);

/*
Creating and freeing values
The following functions create new values of various types.
Ownership of the returned pointer is transferred to the caller,
therefore it must be freed using `bloc_free_value()`.
*/

/**
 * Frees a previously created `bloc_value` and releases associated
 * resources. After this call the `v` pointer must not be used.
 * @param ctx the value to free
 */
LIBBLOC_API void
bloc_free_value(bloc_value *v);

/**
 * Returns new null value from given type.
 * Returned pointer must be freed using bloc_free_value().
 * @return pointer to the created null value
 */
LIBBLOC_API bloc_value*
bloc_create_null(bloc_type_major type);

/**
 * Returns new boolean value.
 * Returned pointer must be freed using bloc_free_value().
 * @return pointer to the created boolean value
 */
LIBBLOC_API bloc_value*
bloc_create_boolean(bloc_bool v);

/**
 * Returns new integer value.
 * Returned pointer must be freed using bloc_free_value().
 * @return pointer to the created integer value
 */
LIBBLOC_API bloc_value*
bloc_create_integer(int64_t v);

/**
 * Returns new numeric value.
 * Returned pointer must be freed using bloc_free_value().
 * @return pointer to the created numeric value
 */
LIBBLOC_API bloc_value*
bloc_create_numeric(double v);

/**
 * Returns new literal value.
 * Returned pointer must be freed using bloc_free_value().
 * @return pointer to the created literal value
 */
LIBBLOC_API bloc_value*
bloc_create_literal(const char *v);

/**
 * Returns new tabchar value.
 * Returned pointer must be freed using bloc_free_value().
 * @return pointer to the created tabchar value
 */
LIBBLOC_API bloc_value*
bloc_create_tabchar(const char *v, unsigned len);

/**
 * Returns new imaginary value.
 * Returned pointer must be freed using bloc_free_value().
 * @return pointer to the created imaginary value
 */
LIBBLOC_API bloc_value*
bloc_create_imaginary(bloc_pair i);

/**
 * Assign new string to the given literal value.
 * Assigning a null pointer will make the value null.
 * @param val the value to update
 * @param v the new string
 * @return `bloc_true` on success, `bloc_false` on failure
 */
LIBBLOC_API bloc_bool
bloc_assign_literal(bloc_value *val, const char *v);

/**
 * Assign new byte array to the given tabchar value.
 * Assigning a null pointer will make the value null.
 * @param val the value to update
 * @param v the new byte array
 * @return `bloc_true` on success, `bloc_false` on failure
 */
LIBBLOC_API bloc_bool
bloc_assign_tabchar(bloc_value *val, const char *v, unsigned len);

/*
Handling null value and type introspection
*/

/**
 * Returns the type of value
 */
LIBBLOC_API bloc_type
bloc_value_type(bloc_value *v);

/**
 * Returns true if value is null
 */
LIBBLOC_API bloc_bool
bloc_value_isnull(bloc_value *v);

/*
The following functions allow to bind internal data of value.
The out parameter receives a pointer to the internal data.
They return `bloc_true` when the type of value matches, otherwise
`bloc_false`.

Use `bloc_value_isnull()` to test for null value before binding.
Otherwise you have to check for a NULL pointer before deref.

Examples:

  double *data;
  if (bloc_numeric(val, &data) && data) {
    printf("value = %g\n", *data);
  }
*/

/**
 * Bind boolean data from a value.
 * If the value type is boolean this stores a pointer to the internal data
 * into `*buf`. If the value is null, `*buf` will be set to NULL.
 * @param v the value to access
 * @param buf out parameter to receive pointer to boolean data
 * @return `bloc_true` if the value type is boolean, `bloc_false` otherwise
 */
LIBBLOC_API bloc_bool
bloc_boolean(bloc_value *v, bloc_bool **buf);

/**
 * Bind integer data from a value.
 * If the value type is integer this stores a pointer to the internal data
 * into `*buf`. If the value is null, `*buf` will be set to NULL.
 * @param v the value to access
 * @param buf out parameter to receive pointer to integer data
 * @return `bloc_true` if the value type is integer, `bloc_false` otherwise
 */
LIBBLOC_API bloc_bool
bloc_integer(bloc_value *v, int64_t **buf);

/**
 * Bind numeric (double) data from a value.
 * If the value type is numeric this stores a pointer to the internal data
 * into `*buf`. If the value is null, `*buf` will be set to NULL.
 * @param v the value to access
 * @param buf out parameter to receive pointer to numeric data
 * @return `bloc_true` if the value type is numeric, `bloc_false` otherwise
 */
LIBBLOC_API bloc_bool
bloc_numeric(bloc_value *v, double **buf);

/**
 * Bind literal (string) data from a value.
 * If the value type is literal this stores a pointer to the internal data
 * into `*buf`. If the value is null, `*buf` will be set to NULL. The bound
 * data is const, therefore it cannot be modified. Use `bloc_assign_literal()`
 * to modify the value's contents.
 * @param v the value to access
 * @param buf out parameter to receive pointer to a null-terminated string
 * @return `bloc_true` if the value type is literal, `bloc_false` otherwise
 * @see `bloc_assign_literal()`
 */
LIBBLOC_API bloc_bool
bloc_literal(bloc_value *v, const char **buf);

/**
 * Bind tabchar data from a value.
 * If the value type is tabchar this stores a pointer to the internal data
 * into `*buf`. If the value is null, `*buf` will be set to NULL. The bound
 * data is const, therefore it cannot be modified. Use `bloc_assign_tabchar()`
 * to modify the value's contents.
 * @param v the value to access
 * @param buf out parameter to receive pointer to data
 * @param len out parameter to receive the length of the data
 * @return `bloc_true` if the value type is tabchar, `bloc_false` otherwise
 * @see `bloc_assign_tabchar()`
 */
LIBBLOC_API bloc_bool
bloc_tabchar(bloc_value *v, const char **buf, unsigned *len);

/**
 * Bind array/table data from a value.
 * If the value is array/table this stores a pointer to the internal data
 * into `*buf`. If the value is null, `*buf` will be set to NULL.
 * @param v the value to access
 * @param array out parameter to receive pointer to `bloc_array`
 * @return `bloc_true` if the value type is an table, `bloc_false` otherwise
 */
LIBBLOC_API bloc_bool
bloc_table(bloc_value *v, bloc_array **array);

/**
 * Bind tuple/row data from a value.
 * If the value type is tuple/row this stores a pointer to the internal data
 * into `*buf`. If the value is null, `*buf` will be set to NULL.
 * @param v the value to access
 * @param row out parameter to receive pointer to `bloc_row`
 * @return `bloc_true` if the value type is a tuple, `bloc_false` otherwise
 */
LIBBLOC_API bloc_bool
bloc_tuple(bloc_value *v, bloc_row **row);

/**
 * Bind imaginary pair data from a value.
 * If the value type is imaginary this stores a pointer to the internal data
 * into `*buf`. If the value is null, `*buf` will be set to NULL.
 * @param v the value to access
 * @param buf out parameter to receive pointer to `bloc_pair`
 * @return `bloc_true` if the value type is imaginary, `bloc_false` otherwise
 */
LIBBLOC_API bloc_bool
bloc_imaginary(bloc_value *v, bloc_pair **buf);

/*
About array
*/

/**
 * Returns the count of items in the array/table.
 * @param array the table value
 * @return the count
 */
LIBBLOC_API unsigned
bloc_array_size(bloc_array *array);

/**
 * Retrieves the item value at position in the array/table.
 * @param array the array/table value
 * @param index the item position
 * @param v pointer to item value
 * @return succeeded or failed
 */
LIBBLOC_API bloc_bool
bloc_array_item(bloc_array *array, unsigned index, bloc_value **v);

/*
About row
*/

/**
 * Returns the count of items wrapped in the tuple/row.
 * @param row the tuple/row value
 * @return the count
 */
LIBBLOC_API unsigned
bloc_tuple_size(bloc_row *row);

/**
 * Retrieves the item value at position in the tuple/row.
 * @param row the tuple/row value
 * @param index the item position
 * @param v pointer to item value
 * @return succeeded or failed
 */
LIBBLOC_API bloc_bool
bloc_tuple_item(bloc_row *row, unsigned index, bloc_value **v);

/*
Parsing and processing expressions and executables
*/

/**
 * Returns compiled expression from source.
 * Returned pointer must be freed by the caller using `bloc_free_expression()`.
 * @return pointer to compiled expression, or NULL on failure
 */
LIBBLOC_API bloc_expression*
bloc_parse_expression(bloc_context *ctx, const char *text);

/**
 * Free a `bloc_expression` previously created by `bloc_parse_expression()`.
 * @param e the expression to free
 */
LIBBLOC_API void
bloc_free_expression(bloc_expression *e);

/**
 * Return the type of an expression (as determined in the given context).
 * @param ctx the context used for type resolution
 * @param e the expression to inspect
 * @return the `bloc_type` of the expression
 */
LIBBLOC_API bloc_type
bloc_expression_type(bloc_context *ctx, bloc_expression *e);

/**
 * Evaluate the expression.
 * The returned `bloc_value` is owned by the library and must not be freed.
 * @param ctx the context
 * @param e the expression
 * @return the value
 */
LIBBLOC_API bloc_value*
bloc_evaluate_expression(bloc_context *ctx, bloc_expression *e);

/**
 * Returns compiled executable from source.
 * Returned pointer must be freed by the caller using `bloc_free_executable()`.
 * @return pointer to a new `bloc_executable`, or NULL on failure
 */
LIBBLOC_API bloc_executable*
bloc_parse_executable(bloc_context *ctx, const char *text);

/**
 * Free a `bloc_executable` previously created by `bloc_parse_executable()`.
 * @param exec the executable to free
 */
LIBBLOC_API void
bloc_free_executable(bloc_executable *exec);

/**
 * Run the given executable.
 * @param exec the executable to run
 * @return `bloc_true` on success, `bloc_false` on failure
 */
LIBBLOC_API bloc_bool
bloc_execute(bloc_executable *exec);

/**
 * Retrieve and take ownership of the last returned value from the context.
 * Returned pointer must be freed by the caller using `bloc_free_value()`.
 * @return pointer to returned value, or NULL if no value
 */
LIBBLOC_API bloc_value*
bloc_drop_returned(bloc_context *ctx);

/**
 * Stop running executable or expression. The context won't be reusable
 * until the purge or a reset of the stop condition.
 * @param ctx the running context
 * @see `bloc_purge()`
 * @see `bloc_reset_stop()`
 */
LIBBLOC_API void
bloc_break(bloc_context *ctx);

/**
 * Reset the stop condition of context.
 * @param ctx the stopped context
 */
LIBBLOC_API void
bloc_reset_stop(bloc_context *ctx);

#ifdef __cplusplus
}
#endif

#endif /* BLOC_CAPI_H */


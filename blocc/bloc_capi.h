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

typedef struct
{
  bloc_type_major major;
  unsigned ndim;
} bloc_type;

typedef char bloc_bool;
#define bloc_true  1
#define bloc_false 0

typedef struct {
  double a;
  double b;
} bloc_pair;

LIBBLOC_API const char*
bloc_strerror();

LIBBLOC_API int
bloc_errno();

LIBBLOC_API const char*
bloc_version();

LIBBLOC_API const char*
bloc_version_header();

LIBBLOC_API int
bloc_compatible();

/**
 * Authorize the use of the given plugin.
 * WARNING: Some plugins may modify the system environment, even execute
 * system commands.
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

LIBBLOC_API void
bloc_free_context(bloc_context *ctx);

/**
 * Returns new context.
 * Returned pointer must be freed using bloc_free_context().
 */
LIBBLOC_API bloc_context*
bloc_create_context(int fd_out, int fd_err);

LIBBLOC_API void
bloc_ctx_purge(bloc_context *ctx);

LIBBLOC_API bloc_symbol*
bloc_ctx_register_symbol(bloc_context *ctx, const char *name, bloc_type type);

LIBBLOC_API bloc_bool
bloc_ctx_store_variable(bloc_context *ctx, const bloc_symbol *symbol, bloc_value *v);

LIBBLOC_API bloc_symbol*
bloc_ctx_find_symbol(bloc_context *ctx, const char *name);

LIBBLOC_API bloc_value*
bloc_ctx_load_variable(bloc_context *ctx, const bloc_symbol *symbol);

LIBBLOC_API void
bloc_ctx_enable_trace(bloc_context *ctx, bloc_bool yesno);

LIBBLOC_API bloc_bool
bloc_ctx_trace(bloc_context *ctx);

LIBBLOC_API void
bloc_ctx_purge_working_mem(bloc_context *ctx);

LIBBLOC_API FILE*
bloc_ctx_out(bloc_context *ctx);

LIBBLOC_API FILE*
bloc_ctx_err(bloc_context *ctx);

/* */

LIBBLOC_API void
bloc_free_value(bloc_value *v);

/**
 * Returns new null value from given type.
 * Returned pointer must be freed using bloc_free_value().
 */
LIBBLOC_API bloc_value*
bloc_create_null(bloc_type_major type);

/**
 * Returns new boolean value.
 * Returned pointer must be freed using bloc_free_value().
 */
LIBBLOC_API bloc_value*
bloc_create_boolean(bloc_bool v);

/**
 * Returns new integer value.
 * Returned pointer must be freed using bloc_free_value().
 */
LIBBLOC_API bloc_value*
bloc_create_integer(int64_t v);

/**
 * Returns new numeric value.
 * Returned pointer must be freed using bloc_free_value().
 */
LIBBLOC_API bloc_value*
bloc_create_numeric(double v);

/**
 * Returns new literal value.
 * Returned pointer must be freed using bloc_free_value().
 */
LIBBLOC_API bloc_value*
bloc_create_literal(const char *v);

/**
 * Returns new tabchar value.
 * Returned pointer must be freed using bloc_free_value().
 */
LIBBLOC_API bloc_value*
bloc_create_tabchar(const char *v, unsigned len);

/**
 * Returns new imaginary value.
 * Returned pointer must be freed using bloc_free_value().
 */
LIBBLOC_API bloc_value*
bloc_create_imaginary(bloc_pair i);

/* */

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

/* The functions to follow allow to bind the data of value, for a type.
 * Each one fills the reference of the data, or NULL for a null value,
 * into the given pointer. It returns true if the type of value matches,
 * else false.
 *
 * You can test for a null value before binding, using bloc_value_isnull(),
 * else you have to check for a NULL pointer before deref.
 *
 *   double *data;
 *   if (bloc_numeric(val, &data) && data) {
 *     printf("value = %g\n", *data);
 *   }
 */

LIBBLOC_API bloc_bool
bloc_boolean(bloc_value *v, bloc_bool **buf);

LIBBLOC_API bloc_bool
bloc_integer(bloc_value *v, int64_t **buf);

LIBBLOC_API bloc_bool
bloc_numeric(bloc_value *v, double **buf);

LIBBLOC_API bloc_bool
bloc_literal(bloc_value *v, const char **buf);

LIBBLOC_API bloc_bool
bloc_tabchar(bloc_value *v, const char **buf, unsigned *len);

LIBBLOC_API bloc_bool
bloc_table(bloc_value *v, bloc_array **array);

LIBBLOC_API bloc_bool
bloc_tuple(bloc_value *v, bloc_row **row);

LIBBLOC_API bloc_bool
bloc_imaginary(bloc_value *v, bloc_pair **buf);

/* About array */

/**
 * Returns the count of items in the table.
 * @param array the table value
 * @return the count
 */
LIBBLOC_API unsigned
bloc_array_size(bloc_array *array);

/**
 * Retrieves the item value at position in the table.
 * @param array the table value
 * @param index the item position
 * @param v pointer to item value
 * @return succeeded or failed
 */
LIBBLOC_API bloc_bool
bloc_array_item(bloc_array *array, unsigned index, bloc_value **v);

/* About row */

/**
 * Returns the count of items wrapped in the tuple.
 * @param row the tuple value
 * @return the count
 */
LIBBLOC_API unsigned
bloc_tuple_size(bloc_row *row);

/**
 * Retrieves the item value at position in the tuple.
 * @param row the tuple value
 * @param index the item position
 * @param v pointer to item value
 * @return succeeded or failed
 */
LIBBLOC_API bloc_bool
bloc_tuple_item(bloc_row *row, unsigned index, bloc_value **v);

/* About parsing and processing */

/**
 * Returns expression from source.
 * Returned pointer must be freed using bloc_free_expression().
 */
LIBBLOC_API bloc_expression*
bloc_parse_expression(bloc_context *ctx, const char *text);

LIBBLOC_API void
bloc_free_expression(bloc_expression *e);

LIBBLOC_API bloc_type
bloc_expression_type(bloc_context *ctx, bloc_expression *e);

/**
 * Evaluate the expression.
 * @param ctx the context
 * @param e the expression
 * @return the value
 */
LIBBLOC_API bloc_value*
bloc_evaluate_expression(bloc_context *ctx, bloc_expression *e);

/**
 * Returns executable from source.
 * Returned pointer must be freed using bloc_free_executable().
 */
LIBBLOC_API bloc_executable*
bloc_parse_executable(bloc_context *ctx, const char *text);

LIBBLOC_API void
bloc_free_executable(bloc_executable *exec);

/**
 * Run executable.
 */
LIBBLOC_API bloc_bool
bloc_execute(bloc_executable *exec);

/**
 * Retrieve last returned result (value).
 * Returned pointer must be freed using bloc_free_value().
 */
LIBBLOC_API bloc_value*
bloc_drop_returned(bloc_context *ctx);

/**
 * Stop running executable or expression. The context won't be reusable until
 * the purge or a reset of the stop condition.
 * @param ctx the running context
 * @see bloc_purge
 * @see bloc_reset_stop
 */
LIBBLOC_API void
bloc_break(bloc_context *ctx);

/**
 * Reset the stop condition of context
 * @param ctx the stopped context
 */
LIBBLOC_API void
bloc_reset_stop(bloc_context *ctx);

#ifdef __cplusplus
}
#endif

#endif /* BLOC_CAPI_H */


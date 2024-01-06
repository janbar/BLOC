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

struct bloc_context;
struct bloc_symbol;
struct bloc_expression;
struct bloc_executable;

typedef struct bloc_context             BLOC_CONTEXT;
typedef struct bloc_symbol              BLOC_SYMBOL;
typedef struct bloc_expression          BLOC_EXPRESSION;
typedef struct bloc_executable          BLOC_EXECUTABLE;
typedef struct bloc_value               BLOC_VALUE;

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
} bloc_type_major;

typedef struct
{
  bloc_type_major major;
  unsigned ndim;
} bloc_type;

typedef unsigned int bloc_bool;
#define bloc_true  1
#define bloc_false 0

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

/* */

LIBBLOC_API void
bloc_free_context(BLOC_CONTEXT *ctx);

/**
 * Returns new context.
 * Returned pointer must be freed using bloc_free_context().
 */
LIBBLOC_API BLOC_CONTEXT*
bloc_create_context(int fd_out, int fd_err);

LIBBLOC_API void
bloc_ctx_purge(BLOC_CONTEXT *ctx);

LIBBLOC_API BLOC_SYMBOL*
bloc_ctx_register_symbol(BLOC_CONTEXT *ctx, const char *name, bloc_type type);

LIBBLOC_API bloc_bool
bloc_ctx_store_variable(BLOC_CONTEXT *ctx, const BLOC_SYMBOL *symbol, BLOC_VALUE *v);

LIBBLOC_API BLOC_SYMBOL*
bloc_ctx_find_symbol(BLOC_CONTEXT *ctx, const char *name);

LIBBLOC_API BLOC_VALUE*
bloc_ctx_load_variable(BLOC_CONTEXT *ctx, const BLOC_SYMBOL *symbol);

LIBBLOC_API void
bloc_ctx_enable_trace(BLOC_CONTEXT *ctx, bloc_bool yesno);

bloc_bool
bloc_ctx_trace(BLOC_CONTEXT *ctx);

LIBBLOC_API void
bloc_ctx_purge_working_mem(BLOC_CONTEXT *ctx);

LIBBLOC_API FILE*
bloc_ctx_out(BLOC_CONTEXT *ctx);

LIBBLOC_API FILE*
bloc_ctx_err(BLOC_CONTEXT *ctx);

/* */

LIBBLOC_API void
bloc_free_value(BLOC_VALUE *v);

/**
 * Returns new boolean value.
 * Returned pointer must be freed using bloc_free_value().
 */
LIBBLOC_API BLOC_VALUE*
bloc_create_boolean(bloc_bool v);

/**
 * Returns new integer value.
 * Returned pointer must be freed using bloc_free_value().
 */
LIBBLOC_API BLOC_VALUE*
bloc_create_integer(int64_t v);

/**
 * Returns new numeric value.
 * Returned pointer must be freed using bloc_free_value().
 */
LIBBLOC_API BLOC_VALUE*
bloc_create_numeric(double v);

/**
 * Returns new literal value.
 * Returned pointer must be freed using bloc_free_value().
 */
LIBBLOC_API BLOC_VALUE*
bloc_create_literal(const char *v);

/**
 * Returns new tabchar value.
 * Returned pointer must be freed using bloc_free_value().
 */
LIBBLOC_API BLOC_VALUE*
bloc_create_tabchar(const char *v, unsigned len);

/* */

LIBBLOC_API bloc_type
bloc_expression_type(BLOC_CONTEXT *ctx, BLOC_EXPRESSION *e);

/**
 * Process an expression
 */
LIBBLOC_API BLOC_VALUE*
bloc_evaluate_expression(BLOC_CONTEXT *ctx, BLOC_EXPRESSION *e);

/* */

/**
 * Returns the value type
 */
LIBBLOC_API bloc_type
bloc_value_type(BLOC_VALUE *v);

LIBBLOC_API bloc_bool
bloc_boolean(BLOC_VALUE *v, bloc_bool **buf);

LIBBLOC_API bloc_bool
bloc_integer(BLOC_VALUE *v, int64_t **buf);

LIBBLOC_API bloc_bool
bloc_numeric(BLOC_VALUE *v, double **buf);

LIBBLOC_API bloc_bool
bloc_literal(BLOC_VALUE *v, const char **buf);

LIBBLOC_API bloc_bool
bloc_tabchar(BLOC_VALUE *v, const char **buf, unsigned *len);

/* */

/**
 * Returns expression from source.
 * Returned pointer must be freed using bloc_free_expression().
 */
LIBBLOC_API BLOC_EXPRESSION*
bloc_parse_expression(BLOC_CONTEXT *ctx, const char *text);

LIBBLOC_API void
bloc_free_expression(BLOC_EXPRESSION *e);

/**
 * Returns executable from source.
 * Returned pointer must be freed using bloc_free_executable().
 */
LIBBLOC_API BLOC_EXECUTABLE*
bloc_parse_executable(BLOC_CONTEXT *ctx, const char *text);

LIBBLOC_API void
bloc_free_executable(BLOC_EXECUTABLE *exec);

/**
 * Run executable.
 */
LIBBLOC_API bloc_bool
bloc_execute(BLOC_EXECUTABLE *exec);

/**
 * Retrieve last returned result (value).
 * Returned pointer must be freed using bloc_free_value().
 */
LIBBLOC_API BLOC_VALUE*
bloc_drop_returned(BLOC_CONTEXT *ctx);

#ifdef __cplusplus
}
#endif

#endif /* BLOC_CAPI_H */


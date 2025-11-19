BLOC C API
==========

2025-11

This document summarizes the public C API exposed in `blocc/bloc_capi.h`.

## Overview

The API provides a minimal C interface for creating and manipulating a `bloc` runtime context, creating values, parsing and executing expressions and scripts, and inspecting results.

All pointers returned by the API are owned by the caller unless the function documentation explicitly states otherwise. When provided, `bloc_free_*` functions must be used to release returned objects.

---

## Types

- **`bloc_context`** : opaque runtime context
- **`bloc_symbol`**  : opaque symbol representation
- **`bloc_expression`** : opaque parsed expression
- **`bloc_executable`** : opaque parsed executable/script
- **`bloc_value`** : opaque typed value container
- **`bloc_array`** : opaque array/table value
- **`bloc_row`** : opaque tuple/row value

- **`bloc_type_major`** : enum of major types
  -  NO_TYPE
  -  BOOLEAN
  -  INTEGER
  -  NUMERIC
  -  LITERAL
  -  COMPLEX
  -  TABCHAR
  -  ROWTYPE
  -  POINTER
  -  IMAGINARY

- **`bloc_type`** : The structure represents the type of value

  *struct { bloc_type_major major; unsigned ndim; }*

  **ndim** is the number of array dimensions (0 for scalar).
  
- **`bloc_bool`** : char (use `bloc_true` / `bloc_false`)
- **`bloc_pair`** : The structure represents a complex number

  *struct { double a; double b; }*

  **a** is the real part, **b** is the imaginary part.

---

## Error and Version

- **`const char* bloc_strerror();`**

  Returns a human-readable string describing the last error (owned by library).

- **`int bloc_errno();`**

  Returns the last error code set by the library.

- **`const char* bloc_version();`**

  Runtime version string.

- **`const char* bloc_version_header();`**

  Version header string.

- **`int bloc_compatible();`**

  The compatibility level of the library.

---

## Plugin Management

- **`void bloc_unban_plugin(const char *name);`**

  Authorize the use of the given plugin.

- **`void bloc_clear_plugin_permissions();`**

  Remove all permissions on plugins.

- **`void bloc_deinit_plugins();`**

  Unload imported modules; call on program exit.

---

## Context lifecycle and symbol API

- **`void bloc_free_context(bloc_context *ctx);`**

  Free a context and its resources.

- **`bloc_context* bloc_create_context(int fd_out, int fd_err);`**

  Create a new context. `fd_out`/`fd_err` are file descriptors used for stdout/stderr by the context.

- **`void bloc_ctx_purge(bloc_context *ctx);`**

  Purge temporary context state and reset transient data.

- **`bloc_symbol* bloc_ctx_register_symbol(bloc_context *ctx, const char *name, bloc_type type);`**

  Register a symbol in the context's symbol table. The returned `bloc_symbol*` is owned by the library.

- **`bloc_bool bloc_ctx_store_variable(bloc_context *ctx, const bloc_symbol *symbol, bloc_value *v);`**

  Store a value in a previously registered symbol. Returns `bloc_true` on success. If the value is allocated dynamically, the payload is moved into the symbol; Otherwise it is copied. The value pointed to by `v` remains owned by the caller, and it must be freed if needed.

- **`bloc_symbol* bloc_ctx_find_symbol(bloc_context *ctx, const char *name);`**

  Find a symbol by name; returns NULL if not found. The returned `bloc_symbol*` is owned by the library.

- **`bloc_value* bloc_ctx_load_variable(bloc_context *ctx, const bloc_symbol *symbol);`**

  Load the value associated with a symbol. The returned `bloc_value*`  is owned by the library and must not be freed.

- **`void bloc_ctx_enable_trace(bloc_context *ctx, bloc_bool yesno);`**

  Enable or disable tracing for the context.

- **`bloc_bool bloc_ctx_trace(bloc_context *ctx);`**

  Query whether tracing is enabled.

- **`void bloc_ctx_purge_working_mem(bloc_context *ctx);`**

  Purge working memory used for intermediate results.

- **`FILE* bloc_ctx_out(bloc_context *ctx);`**

  Returns the `FILE*` used by the context for standard output.

- **`FILE* bloc_ctx_err(bloc_context *ctx);`**

  Returns the `FILE*` used by the context for error output.

---

## Value creation and lifecycle

- **`void bloc_free_value(bloc_value *v);`**

  Free a `bloc_value` created by the API.

- **`bloc_value* bloc_create_null(bloc_type_major type);`**

  Create a null value for the given type.

- **`bloc_value* bloc_create_boolean(bloc_bool v);`**

  Create a boolean value.

- **`bloc_value* bloc_create_integer(int64_t v);`**

  Create an integer value.

- **`bloc_value* bloc_create_numeric(double v);`**

  Create a numeric (double) value.

- **`bloc_value* bloc_create_literal(const char *v);`**

  Create a literal (string) value (caller supplies null-terminated string).

- **`bloc_value* bloc_create_tabchar(const char *v, unsigned len);`**

  Create a tabchar value from a buffer + length.

- **`bloc_value* bloc_create_imaginary(bloc_pair i);`**

  Create an imaginary value.

---

## Value inspection and binding

- **`bloc_type bloc_value_type(bloc_value *v);`**

  Return the type description of a `bloc_value`.

- **`bloc_bool bloc_value_isnull(bloc_value *v);`**

  Returns `bloc_true` if the value is null.

### Binding helpers

Each fills an out-parameter with a pointer to the internal data (or NULL for a null value).

- **`bloc_bool bloc_boolean(bloc_value *v, bloc_bool **buf);`**

  If `v` is a boolean, sets `*buf` to point to the boolean data. Returns `bloc_true` if the type matches.

- **`bloc_bool bloc_integer(bloc_value *v, int64_t **buf);`**

  If `v` is an integer, sets `*buf` to point to the integer data. Returns `bloc_true` if the type matches.

- **`bloc_bool bloc_numeric(bloc_value *v, double **buf);`**

  If `v` is a numeric, sets `*buf` to point to the numeric data. Returns `bloc_true` if the type matches.

- **`bloc_bool bloc_literal(bloc_value *v, const char **buf);`**

  If `v` is a literal, sets `*buf` to point to the literal data. Returns `bloc_true` if the type matches.

- **`bloc_bool bloc_tabchar(bloc_value *v, const char **buf, unsigned *len);`**

  If `v` is a tabchar, sets `*buf` to point to the tabchar data. Returns `bloc_true` if the type matches.

- **`bloc_bool bloc_table(bloc_value *v, bloc_array **array);`**

  If `v` is a table, sets `*buf` to point to the table data. Returns `bloc_true` if the type matches.

- **`bloc_bool bloc_tuple(bloc_value *v, bloc_row **row);`**

  If `v` is a tuple, sets `*buf` to point to the tuple data. Returns `bloc_true` if the type matches.

- **`bloc_bool bloc_imaginary(bloc_value *v, bloc_pair **buf);`**

  If `v` is a complex number, sets `*buf` to point to the imaginary data. Returns `bloc_true` if the type matches.

### Arrays and tuples

- **`unsigned bloc_array_size(bloc_array *array);`**

  Returns the number of items in an array/table.

- **`bloc_bool bloc_array_item(bloc_array *array, unsigned index,** bloc_value **v);`

  Retrieve the item at `index` (0-based). On success `*v` is set to the internal item value pointer.

- **`unsigned bloc_tuple_size(bloc_row *row);`**

  Returns the number of items in a tuple/row.

- **`bloc_bool bloc_tuple_item(bloc_row *row, unsigned index, bloc_value **v);`**

  Retrieve the item at `index` (0-based). On success `*v` is set to the internal item value pointer.

---

## Parsing and execution

- **`bloc_expression* bloc_parse_expression(bloc_context *ctx, const char *text);`**

  Parse an expression from `text`. Returned pointer must be freed with `bloc_free_expression()`.

- **`void bloc_free_expression(bloc_expression *e);`**

   Free a `bloc_expression` created by the API.

- **`bloc_type bloc_expression_type(bloc_context *ctx, bloc_expression *e);`**

  Return the expression type as resolved in `ctx`.

- **`bloc_value* bloc_evaluate_expression(bloc_context *ctx, bloc_expression *e);`**

  Evaluate `e` in `ctx` and return the value (it is owned by the library and must not be freed).

- **`bloc_executable* bloc_parse_executable(bloc_context *ctx, const char *text);`**

  Parse an executable/script from `text`. Returned pointer must be freed with `bloc_free_executable()`.

- **`void bloc_free_executable(bloc_executable *exec);`**

   Free a `bloc_executable` created by the API.

- **`bloc_bool bloc_execute(bloc_executable *exec);`**

  Run the executable. Returns `bloc_true` on success.

- **`bloc_value* bloc_drop_returned(bloc_context *ctx);`**

  Retrieve and take ownership of the last returned result; returned value must be freed with `bloc_free_value()`.

- **`void bloc_break(bloc_context *ctx);`**

  Stop running executable or expression. Context requires purge or reset afterwards.

- **`void bloc_reset_stop(bloc_context *ctx);`**

  Reset the stop condition of the context.

---

## Notes and pointers

This document is generated from `blocc/bloc_capi.h`. For detailed behavior and ownership semantics consult the implementation and source comments.

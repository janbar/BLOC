#include <blocc/bloc_capi.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv)
{
  BLOC_CONTEXT* ctx = bloc_create_context(fileno(stdout), fileno(stderr));

  printf("BLOC version    = %s\n", bloc_version());
  printf("BLOC header     = %s\n", bloc_version_header());
  printf("BLOC compatible = %d\n", bloc_compatible());

  {
    BLOC_VALUE * tmp_a = bloc_create_boolean(bloc_true);
    BLOC_SYMBOL * sym_a = bloc_ctx_register_symbol(ctx, "B", bloc_value_type(tmp_a));
    bloc_ctx_store_variable(ctx, sym_a, tmp_a);
    bloc_free_value(tmp_a);

    BLOC_VALUE * sto_a = bloc_ctx_load_variable(ctx, sym_a);

    bloc_bool  * val;
    if (bloc_boolean(sto_a, &val))
      printf("B        = %u\n", *val);
  }

  {
    BLOC_VALUE * tmp_a = bloc_create_integer(123456789);
    BLOC_SYMBOL * sym_a = bloc_ctx_register_symbol(ctx, "I", bloc_value_type(tmp_a));
    bloc_ctx_store_variable(ctx, sym_a, tmp_a);
    bloc_free_value(tmp_a);

    BLOC_VALUE * sto_a = bloc_ctx_load_variable(ctx, sym_a);

    int64_t * val;
    if (bloc_integer(sto_a, &val))
      printf("I        = %ld\n", (long) *val);
  }

  {
    BLOC_VALUE * tmp_a = bloc_create_numeric(1.23456);
    BLOC_SYMBOL * sym_a = bloc_ctx_register_symbol(ctx, "N", bloc_value_type(tmp_a));
    bloc_ctx_store_variable(ctx, sym_a, tmp_a);
    bloc_free_value(tmp_a);

    BLOC_VALUE * sto_a = bloc_ctx_load_variable(ctx, sym_a);

    double * val;
    if (bloc_numeric(sto_a, &val))
      printf("N        = %g\n", *val);
  }

  {
    BLOC_VALUE * tmp_a = bloc_create_literal("abcdefgh");
    BLOC_SYMBOL * sym_a = bloc_ctx_register_symbol(ctx, "L", bloc_value_type(tmp_a));
    bloc_ctx_store_variable(ctx, sym_a, tmp_a);
    bloc_free_value(tmp_a);

    BLOC_VALUE * sto_a = bloc_ctx_load_variable(ctx, sym_a);

    const char* val;
    if (bloc_literal(sto_a, &val))
      printf("L        = %s\n", val);
  }

  {
    char raw[8] = { 65, 0, 67, 0, 69, 0, 71, 127 };
    BLOC_VALUE * tmp_a = bloc_create_tabchar(raw, sizeof(raw));
    BLOC_SYMBOL * sym_a = bloc_ctx_register_symbol(ctx, "L", bloc_value_type(tmp_a));
    bloc_ctx_store_variable(ctx, sym_a, tmp_a);
    bloc_free_value(tmp_a);

    BLOC_VALUE * sto_a = bloc_ctx_load_variable(ctx, sym_a);

    const char* val;
    unsigned len;
    if (bloc_tabchar(sto_a, &val, &len))
    {
      printf("X        =");
      for (unsigned i = 0; i < len; ++i)
        printf(" %c", (val[i] == 0 ? '.' : val[i]));
      printf("\n");
    }
  }

  {
    double * val;
    BLOC_EXPRESSION * e = bloc_parse_expression(ctx, "sqrt( sin(3*pi/4)**2 + cos(3*pi/4)**2 )\n");
    BLOC_VALUE * v;
    if (e && (v = bloc_evaluate_expression(ctx, e)) && bloc_numeric(v, &val))
    {
      printf("expr     = %3f\n", *val);
    }
    else
    {
      printf("ERROR: %s\n", bloc_strerror());
    }
    bloc_free_expression(e);

    e = bloc_parse_expression(ctx, "sqrt( sin(3*pi/4)**2 + cos(3*pi/4)**2\n");
    if (e && (v = bloc_evaluate_expression(ctx, e)) && bloc_numeric(v, &val))
    {
      printf("ERROR: No throw\n");
    }
    else
    {
      printf("expr     = %s\n", bloc_strerror());
    }
    bloc_free_expression(e);
  }

  {
    bloc_ctx_purge(ctx);
    BLOC_VALUE * tmp_1 = bloc_create_integer(100);
    BLOC_SYMBOL * sym_1 = bloc_ctx_register_symbol(ctx, "$1", bloc_value_type(tmp_1));
    bloc_ctx_store_variable(ctx, sym_1, tmp_1);
    bloc_free_value(tmp_1);

    BLOC_EXECUTABLE * x = bloc_parse_executable(ctx,
          "cnt=0;\nfor i in 2 to $1 loop\nb=true;\n"
          "for j in 2 to i/2 loop\nif i%j == 0 then b=false; break; end if;\n"
          "end loop;\nif b then put i \" \"; cnt=cnt+1; end if;\nend loop;\nreturn cnt;");
    if (x)
    {
      printf("exec     =\n");
      fflush(stdout);
      if (!bloc_execute(x))
      {
        printf("ERROR: %s\n", bloc_strerror());
      }
      else
      {
        BLOC_VALUE * r = bloc_drop_returned(ctx);
        if (r)
        {
          int64_t * val;
          bloc_integer(r, &val);
          printf("\nreturn   = %ld\n", (long) *val);
          bloc_free_value(r);
        }
        else
        {
          printf("\nERROR: no return\n");
        }
      }
      bloc_free_executable(x);
    }
    else
    {
      printf("ERROR: %s\n", bloc_strerror());
    }

    bloc_ctx_purge(ctx);
    x = bloc_parse_executable(ctx,
          "cnt=0;\nfor i in 2 to $1 loop\nb=true;\n"
          "for j in 2 to i/2 loop\nif i%j == 0 then b=false; break; end if;\n"
          "end loop;\nif b then put i \" \"; cnt=cnt+1; end if;\nend loop;\nreturn cnt;");
    if (!x)
    {
      printf("exec     = %s\n", bloc_strerror());
    }
    else
    {
      printf("ERROR: No throw\n");
      bloc_free_executable(x);
    }

    x = bloc_parse_executable(ctx, "begin do 1/0; end;");
    if (x)
    {
      if (!bloc_execute(x))
      {
        printf("exec     = %s\n", bloc_strerror());
      }
      else
      {
        printf("ERROR: No throw\n");
      }
      bloc_free_executable(x);
    }
    else
    {
      printf("ERROR: %s\n", bloc_strerror());
    }
  }

  bloc_free_context(ctx);

  return EXIT_SUCCESS;
}

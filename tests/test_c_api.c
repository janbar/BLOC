#include <blocc/bloc_capi.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv)
{
  bloc_context* ctx = bloc_create_context(fileno(stdout), fileno(stderr));

  printf("BLOC version    = %s\n", bloc_version());
  printf("BLOC header     = %s\n", bloc_version_header());
  printf("BLOC compatible = %d\n", bloc_compatible());

  {
    bloc_value * tmp_a = bloc_create_boolean(bloc_true);
    bloc_symbol * sym_a = bloc_ctx_register_symbol(ctx, "B", bloc_value_type(tmp_a));
    bloc_ctx_store_variable(ctx, sym_a, tmp_a);
    bloc_free_value(tmp_a);

    bloc_value * sto_a = bloc_ctx_load_variable(ctx, sym_a);

    bloc_bool  * val;
    if (bloc_boolean(sto_a, &val))
      printf("B        = %d\n", (int)(*val));
  }

  {
    bloc_value * tmp_a = bloc_create_integer(123456789);
    bloc_symbol * sym_a = bloc_ctx_register_symbol(ctx, "I", bloc_value_type(tmp_a));
    bloc_ctx_store_variable(ctx, sym_a, tmp_a);
    bloc_free_value(tmp_a);

    bloc_value * sto_a = bloc_ctx_load_variable(ctx, sym_a);

    int64_t * val;
    if (bloc_integer(sto_a, &val))
      printf("I        = %ld\n", (long) *val);
  }

  {
    bloc_value * tmp_a = bloc_create_numeric(1.23456);
    bloc_symbol * sym_a = bloc_ctx_register_symbol(ctx, "N", bloc_value_type(tmp_a));
    bloc_ctx_store_variable(ctx, sym_a, tmp_a);
    bloc_free_value(tmp_a);

    bloc_value * sto_a = bloc_ctx_load_variable(ctx, sym_a);

    double * val;
    if (bloc_numeric(sto_a, &val))
      printf("N        = %g\n", *val);
  }

  {
    bloc_value * tmp_a = bloc_create_literal("abcdefgh");
    bloc_symbol * sym_a = bloc_ctx_register_symbol(ctx, "L", bloc_value_type(tmp_a));
    bloc_ctx_store_variable(ctx, sym_a, tmp_a);
    bloc_free_value(tmp_a);

    bloc_value * sto_a = bloc_ctx_load_variable(ctx, sym_a);

    const char* val;
    if (bloc_literal(sto_a, &val))
      printf("L        = %s\n", val);
  }

  {
    char raw[8] = { 65, 0, 67, 0, 69, 0, 71, 127 };
    bloc_value * tmp_a = bloc_create_tabchar(raw, sizeof(raw));
    bloc_symbol * sym_a = bloc_ctx_register_symbol(ctx, "L", bloc_value_type(tmp_a));
    bloc_ctx_store_variable(ctx, sym_a, tmp_a);
    bloc_free_value(tmp_a);

    bloc_value * sto_a = bloc_ctx_load_variable(ctx, sym_a);

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
    bloc_pair p = { 1.23456, -6.54321 };
    bloc_value * tmp_a = bloc_create_imaginary(p);
    bloc_symbol * sym_a = bloc_ctx_register_symbol(ctx, "C", bloc_value_type(tmp_a));
    bloc_ctx_store_variable(ctx, sym_a, tmp_a);
    bloc_free_value(tmp_a);

    bloc_value * sto_a = bloc_ctx_load_variable(ctx, sym_a);

    bloc_pair * val;
    if (bloc_imaginary(sto_a, &val))
      printf("C        = { %g , %g }\n", val->a, val->b);
  }

  {
    double * val;
    bloc_expression * e = bloc_parse_expression(ctx, "sqrt( sin(3*pi/4)**2 + cos(3*pi/4)**2 )\n");
    bloc_value * v;
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
      printf("expr_err = %s\n", bloc_strerror());
    }
    bloc_free_expression(e);
  }

  {
    bloc_ctx_purge(ctx);
    bloc_value * tmp_1 = bloc_create_integer(100);
    bloc_symbol * sym_1 = bloc_ctx_register_symbol(ctx, "$1", bloc_value_type(tmp_1));
    bloc_ctx_store_variable(ctx, sym_1, tmp_1);
    bloc_free_value(tmp_1);

    bloc_executable * x = bloc_parse_executable(ctx,
          "cnt=0;\nfor i in 2 to $1 loop\nb=true;\n"
          "for j in 2 to i/2+1 loop\nif i%j == 0 then b=false; break; end if;\n"
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
        bloc_value * r = bloc_drop_returned(ctx);
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
          "for j in 2 to i/2+1 loop\nif i%j == 0 then b=false; break; end if;\n"
          "end loop;\nif b then put i \" \"; cnt=cnt+1; end if;\nend loop;\nreturn cnt;");
    if (!x)
    {
      printf("exec_err = %s\n", bloc_strerror());
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
        printf("exec_err = %s\n", bloc_strerror());
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

    bloc_ctx_purge(ctx);
    x = bloc_parse_executable(ctx,
          "a=tab(10, tab(5, tup(1234, 1.234, true, \"abcdefgh\")));");
    if (x)
    {
      if (bloc_execute(x))
      {
        bloc_symbol * a = bloc_ctx_find_symbol(ctx, "A");
        if (!a)
          printf("ERROR: Symbol A not found\n");
        else
        {
          /* get value of variable A */
          bloc_value * v = bloc_ctx_load_variable(ctx, a);
          if (!v || bloc_value_isnull(v))
            printf("ERROR: A is NULL\n");
          else if (bloc_value_type(v).ndim != 2)
            printf("ERROR: A is not array level 2\n");
          else
          {
            bloc_array * tab1 = NULL;
            bloc_value * i1 = NULL;
            if (!bloc_table(v, &tab1) || !bloc_array_item(tab1, 5, &i1) ||
                    bloc_value_type(i1).ndim != 1)
              printf("ERROR: A[5] is not table level 1\n");
            else
            {
              bloc_array * tab2 = NULL;
              bloc_value * i2 = NULL;
              if (!bloc_table(i1, &tab2) || !bloc_array_item(tab2, 2, &i2) ||
                      bloc_value_type(i2).major != ROWTYPE ||
                      bloc_value_type(i2).ndim > 0)
                printf("ERROR: A[5][2] is not tuple\n");
              else
              {
                struct {
                  int64_t     *i;
                  double      *d;
                  bloc_bool   *b;
                  const char  *s;
                } data;
                bloc_row * row = NULL;
                bloc_value * vr = NULL;;
                /* assign tuple items to data members */
                if (bloc_tuple(i2, &row) &&             /* check get tuple  */
                        bloc_tuple_item(row, 0, &vr) && /* check get #0     */
                        !bloc_value_isnull(vr) &&       /* check not null   */
                        bloc_integer(vr, &data.i) &&    /* check get value  */
                        bloc_tuple_item(row, 1, &vr) &&
                        !bloc_value_isnull(vr) &&
                        bloc_numeric(vr, &data.d) &&
                        bloc_tuple_item(row, 2, &vr) &&
                        !bloc_value_isnull(vr) &&
                        bloc_boolean(vr, &data.b) &&
                        bloc_tuple_item(row, 3, &vr) &&
                        !bloc_value_isnull(vr) &&
                        bloc_literal(vr, &data.s))
                {
                  printf("[5][2]   = %d, %g, %d, %s\n",
                         (int)*data.i, *data.d, (int)*data.b, data.s);
                }
                else
                  printf("ERROR: Invalid data\n");
              }
            }
          }
        }
      }
      else
        printf("ERROR: %s\n", bloc_strerror());

      bloc_free_executable(x);
    }
    else
      printf("ERROR: %s\n", bloc_strerror());
  }

  {
    bloc_value * v = bloc_create_literal("ABC");
    if (!bloc_assign_literal(v, "abcdefgh"))
      printf("ERROR: Assign Literal failed\n");
    else
    {
      const char * data = NULL;
      bloc_literal(v, &data);
      printf("LV       = %s\n", data);
    }
    bloc_free_value(v);
  }

  {
    char raw[8] = { 65, 0, 67, 0, 69, 0, 71, 127 };
    bloc_value * v = bloc_create_tabchar(raw+4, 3);
    if (!bloc_assign_tabchar(v, raw, sizeof(raw)))
      printf("ERROR: Assign Bytes failed\n");
    else
    {
      const char * data = NULL;
      unsigned len;
      bloc_tabchar(v, &data, &len);
      printf("XV       =");
      for (unsigned i = 0; i < len; ++i)
        printf(" %c", (data[i] == 0 ? '.' : data[i]));
      printf("\n");
    }
    bloc_free_value(v);
  }

  {
    bloc_value * v = bloc_create_numeric(1.23456);
    bloc_assign_null(v);
    if (!bloc_value_isnull(v))
      printf("ERROR: Assigning a null value failed\n");
    else
    {
      double * d = NULL;
      bloc_numeric(v, &d);
      printf("NV_null  = %p\n", d);
    }
    bloc_free_value(v);
  }

  bloc_free_context(ctx);

  bloc_deinit_plugins();
  return EXIT_SUCCESS;
}

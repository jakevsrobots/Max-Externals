/**
   @file
   bitmush - arbitrary logic expressions for lists of bits
*/

#include "ext.h" // standard Max include, always required
#include "ext_obex.h" // required for new style Max object


////////////////////////// object struct
typedef struct _bitmush
{
  t_object obj;
  char *expr;
  int expr_length;
  void *int_outlet;
  void *bang_outlet;
} t_bitmush;
  
///////////////////////// function prototypes
//// standard set
void *bitmush_new(t_symbol *s, long argc, t_atom *argv);
void bitmush_free(t_bitmush *x);
void bitmush_mush(t_bitmush *x, t_symbol *msg, long argc, t_atom *argv);

//////////////////////// global class pointer variable
static t_class *s_bitmush_class;

t_symbol *ps_list;

void ext_main(void *r)
{
  t_class *c;

  c = class_new("bitmush", (method)bitmush_new, (method)bitmush_free, sizeof(t_bitmush), NULL, A_GIMME, 0);
  class_addmethod(c, (method)bitmush_mush, "list", A_GIMME, 0);
  
  class_register(CLASS_BOX, c);
  
  ps_list = gensym("list");
  
  s_bitmush_class = c;
}

void *bitmush_new(t_symbol *s, long argc, t_atom *argv)
{
  t_bitmush *x = (t_bitmush *)object_alloc(s_bitmush_class);

  if(argc == 1)
    {
      t_symbol *expr_sym = atom_getsym(argv);
      x->expr = malloc(strlen(expr_sym->s_name) * sizeof(x->expr));
      x->expr_length = strlen(expr_sym->s_name);
      strcpy(x->expr, expr_sym->s_name);
    }

  x->int_outlet = intout(x);
  x->bang_outlet = bangout(x);
  
  return x;
}

void bitmush_free(t_bitmush *x)
{
  free(x->expr);
}

void bitmush_mush(t_bitmush *x, t_symbol *msg, long argc, t_atom *argv)
{
  bool *bits = malloc(argc * sizeof(bool));
  
  for(int i=0; i<argc; i++)
    {
      bits[i] = (bool)atom_getlong(argv+i);
    }

  bool mushed_value = 1;
  char last_operator = '\0';
  
  for(int i=0; i<x->expr_length; i++)
    {
      char v = x->expr[i];

      // number indicates an index
      if(v >= 48 && v <= 57)
        {
          bool bit = bits[v-48];

          switch(last_operator)
            {
            case '\0':
              mushed_value = bit;
              break;
            case '^':
              mushed_value ^= bit;
              break;
            case '&':
              mushed_value &= bit;
              break;
            case '|':
              mushed_value |= bit;
              break;
            }
        }
      else
        {
          last_operator = v;
        }
    }

  outlet_int(x->int_outlet, mushed_value);
  
  if(mushed_value == 1)
    {
      outlet_bang(x->bang_outlet);
    }
}

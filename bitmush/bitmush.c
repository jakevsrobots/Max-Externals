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
} t_bitmush;
  
///////////////////////// function prototypes
//// standard set
void *bitmush_new(t_symbol *s, long argc, t_atom *argv);
  
//////////////////////// global class pointer variable
static t_class *s_bitmush_class;

t_symbol *ps_list;

void ext_main(void *r)
{
  t_class *c;

  c = class_new("bitmush", (method)bitmush_new, (method)NULL, sizeof(t_bitmush), NULL, A_GIMME, 0);
  
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
      strcpy(x->expr, expr_sym->s_name);
    }
  
  return x;
}

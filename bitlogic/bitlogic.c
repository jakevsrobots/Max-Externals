/**
   @file
   bitlogic - arbitrary logic expressions for lists of bits
*/

#include "ext.h" // standard Max include, always required
#include "ext_obex.h" // required for new style Max object


////////////////////////// object struct
typedef struct _bitlogic
{
  t_object obj;
} t_bitlogic;
  
///////////////////////// function prototypes
//// standard set
void *bitlogic_new(t_symbol *s, long argc, t_atom *argv);
  
//////////////////////// global class pointer variable
static t_class *s_bitlogic_class;

t_symbol *ps_list;

void ext_main(void *r)
{
  t_class *c;

  c = class_new("bitlogic", (method)bitlogic_new, (method)NULL, sizeof(t_turing), NULL, A_GIMME, 0);
  
  class_register(CLASS_BOX, c);
  
  ps_list = gensym("list");
  
  s_bitlogic_class = c;
}

void *bitlogic_new(t_symbol *s, long argc, t_atom *argv)
{
  t_turing *x = (t_turing *)object_alloc(s_turing_class);
  
  attr_args_process(x, argc, argv);
  
  return x;
}

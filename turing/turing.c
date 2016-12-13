/**
   @file
   turing - music thing turing machine random looping seq clone
*/

#include "ext.h" // standard Max include, always required
#include "ext_obex.h" // required for new style Max object


////////////////////////// object struct
typedef struct _turing
{
  t_object obj;
  int machineSize;
  bool *data;
  int change;
  void *floatval_outlet;
  void *list_outlet;
} t_turing;
  
///////////////////////// function prototypes
//// standard set
void *turing_new(t_symbol *s, long argc, t_atom *argv);
void turing_advance(t_turing *x);
void turing_scramble(t_turing *x);
void turing_output(t_turing *x);
int int_pow(int v, int e);
t_max_err turing_set_machineSize(t_turing *x, void *attr, long ac, t_atom *av);
void turing_free(t_turing *x);
  
//////////////////////// global class pointer variable
static t_class *s_turing_class;

t_symbol *ps_list;

void ext_main(void *r)
{
  t_class *c;

  c = class_new("turing", (method)turing_new, (method)turing_free, sizeof(t_turing), NULL, A_GIMME, 0);
  class_addmethod(c, (method)turing_advance, "bang", 0);
  class_addmethod(c, (method)turing_scramble, "scramble", 0);  

  CLASS_ATTR_LONG(c, "change", 0, t_turing, change);
  CLASS_ATTR_LONG(c, "machineSize", 0, t_turing, machineSize);
  CLASS_ATTR_ACCESSORS(c, "machineSize", NULL, turing_set_machineSize);
  
  class_register(CLASS_BOX, c);
  
  ps_list = gensym("list");
  
  s_turing_class = c;
}

void *turing_new(t_symbol *s, long argc, t_atom *argv)
{
  t_turing *x = (t_turing *)object_alloc(s_turing_class);

  srand(time(NULL));

  x->machineSize = 8;

  x->data = malloc(x->machineSize * sizeof(x->data));
  
  for(int i=0; i<x->machineSize; i++)
    x->data[i] = 0;
  
  x->change = 50;
  x->list_outlet = listout(x);
  x->floatval_outlet = floatout(x);
  
  attr_args_process(x, argc, argv);
  
  return x;
}

void turing_scramble(t_turing *x)
{
  for(int i=0; i<x->machineSize; i++)
    x->data[i] = rand() % 2;

  turing_output(x);  
}

void turing_advance(t_turing *x)
{
  bool newData[x->machineSize];

  for(int i=0; i<x->machineSize; i++)
    {
      if(i==x->machineSize-1)
        newData[0] = x->data[i];
      else
        newData[i+1] = x->data[i];
    }
    
  if(rand() % 100 < x->change)
    newData[0] = !newData[0];

  for(int i=0; i<x->machineSize; i++)
    x->data[i] = newData[i];

  turing_output(x);
}

void turing_output(t_turing *x)
{
  t_atom bit_atom_list[x->machineSize];
  int total = 0;
  
  for(int i=0; i < x->machineSize; i++)
    {
      total += x->data[i] * int_pow(2, (x->machineSize-1)-i);
      atom_setlong(bit_atom_list+i, x->data[i]);
    }
  
  outlet_list(x->list_outlet, ps_list, x->machineSize, bit_atom_list);
  outlet_float(x->floatval_outlet, ((float)total) / pow(2, x->machineSize));
}

t_max_err turing_set_machineSize(t_turing *x, void *attr, long ac, t_atom *av)
{
  if(ac&&av)
    {
      x->machineSize = atom_getlong(av);
      free(x->data);
      x->data = malloc(x->machineSize * sizeof(x->data));
    }
  else
    x->machineSize = 8;
  
  return MAX_ERR_NONE;
}

void turing_free(t_turing *x)
{
  free(x->data);
}

int int_pow(int v, int e)
{
  int result = 1;
  
  while(e)
    {
      result *= v;
      e--;
    }

  return result;
}

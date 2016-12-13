/**
   @file
   elementaryca - elementary cellular automata
*/

#include "ext.h" // standard Max include, always required
#include "ext_obex.h" // required for new style Max object


////////////////////////// object struct
typedef struct _elementaryca
{
  t_object obj;
  uint8_t rule;
  long windowSize;
  int worldSize;
  bool *world;
  bool wrap;
  void *floatval_outlet;
  void *list_outlet;  
} t_elementaryca;
  
///////////////////////// function prototypes
//// standard set
void *elementaryca_new(t_symbol *s, long argc, t_atom *argv);
void elementaryca_free(t_elementaryca *x);
void elementaryca_advance(t_elementaryca *x);
void elementaryca_reset(t_elementaryca *x);
void elementaryca_output(t_elementaryca *x);
int int_pow(int v, int e);
t_max_err elementaryca_set_windowSize(t_elementaryca *x, void *attr, long ac, t_atom *av);
t_max_err elementaryca_set_worldSize(t_elementaryca *x, void *attr, long ac, t_atom *av);
  
//////////////////////// global class pointer variable
static t_class *s_elementaryca_class;

t_symbol *ps_list;

void ext_main(void *r)
{
  t_class *c;

  c = class_new("elementaryca", (method)elementaryca_new, (method)elementaryca_free, sizeof(t_elementaryca), NULL, A_GIMME, 0);
  class_addmethod(c, (method)elementaryca_advance, "bang", 0);
  class_addmethod(c, (method)elementaryca_reset, "reset", 0);  

  CLASS_ATTR_LONG(c, "rule", 0, t_elementaryca, rule);
  CLASS_ATTR_LONG(c, "wrap", 0, t_elementaryca, wrap);
  CLASS_ATTR_LONG(c, "windowSize", 0, t_elementaryca, windowSize);
  CLASS_ATTR_ACCESSORS(c, "windowSize", NULL, elementaryca_set_windowSize);
  CLASS_ATTR_LONG(c, "worldSize", 0, t_elementaryca, worldSize);  
  CLASS_ATTR_ACCESSORS(c, "worldSize", NULL, elementaryca_set_worldSize);
  
  class_register(CLASS_BOX, c);

  ps_list = gensym("list");
  
  s_elementaryca_class = c;
}

void elementaryca_advance(t_elementaryca *x)
{
  bool newWorld[x->worldSize];
  
  for(int i=0; i<x->worldSize; i++)
    {
      bool left, middle, right;

      if(i == 0)
        {
          if(x->wrap)
            left = x->world[x->worldSize-1];
          else
            left = 0;
        }
      else
        {
          left = x->world[i-1];
        }

      middle = x->world[i];
      
      if(i == x->worldSize-1)
        {
          if(x->wrap)
            right = x->world[0];
          else
            right = 0;
        }
      else
        {
          right = x->world[i+1];
        }

      uint8_t neighborhood = (left * 4) + (middle * 2) + (right * 1);

      newWorld[i] = x->rule & (1 << neighborhood);
    }

  for(int i=0; i<x->worldSize; i++)
    x->world[i] = newWorld[i];

  elementaryca_output(x);
}

void elementaryca_output(t_elementaryca *x)
{
  int snapshotTotal = 0;
  int worldOffset = (x->worldSize-x->windowSize)/2;
  t_atom subset_list[x->windowSize];
  
  for(int i=0; i < x->windowSize; i++)
    {
      snapshotTotal += x->world[worldOffset+i] * int_pow(2, (x->windowSize-1)-i);
      atom_setlong(subset_list+i, x->world[worldOffset+i]);
    }

  outlet_list(x->list_outlet, ps_list, x->windowSize, subset_list);  
  outlet_float(x->floatval_outlet, ((float)snapshotTotal) / pow(2, x->windowSize));
}

void elementaryca_reset(t_elementaryca *x)
{
  for(int i=0; i<x->worldSize; i++)
    x->world[i] = 0;
  
  x->world[x->worldSize/2] = 1;
  
  elementaryca_output(x);
}

t_max_err elementaryca_set_windowSize(t_elementaryca *x, void *attr, long ac, t_atom *av)
{
  if(ac&&av)
    {
      long newWindowSize = atom_getlong(av);
      
      if(newWindowSize > x->worldSize)
        newWindowSize = x->worldSize;
      
      x->windowSize = newWindowSize;
    }
  else
    x->windowSize = 8;
  
  return MAX_ERR_NONE;
}

t_max_err elementaryca_set_worldSize(t_elementaryca *x, void *attr, long ac, t_atom *av)
{
  if(ac&&av)
    {
      x->worldSize = atom_getlong(av);

      free(x->world);
      x->world = malloc(x->worldSize * sizeof(x->world));
      elementaryca_reset(x);
      
      if(x->worldSize < x->windowSize)
        x->windowSize = x->worldSize;
    }
  else
    x->worldSize = 16;
  
  return MAX_ERR_NONE;
}

void *elementaryca_new(t_symbol *s, long argc, t_atom *argv)
{
  t_elementaryca *x = (t_elementaryca *)object_alloc(s_elementaryca_class);

  x->list_outlet = listout(x);  
  x->floatval_outlet = floatout(x);
  
  x->rule = 30;
  x->worldSize = 16;  
  x->windowSize = 8;
  x->wrap = true;

  attr_args_process(x, argc, argv);

  x->world = malloc(x->worldSize * sizeof(x->world));
  
  for(int i=0; i<x->worldSize; i++)
    x->world[i] = 0;
  
  x->world[x->worldSize/2] = 1;
  
  return x;
}

void elementaryca_free(t_elementaryca *x)
{
  free(x->world);
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


#include <stdlib.h>
#include <assert.h>
#include "coroutine.h"

int coroutine_free(coroutine__s *co)
{
  assert(co       != NULL);
  assert(co->base != NULL);
  assert(co->size >  0);
  
  free(co->base);
  return 0;
}

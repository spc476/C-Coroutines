
#include <stdlib.h>
#include <errno.h>
#include <assert.h>
#include <sys/mman.h>
#include "coroutine.h"

int coroutine_free(coroutine__s *co)
{
  assert(co       != NULL);
  assert(co->base != MAP_FAILED);
  assert(co->base != NULL);
  assert(co->size >  0);
  
  free(co->base);
  return 0;
}

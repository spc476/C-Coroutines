
#include <stdlib.h>
#include <assert.h>
#include "coroutine.h"

extern void coroutine_init(
                            coroutine__s *,
                            uintptr_t (*)(coroutine__s *,uintptr_t),
                            void         *
                          );

int coroutine_create(
        coroutine__s  **pco,
        size_t          stsize,
        uintptr_t     (*fun)(coroutine__s *,uintptr_t)
)
{
  char         *blob;
  coroutine__s *co;
  
  assert(pco != NULL);
  assert(fun);
  
  if (stsize == 0)
    stsize = 8192;
  
  if ((blob = malloc(stsize)) == NULL)
    return -1;
  
  co       = (coroutine__s *)&blob[stsize - sizeof(coroutine__s)];
  co->base = blob;
  co->size = stsize;
  
  coroutine_init(co,fun,co);
  *pco = co;
  return 0;
}


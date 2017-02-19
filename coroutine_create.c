
#define _GNU_SOURCE

#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <assert.h>
#include <syslog.h>
#include "coroutine.h"

extern void coroutine_init(coroutine__s *,uintptr_t (*)(coroutine__s *,uintptr_t));

int coroutine_create(
        coroutine__s  **pco,
        size_t          stsize,
        uintptr_t     (*fun)(coroutine__s *,uintptr_t)
)
{
  coroutine__s *co;
  
  assert(pco != NULL);
  assert(fun);
  
  if (stsize == 0)
    stsize = 4192;
  
  co = calloc(1,stsize);
  if (co == NULL)
    return errno;
  
  co->base = co;
  co->size = stsize;

  syslog(LOG_DEBUG,"create: stack=%p",co->base);

  coroutine_init(co,fun);
  *pco = co;
  return 0;
}


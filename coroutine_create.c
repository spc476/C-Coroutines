
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
  char         *blob;
  coroutine__s *co;
  
  assert(pco != NULL);
  assert(fun);
  
  if (stsize == 0)
    stsize = 4192;
  
  blob = calloc(1,stsize);
  if (blob == NULL)
    return errno;
  
  co       = (coroutine__s *)&blob[stsize - sizeof(coroutine__s)];
  co->base = blob;
  co->size = stsize - sizeof(coroutine__s);

  syslog(LOG_DEBUG,"create: stack=%p",co->base);

  coroutine_init(co,fun);
  *pco = co;
  return 0;
}


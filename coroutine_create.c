
#define _GNU_SOURCE

#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <assert.h>
#include <syslog.h>
#include "coroutine.h"

extern void coroutine_init(coroutine__s *,uintptr_t (*)(coroutine__s *,uintptr_t),uintptr_t);

int coroutine_create(
        coroutine__s  *co,
        size_t         stsize,
        uintptr_t    (*fun)(coroutine__s *,uintptr_t),
        uintptr_t      param
)
{
  assert(co != NULL);
  assert(fun);
    
  co->sp   = NULL;
  co->size = stsize == 0 ? 65536u : stsize;
  co->base = malloc(stsize);
  if (co->base == NULL)
    return errno;
  
  coroutine_init(co,fun,param);
  return 0;
}



#define _GNU_SOURCE

#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <assert.h>
#include <syslog.h>
#include "coroutine.h"

extern uintptr_t coroutine_init(coroutine__s *,uintptr_t,uintptr_t (*)(coroutine__s *,uintptr_t));

uintptr_t coroutine_create(
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
  co->base = calloc(1,stsize);
  if (co->base == NULL)
    abort();
  
  return coroutine_init(co,param,fun);
}


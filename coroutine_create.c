
#define _GNU_SOURCE

#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <assert.h>
#include <syslog.h>
#include "coroutine.h"

extern void coroutine_init(coroutine__s *,uintptr_t (*)(coroutine__s *,uintptr_t));

int coroutine_create(
        coroutine__s  *co,
        size_t         stsize,
        uintptr_t    (*fun)(coroutine__s *,uintptr_t)
)
{
  assert(co != NULL);
  assert(fun);
  
  if (stsize == 0)
    stsize = 65536u;
    
  co->base = NULL;
  co->size = 0;
  co->sp   = NULL;
  co->bp   = NULL;
  co->ysp  = NULL;
  co->ybp  = NULL;
  
  co->size = stsize;
  co->base = calloc(1,stsize);
  if (co->base == NULL)
    return errno;
  
  syslog(LOG_DEBUG,"create: stack=%p",co->base);
  coroutine_init(co,fun);
  return 0;
}


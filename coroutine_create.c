
#define _GNU_SOURCE

#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <assert.h>
#include <syslog.h>
#include <unistd.h>
#include <sys/mman.h>
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
  void         *mem;
  char         *blob;
  coroutine__s *co;
  
  assert(pco != NULL);
  assert(fun);
  
  if (stsize == 0)
    stsize = getpagesize();
    
  mem = mmap(0,stsize,PROT_READ | PROT_WRITE,MAP_SHARED | MAP_ANONYMOUS | MAP_GROWSDOWN,-1,0);
  if (mem == MAP_FAILED)
    return errno;
    
  blob     = mem;
  co       = (coroutine__s *)&blob[stsize - sizeof(coroutine__s)];
  co->base = blob;
  co->size = stsize;
  
  syslog(LOG_DEBUG,"create: stack=%p",co->base);
  
  coroutine_init(co,fun,co);
  *pco = co;
  return 0;
}


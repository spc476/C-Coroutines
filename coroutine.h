
#ifndef I_5A22037D_5F90_5456_A562_661022F242BF
#define I_5A22037D_5F90_5456_A562_661022F242BF

#include <stddef.h>
#include <stdint.h>

#if defined(__i386)
#  define coroutine_init	coroutine_init32
#  define coroutine_yield	coroutine_yield32
#elif defined(__x86)
#  define coroutine_init	coroutine_init32
#  define coroutine_yield	coroutine_yield32
#elif defined(__x86_64)
#  define coroutine_init	coroutine_init64
#  define coroutine_yield	coroutine_yield64
#endif

typedef struct
{
  void   *csp;
  void   *base;
  size_t  size;
} coroutine__s;


extern int          coroutine_create(coroutine__s **,size_t,uintptr_t (*fun)(coroutine__s *,uintptr_t));
extern uintptr_t    coroutine_yield (coroutine__s *,uintptr_t);
extern int          coroutine_free  (coroutine__s *);

#endif


#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <assert.h>

#ifdef __APPLE__
#  error Mac OS-X does not support clock_gettime()
#endif

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

extern void      coroutine_init (void *,uintptr_t (*)(void *,uintptr_t),void *);
extern uintptr_t coroutine_yield(void *,uintptr_t);

/*************************************************************************/

static void *create_co(uintptr_t (*fun)(void *,uintptr_t))
{
  char *blob = malloc(4192);
  if (blob != NULL)
    coroutine_init(blob,fun,&blob[4192]);
  return blob;
}

/*************************************************************************/

static uintptr_t subtask(void *self,uintptr_t data)
{
  data = coroutine_yield(self,data);
  data = coroutine_yield(self,data);
  data = coroutine_yield(self,data);
  data = coroutine_yield(self,data);
  return data;
}

/*************************************************************************/

static void runtest(struct timespec *result,void *cos[],size_t max)
{
  struct timespec start;
  struct timespec end;
  
  clock_gettime(CLOCK_MONOTONIC,&start);
  for (size_t i = 0 ; i < max ; i++)
    coroutine_yield(cos[i],i);
  clock_gettime(CLOCK_MONOTONIC,&end);
  
  if (end.tv_nsec < start.tv_nsec)
  {
    end.tv_nsec += 1000000000L;
    end.tv_sec--;
  }
  
  result->tv_nsec = end.tv_nsec - start.tv_nsec;
  result->tv_sec  = end.tv_sec  - start.tv_sec;
}

/*************************************************************************/

int main(int argc,char *argv[])
{
  struct timespec   total;
  void            **co;
  size_t            max;
  
  if (argc == 1)
    max = 1000;
  else
    max = strtoul(argv[1],NULL,10);
  
  co = calloc(max,sizeof(void *));
  assert(co);
  
  for (size_t i = 0 ; i < max ; i++)
  {
    co[i] = create_co(subtask);
    assert(co[i] != NULL);
  }
  
  runtest(&total,co,max);
  printf("Initial coroutine_yield() %ld %ld %ld\n",(long)total.tv_sec,(long)total.tv_nsec,(long)(total.tv_nsec / max)/2);
  
  runtest(&total,co,max);
  printf("Second  coroutine_yield() %ld %ld %ld\n",(long)total.tv_sec,(long)total.tv_nsec,(long)(total.tv_nsec / max)/2);
 
  runtest(&total,co,max);
  printf("Third   coroutine_yield() %ld %ld %ld\n",(long)total.tv_sec,(long)total.tv_nsec,(long)(total.tv_nsec / max)/2);
 
  runtest(&total,co,max);
  printf("Fourth  coroutine_yield() %ld %ld %ld\n",(long)total.tv_sec,(long)total.tv_nsec,(long)(total.tv_nsec / max)/2);
  
  runtest(&total,co,max);
  printf("Final   coroutine_yield() %ld %ld %ld\n",(long)total.tv_sec,(long)total.tv_nsec,(long)(total.tv_nsec / max)/2);
  
  for (size_t i = 0 ; i < max ; i++)
    free(co[i]);
    
  free(co);
  return 0;
}

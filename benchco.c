#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "coroutine.h"

#ifdef __APPLE__
#  error Mac OS-X does not support clock_gettime()
#endif

static uintptr_t subtask(coroutine__s *self,uintptr_t data)
{
  while(data--)
    data = coroutine_yield(self,data);
  return 0;
}

/*************************************************************************/

static void runtest(struct timespec *result,coroutine__s *co,uintptr_t data)
{
  struct timespec start;
  struct timespec end;
  
  clock_gettime(CLOCK_MONOTONIC,&start);
  while(data--)
    data = coroutine_yield(co,data);
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
  struct timespec     total;
  coroutine__s       *co;  
  uintptr_t           max;
  unsigned long long  bav;
  long                res;
  
  if (argc == 1)
    max = 1000000uL;
  else
    max = strtoul(argv[1],NULL,10);
  
  coroutine_create(&co,0,subtask);
  runtest(&total,co,max);
  bav = (unsigned long long)total.tv_sec * 1000000000uLL
      + (unsigned long long)total.tv_nsec;
  res = (unsigned long)(bav / (unsigned long long)max);
  res /= 2; /* adjust for both coroutines adjusting count */
  
  printf("Initial coroutine_yield() %ld %ld %ld\n",(long)total.tv_sec,(long)total.tv_nsec,res);
  
  coroutine_free(co);
  return 0;
}

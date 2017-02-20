
#include <inttypes.h>
#include <stdbool.h>
#include <stdio.h>
#include <cgilib6/crashreport.h>
#include <syslog.h>
#include "coroutine.h"

/**************************************************************************/

static uintptr_t foo(coroutine__s *self,uintptr_t d)
{
  d = coroutine_yield(self,d);
  return d;
}

static uintptr_t routine1(coroutine__s *self,uintptr_t d)
{
  while(true)
  {
    printf("routine1=%" PRIuPTR "\n",d);
    d = foo(self,d + 100);
  }
  
  return 0;
}

/**************************************************************************/

static uintptr_t routine2(coroutine__s *self,uintptr_t d)
{
  while(true)
  {
    printf("routine2=%" PRIuPTR "\n",d);
    d = coroutine_yield(self,d + 1);
  }
  
  return 0;
}

/**************************************************************************/

static uintptr_t routine3(coroutine__s *self,uintptr_t d)
{
  while(true)
  {
    printf("routine3=%" PRIuPTR "\n",d);
    d = coroutine_yield(self,d + 2);
  }
  
  return 0;
}

/**************************************************************************/

static uintptr_t test(coroutine__s *self,uintptr_t d)
{
  syslog(LOG_DEBUG,"test HELLO");
  printf("Hello %" PRIuPTR "\n",d);
  d = coroutine_yield(self,d + 10);
  syslog(LOG_DEBUG,"test YIELD");
  printf("Goodbye %" PRIuPTR "\n",d);
  return d + 10;
}

/**************************************************************************/

int main(int argc,char **argv)
{
  crashreport(SIGSEGV);
  crashreport(SIGILL);
  crashreport(SIGABRT);
  
  (void)argv;
  syslog(LOG_INFO,"STARTING");
  
  if (true)
  {
    coroutine__s *co;
    uintptr_t    r;
    
    syslog(LOG_DEBUG,"argc=%p",(void *)&argc);
    coroutine_create(&co,0,test);
    syslog(LOG_DEBUG,"done with coroutine_create()");
    r = coroutine_yield(co,0);
    printf("test1=%" PRIuPTR "\n",r);
    r = coroutine_yield(co,r);
    printf("test2=%" PRIuPTR "\n",r);
    syslog(LOG_DEBUG,"argc=%p",(void *)&argc);
    
    for (int i = 0 ; i < 10 ; i++)
    {
      r = coroutine_yield(co,r);
      printf("test3=%" PRIuPTR "\n",r);
    }
    coroutine_free(co);
  }
  
  printf("---------\n");
  
  if (true)
  {
    coroutine__s *co1;
    coroutine__s *co2;
    coroutine__s *co3;
    
    coroutine_create(&co1,0,routine1);
    coroutine_create(&co2,0,routine2);
    coroutine_create(&co3,0,routine3);
    
    for (int i = 0 , r = 0 ; i < 10 ; i++)
    {
      r = coroutine_yield(co1,r);
      r = coroutine_yield(co2,r + 200);
      r = coroutine_yield(co3,r + 300);
    }
    
    coroutine_free(co3);
    coroutine_free(co2);
    coroutine_free(co1);
  }
  
  return 0;
}

/**************************************************************************/

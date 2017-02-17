
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
    printf("routine1=%lu\n",(unsigned long)d);
    d = foo(self,d + 100);
  }
}

/**************************************************************************/

static uintptr_t routine2(coroutine__s *self,uintptr_t d)
{
  while(true)
  {
    printf("routine2=%lu\n",(unsigned long)d);
    d = coroutine_yield(self,d + 1);
  }
}

/**************************************************************************/

static uintptr_t routine3(coroutine__s *self,uintptr_t d)
{
  while(true)
  {
    printf("routine3=%lu\n",(unsigned long)d);
    d = coroutine_yield(self,d + 2);
  }
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

int main(void)
{
  crashreport(SIGSEGV);
  crashreport(SIGILL);
  crashreport(SIGABRT);
  
  syslog(LOG_INFO,"STARTING");
  
  if (false)
  {
    coroutine__s co;
    uintptr_t    r;
    
    r = coroutine_create(&co,0,test,0);
    syslog(LOG_DEBUG,"done with coroutine_create()");
    printf("test1=%" PRIuPTR "\n",r);
    r = coroutine_resume(&co,r);
    printf("test2=%" PRIuPTR "\n",r);
  }
  
  printf("In the middle\n");
  
  if (true)
  {
    coroutine__s co1;
    coroutine__s co2;
    coroutine__s co3;
    
//    coroutine_create(&co1,0,routine1,   0);
    coroutine_create(&co2,0,routine2,1000);
    coroutine_create(&co3,0,routine3,2000);
    
    for (int i = 0 , r = 0 ; i < 10 ; i++)
    {
//      r = coroutine_resume(&co1,r);
      r = coroutine_resume(&co2,r);
//      r = coroutine_resume(&co3,r);
    }
    
//    coroutine_free(&co3);
    coroutine_free(&co2);
    coroutine_free(&co1);
  }
  
  return 0;
}

/**************************************************************************/

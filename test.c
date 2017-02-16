
#include <stdbool.h>
#include <stdio.h>
#include <cgilib6/crashreport.h>
#include "coroutine.h"

coroutine__s g_co;

/**************************************************************************/

static uintptr_t foo(coroutine__s *self,uintptr_t d)
{
  d = coroutine_yield(&g_co,self,d);
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
    d = coroutine_yield(&g_co,self,d + 1);
  }
}

/**************************************************************************/

static uintptr_t routine3(coroutine__s *self,uintptr_t d)
{
  while(true)
  {
    printf("routine3=%lu\n",(unsigned long)d);
    d = coroutine_yield(&g_co,self,d + 2);
  }
}

/**************************************************************************/

int main(void)
{
  coroutine__s co1;
  coroutine__s co2;
  coroutine__s co3;
  
  crashreport(SIGSEGV);
  crashreport(SIGILL);
  
  coroutine_self(&g_co);
  coroutine_create(&co1,0,routine1,   0);
  coroutine_create(&co2,0,routine2,1000);
  coroutine_create(&co3,0,routine3,2000);
  
  for (int i = 0 , r = 0 ; i < 10 ; i++)
  {
    r = coroutine_yield(&co1,&g_co,r);
    r = coroutine_yield(&co2,&g_co,r);
    r = coroutine_yield(&co3,&g_co,r);
  }
  
  coroutine_free(&co3);
  coroutine_free(&co2);
  coroutine_free(&co1);
  
  return 0;
}

/**************************************************************************/

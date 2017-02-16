
#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <cgilib6/crashreport.h>

extern void *swapstack(void *);
extern void *getstack(void);

void bar(void *s,size_t size)
{
  FILE *fpout = fopen("/tmp/dump.bin","wb");
  if (fpout)
  {
    fwrite(s,1,size,fpout);
    fclose(fpout);
  }
}

int main(void)
{
  char *newstack;
  char *oldstack;
  char *usestack;
  
  crashreport(SIGILL);
  crashreport(SIGSEGV);
  crashreport(SIGFPE);
  
  printf("esp=%p\n",getstack());
  newstack = malloc(65536u);
  oldstack = swapstack(newstack + 65520u);
  printf("old=%p new=%p\n",oldstack,newstack + 65520u);
  bar(newstack,65536u);
  usestack = swapstack(oldstack);
  printf("esp=%p\n",getstack());
  printf("old=%p new=%p use=%p\n",oldstack,newstack + 65520u,usestack);
  return 0;
}

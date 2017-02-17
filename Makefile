
CC       = gcc -std=c99 -Wall -Wextra -pedantic
CFLAGS   = -g
LDFLAGS  = -g
LDLIBS   = -lcgi6
ASM      = nasm -f elf
ASMFLAGS = -g 
AR       = ar rscu

%.a :
	$(AR) $@ $?

%.o : %.asm
	$(ASM) $(ASMFLAGS) -l $(*F).list -o $@ $<

.PHONY  : all clean
all     : test sstest
test    : test.o libco.a
sstest  : sstest.o swapstack.o
libco.a : coroutine_create.o coroutine_free.o coroutine_yield.o
coroutine_yield.o : coroutine_yield.asm
swapstack.o : swapstack.asm

clean:
	$(RM) *.o *~ *.a test core.* sstest

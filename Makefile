
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
all     : test
test    : test.o libco.a
libco.a : coroutine_create.o coroutine_free.o coroutine_yield.o

coroutine_yield.o  : coroutine_yield.asm
coroutine_create.o : coroutine.h
coroutine_free.o   : coroutine.h
test.o             : coroutine.h

clean:
	$(RM) *.o *~ *.a test core.* *.list

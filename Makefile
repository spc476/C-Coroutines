
CC       = c99
CFLAGS   = -g
LDFLAGS  = -g
LDLIBS   =
ASM      = nasm -f elf
ASMFLAGS = -g
AR       = ar rscu

%.a :
	$(AR) $@ $?

%.o : %.asm
	$(ASM) $(ASMFLAGS) -l $(*F).list -o $@ $<

.PHONY  : all clean
all     : test iter
test    : test.o libco.a
iter    : iter.o libco.a
libco.a : coroutine_create.o coroutine_free.o coroutine_yield.o

coroutine_yield.o  : coroutine_yield.asm
coroutine_create.o : coroutine.h
coroutine_free.o   : coroutine.h
test.o             : coroutine.h
iter.o             : coroutine.h

clean:
	$(RM) test iter *.o *~ *.a core.* *.list

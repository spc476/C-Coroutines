
CC       = c99
CFLAGS   = -g
LDFLAGS  = -g
LDLIBS   =
ASM      = nasm -f elf32
ASMFLAGS = -g
AR       = ar rscu

%.a :
	$(AR) $@ $?

%.o : %.asm
	$(ASM) $(ASMFLAGS) -l $(*F).list -o $@ $<

.PHONY : all clean

all  : test iter
clean:
	$(RM) test iter *.o *~ *.a core.* *.list
	
test    : test.o libco.a
iter    : iter.o libco.a
libco.a : coroutine_create.o coroutine_free.o	\
		coroutine_yield-x86-32.o 	\
		coroutine_yield-x86-64.o

coroutine_yield-x86-64.o : ASM = nasm -f elf64
coroutine_create.o       : coroutine.h
coroutine_free.o         : coroutine.h
test.o                   : coroutine.h
iter.o                   : coroutine.h

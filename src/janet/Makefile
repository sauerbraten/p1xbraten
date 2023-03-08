override CFLAGS+=-O0 -g3 -Wall -Werror -fPIC

OBJS= janet.o

libjanet.a: $(OBJS)
	$(AR) rcs $@ $(OBJS)

default: libjanet.a

clean:
	-$(RM) libjanet.a $(OBJS)

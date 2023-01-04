CPU = m88k
CFLAGS=-I../machine -I../h -I../../sys -Dopus -DINET -DINKERNEL -DKERNEL -D$(CPU) -DCMMU $(DASHO)
LIBDIR = /usr/src/uts/m88k/cf/m88k
LIBNAME = $(LIBDIR)/lib.bsd
FILES = \
	$(LIBNAME)(if_ec.o)

$(LIBNAME): $(FILES)
	$(AR) ruv $(LIBNAME) *.o
	rm -f *.o

.c.a:
	$(CC) -c $(CFLAGS) $<;

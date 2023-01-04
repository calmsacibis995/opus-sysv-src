CPU = m88k
CFLAGS=-I../machine -I../h -I../../sys -Dopus -DINET -DINKERNEL -DKERNEL -D$(CPU) -DCMMU $(DASHO)
LIBDIR = /usr/src/uts/m88k/cf/m88k
LIBNAME = $(LIBDIR)/lib.bsd
FILES = \
	$(LIBNAME)(af.o) \
	$(LIBNAME)(if.o) \
	$(LIBNAME)(if_loop.o) \
	$(LIBNAME)(if_sl.o) \
	$(LIBNAME)(raw_cb.o) \
	$(LIBNAME)(raw_usrreq.o) \
	$(LIBNAME)(route.o)

$(LIBNAME): $(FILES)
	$(AR) ruv $(LIBNAME) *.o
	rm -f *.o

.c.a:
	$(CC) -c $(CFLAGS) $<;

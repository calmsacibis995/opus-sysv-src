h37025
s 00001/00001/00012
d D 1.2 90/04/24 11:55:02 root 2 1
c added DASHO
e
s 00013/00000/00000
d D 1.1 90/03/06 12:32:54 root 1 0
c date and time created 90/03/06 12:32:54 by root
e
u
U
t
T
I 1
CPU = m88k
D 2
CFLAGS=-I../machine -I../h -I../../sys -Dopus -DINET -DINKERNEL -DKERNEL -D$(CPU) -DCMMU
E 2
I 2
CFLAGS=-I../machine -I../h -I../../sys -Dopus -DINET -DINKERNEL -DKERNEL -D$(CPU) -DCMMU $(DASHO)
E 2
LIBDIR = /usr/src/uts/m88k/cf/m88k
LIBNAME = $(LIBDIR)/lib.bsd
FILES = \
	$(LIBNAME)(if_ec.o)

$(LIBNAME): $(FILES)
	$(AR) ruv $(LIBNAME) *.o
	rm -f *.o

.c.a:
	$(CC) -c $(CFLAGS) $<;
E 1

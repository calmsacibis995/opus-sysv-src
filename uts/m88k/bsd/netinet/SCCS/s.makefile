h65274
s 00001/00001/00027
d D 1.2 90/04/24 11:53:50 root 2 1
c added DASHO
e
s 00028/00000/00000
d D 1.1 90/03/06 12:34:03 root 1 0
c date and time created 90/03/06 12:34:03 by root
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
	$(LIBNAME)(if_ether.o) \
	$(LIBNAME)(in.o) \
	$(LIBNAME)(in_cksum.o) \
	$(LIBNAME)(in_pcb.o) \
	$(LIBNAME)(in_proto.o) \
	$(LIBNAME)(ip_icmp.o) \
	$(LIBNAME)(ip_input.o) \
	$(LIBNAME)(ip_output.o) \
	$(LIBNAME)(raw_ip.o) \
	$(LIBNAME)(tcp_debug.o) \
	$(LIBNAME)(tcp_input.o) \
	$(LIBNAME)(tcp_output.o) \
	$(LIBNAME)(tcp_subr.o) \
	$(LIBNAME)(tcp_timer.o) \
	$(LIBNAME)(tcp_usrreq.o) \
	$(LIBNAME)(udp_usrreq.o)

$(LIBNAME): $(FILES)
	$(AR) ruv $(LIBNAME) *.o
	rm -f *.o

.c.a:
	$(CC) -c $(CFLAGS) $<;
E 1

CPU = m88k
CFLAGS=-I../machine -I../h -I../../sys -Dopus -DINET -DINKERNEL -DKERNEL -D$(CPU) -DCMMU $(DASHO)
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

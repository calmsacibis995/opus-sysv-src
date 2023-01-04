h10984
s 00001/00001/00038
d D 1.2 90/04/24 11:52:50 root 2 1
c added DASHO
e
s 00039/00000/00000
d D 1.1 90/03/06 12:39:10 root 1 0
c date and time created 90/03/06 12:39:10 by root
e
u
U
t
T
I 1
CPU = m88k
D 2
CFLAGS=-I../machine -I../h -I../../sys -Dopus -DSVR3 -DINKERNEL -DKERNEL -D$(CPU) -DCMMU
E 2
I 2
CFLAGS=-I../machine -I../h -I../../sys -Dopus -DSVR3 -DINKERNEL -DKERNEL -D$(CPU) -DCMMU $(DASHO)
E 2
LIBDIR = /usr/src/uts/m88k/cf/m88k
LIBNAME = $(LIBDIR)/lib.bsd
FILES = \
	$(LIBNAME)(u_domain.o) \
	$(LIBNAME)(u_mbuf.o) \
	$(LIBNAME)(u_proto.o) \
	$(LIBNAME)(u_socket.o) \
	$(LIBNAME)(u_socket2.o) \
	$(LIBNAME)(u_syscall.o) \
	$(LIBNAME)(u_usrreq.o) \
	$(LIBNAME)(sys_misc.o) \
	$(LIBNAME)(sys_select.o) \
	$(LIBNAME)(sys_socket.o) \
	$(LIBNAME)(kern_subr.o) \
	$(LIBNAME)(sysbsd.o) \
	$(LIBNAME)(tty.o) \
	$(LIBNAME)(bsd_kudp.o)

all:	$(LIBNAME)

$(LIBNAME): $(FILES)
	$(AR) ruv $(LIBNAME) *.o
	rm -f *.o

.c.a:
	$(CC) -c $(CFLAGS) $<;

lint:
	lint -a -n -p -u -x -Isys -I../sys -Dopus -DINKERNEL -DKERNEL \
	kern_subr.c \
	u_domain.c \
	u_mbuf.c \
	u_proto.c \
	u_socket.c \
	u_socket2.c \
	u_syscall.c \
	u_usrreq.c
E 1

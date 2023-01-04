h54496
s 00004/00004/00015
d D 1.2 90/04/24 11:34:46 root 2 1
c pass DASHO option to lower level make files.
e
s 00019/00000/00000
d D 1.1 90/03/06 12:40:04 root 1 0
c date and time created 90/03/06 12:40:04 by root
e
u
U
t
T
I 1
LIBDIR = /usr/src/uts/m88k/cf/m88k
LIBNAME = $(LIBDIR)/lib.bsd
DASHG = 
DASHO =
FRC =
CPU = m88k
CC=mcc
AR=mar
all:
D 2
	cd sys; $(MAKE) CPU=$(CPU) CC=$(CC) AR=$(AR)
	cd netinet; $(MAKE) CPU=$(CPU) CC=$(CC) AR=$(AR)
	cd net; $(MAKE) CPU=$(CPU) CC=$(CC) AR=$(AR)
	cd if; $(MAKE) CPU=$(CPU) CC=$(CC) AR=$(AR)
E 2
I 2
	cd sys; $(MAKE) CPU=$(CPU) CC=$(CC) AR=$(AR) DASHO=$(DASHO)
	cd netinet; $(MAKE) CPU=$(CPU) CC=$(CC) AR=$(AR) DASHO=$(DASHO)
	cd net; $(MAKE) CPU=$(CPU) CC=$(CC) AR=$(AR) DASHO=$(DASHO)
	cd if; $(MAKE) CPU=$(CPU) CC=$(CC) AR=$(AR) DASHO=$(DASHO)
E 2

clean:
	rm -f $(LIBDIR)/lib.bsd

clobber:
	rm -f $(LIBDIR)/lib.bsd
E 1

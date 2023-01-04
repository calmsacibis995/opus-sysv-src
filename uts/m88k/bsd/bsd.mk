LIBDIR = /usr/src/uts/m88k/cf/m88k
LIBNAME = $(LIBDIR)/lib.bsd
DASHG = 
DASHO =
FRC =
CPU = m88k
CC=mcc
AR=mar
all:
	cd sys; $(MAKE) CPU=$(CPU) CC=$(CC) AR=$(AR) DASHO=$(DASHO)
	cd netinet; $(MAKE) CPU=$(CPU) CC=$(CC) AR=$(AR) DASHO=$(DASHO)
	cd net; $(MAKE) CPU=$(CPU) CC=$(CC) AR=$(AR) DASHO=$(DASHO)
	cd if; $(MAKE) CPU=$(CPU) CC=$(CC) AR=$(AR) DASHO=$(DASHO)

clean:
	rm -f $(LIBDIR)/lib.bsd

clobber:
	rm -f $(LIBDIR)/lib.bsd

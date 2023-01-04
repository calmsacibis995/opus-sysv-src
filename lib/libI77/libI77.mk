#	Copyright (c) 1984 AT&T
#	  All Rights Reserved

#	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T
#	The copyright notice above does not evidence any
#	actual or intended publication of such source code.

#ident	"@(#)libI77:libI77.mk	1.13"
# F77 I/O library (libI77.a) makefile

CC = cc
PCC = /usr/bin/cc
AR = ar
RM=/bin/rm
STRIP = strip
MAKE=make
CFLAGS = -O
INS = install
LIB=$(ROOT)/usr/lib
LIBP=$(ROOT)/usr/lib/libp
LIBPCC=$(ROOT)/usr/lib/libpcc
VARIANT = I77
LIBNAME = lib$(VARIANT).a


OBJ =	$(LIBNAME)(Version.o) \
	$(LIBNAME)(backspace.o) \
	$(LIBNAME)(dfe.o) \
	$(LIBNAME)(due.o) \
	$(LIBNAME)(iio.o) \
	$(LIBNAME)(inquire.o) \
	$(LIBNAME)(rewind.o) \
	$(LIBNAME)(rsfe.o) \
	$(LIBNAME)(rdfmt.o) \
	$(LIBNAME)(sue.o) \
	$(LIBNAME)(uio.o) \
	$(LIBNAME)(wsfe.o) \
	$(LIBNAME)(sfe.o) \
	$(LIBNAME)(fmt.o) \
	$(LIBNAME)(nio.o) \
	$(LIBNAME)(lio.o) \
	$(LIBNAME)(lread.o) \
	$(LIBNAME)(open.o) \
	$(LIBNAME)(close.o) \
	$(LIBNAME)(util.o) \
	$(LIBNAME)(endfile.o) \
	$(LIBNAME)(wrtfmt.o) \
	$(LIBNAME)(err.o) \
	$(LIBNAME)(fmtlib.o) \
	$(LIBNAME)(ecvt.o) \
	$(LIBNAME)(ltostr.o)
.c.o:;

all: $(LIBNAME) profiledlib

$(LIBNAME):	$(OBJ)
		if clipper && [ ! -r /bin/ghs ] ; then \
			$(PCC) -W0,-C1 -c $(CFLAGS) $(?:.o=.c) ; \
		else \
			$(CC) -c $(CFLAGS) $(?:.o=.c) ; \
		fi
		$(AR) r $(LIBNAME) $?
		$(RM) $?

$(OBJ): lio.h fio.h fmt.h

.c.a:;

profiledlib:
	$(MAKE) -f libI77.mk -e libp.$(LIBNAME) \
	LIBNAME=libp.$(LIBNAME) CFLAGS="-p $(CFLAGS)"

install: all
	cp $(LIBNAME) $(LIB)/$(LIBNAME)
	if clipper && [ ! -r /bin/ghs ] ; then \
		if [ ! -d $(LIBPCC) ] ; then mkdir $(LIBPCC) ; fi ; \
		cp libp.$(LIBNAME) $(LIBPCC)/$(LIBNAME) ; \
	else \
		if [ ! -d $(LIBP) ] ; then mkdir $(LIBP) ; fi ; \
		cp libp.$(LIBNAME) $(LIBP)/$(LIBNAME) ; \
	fi
clean:
	-rm -f *.o

clobber:	clean
	-rm -f $(LIBNAME) libp.$(LIBNAME)

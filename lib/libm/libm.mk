#	Copyright (c) 1984 AT&T
#	  All Rights Reserved

#	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T
#	The copyright notice above does not evidence any
#	actual or intended publication of such source code.

#ident	"@(#)libm:libm.mk	1.17"
#
# makefile for libm
#
# Note that the object list given in FILES is in the order in which the files
# should be added to the archive when it is created.  If the files are not
# ordered correctly, linking of libm fails on the pdp11 and is slowed down
# on other machines.

INC=$(ROOT)/usr/include
LIB=$(ROOT)/lib
LIBCC=$(ROOT)/usr/lib
LIBP=$(ROOT)/usr/lib/libp
LIBPCC=$(ROOT)/usr/lib/libpcc
USRLIB=$(ROOT)/usr/lib
VARIANT =
LIBNAME = lib$(VARIANT)m.a
PCC=/usr/bin/cc
AR=ar
MAKE=make
FILES =\
	$(LIBNAME)(asin.o)\
	$(LIBNAME)(atan.o)\
	$(LIBNAME)(erf.o)\
	$(LIBNAME)(fabs.o)\
	$(LIBNAME)(floor.o)\
	$(LIBNAME)(fmod.o)\
	$(LIBNAME)(gamma.o)\
	$(LIBNAME)(hypot.o)\
	$(LIBNAME)(jn.o)\
	$(LIBNAME)(j0.o)\
	$(LIBNAME)(j1.o)\
	$(LIBNAME)(pow.o)\
	$(LIBNAME)(log.o)\
	$(LIBNAME)(sin.o)\
	$(LIBNAME)(sinh.o)\
	$(LIBNAME)(sqrt.o)\
	$(LIBNAME)(tan.o)\
	$(LIBNAME)(tanh.o)\
	$(LIBNAME)(exp.o)\
	$(LIBNAME)(matherr.o)

all: $(LIBNAME) profiledlib

$(LIBNAME): $(FILES)
	if u3b15 || u3b5 || u3b2 ; then $(CC) -c -DM32 $(CFLAGS) $(?:.o=.c) ;\
	else if clipper && [ ! -r /bin/ghs ] ; then \
		$(PCC) -c -W0,-C1 $(CFLAGS) -I$(INC) $(?:.o=.c) ; \
	else $(CC) -c $(CFLAGS) -I$(INC) $(?:.o=.c) ; fi ; fi
	$(AR) rv $(LIBNAME) $?
	rm -f $?

$(FILES): $(INC)/math.h $(INC)/values.h $(INC)/errno.h \
		$(INC)/sys/errno.h

.c.a:;

profiledlib:
	$(MAKE) -f libm.mk -e libp.$(LIBNAME) LIBNAME=libp.$(LIBNAME) \
						CFLAGS="-p $(CFLAGS)"

install: all
	if clipper && [ ! -r /bin/ghs ] ; then \
		cp $(LIBNAME) $(LIBCC)/$(LIBNAME) ; \
		cp libp.$(LIBNAME) $(LIBPCC)/$(LIBNAME) ; \
	elif m88k && [ ! -r /bin/ghs ] ; then \
		cp $(LIBNAME) $(LIBCC)/$(LIBNAME) ; \
		cp libp.$(LIBNAME) $(LIBPCC)/$(LIBNAME) ; \
	else \
		cp $(LIBNAME) $(LIB)/$(LIBNAME) ; \
		cp libp.$(LIBNAME) $(LIBP)/$(LIBNAME) ; \
	fi

clean:
	-rm -f *.o

clobber:	clean
	-rm -f $(LIBNAME) libp.$(LIBNAME)

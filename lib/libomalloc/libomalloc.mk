#	Copyright (c) 1984 AT&T
#	  All Rights Reserved

#	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T
#	The copyright notice above does not evidence any
#	actual or intended publication of such source code.

#ident	"@(#)libomalloc:libomalloc.mk	1.18"
#
# makefile for libomalloc
#

.SUFFIXES: .p
ROOT=
AR=ar
ARFLAGS=r
CC=cc
CFLAGS=-O
CP=cp
ENV=
INC= $(ROOT)/usr/include
LIB=$(ROOT)/lib
USRLIB= $(ROOT)/usr/lib
LIBP= $(ROOT)/usr/lib/libp
LIBPCC= $(ROOT)/usr/lib/libpcc
LINT=lint
LINTFLAGS=-p
MV=mv
OBJECTS=malloc.o _free.o _malloc.o _realloc.o
POBJECTS=malloc.p _free.p _malloc.p _realloc.p
RM=rm
#variables used to build lint library in 5.0 compatible mode
TMPDIR=/usr/tmp
FRC=

all: noprof prof

noprof: $(OBJECTS)
	$(RM) -rf objects
	mkdir objects
	$(CP) $(OBJECTS) objects
	cd objects; $(AR) $(ARFLAGS) libomalloc.a *

prof: $(POBJECTS)
	$(RM) -rf pobjects
	mkdir pobjects
	for i in $(POBJECTS);\
	do\
		$(CP) $$i pobjects/`basename $$i .p`.o;\
	done
	cd pobjects; $(AR) $(ARFLAGS) libomalloc.a *

.c.o .c.p:
	@echo $*.c:
	$(PROF)$(CC) -p $(CFLAGS) -c $*.c    && $(MV) $(*F).o $*.p
	$(CC) $(CFLAGS) -c $*.c

.s.o .s.p:
	@echo $*.c:
	$(PROF)$(CC) -p $(CFLAGS) -c $*.s    && $(MV) $(*F).o $*.p
	$(CC) $(CFLAGS) -c $*.s

install: all
	if [ ! -d $(LIBP) ];\
	then\
		mkdir $(LIBP);\
	fi
	$(CP) objects/libomalloc.a $(USRLIB)/libomalloc.a
	if clipper && [ ! -r /bin/ghs ] ; then \
		$(CP) pobjects/libomalloc.a $(LIBPCC)/libomalloc.a ; \
	else \
		$(CP) pobjects/libomalloc.a $(LIBP)/libomalloc.a ; \
	fi
	if pdp11; then \
		$(USRLIB)/libomalloc.a; \
		$(LIBP)/libomalloc.a; \
	fi

clean: 
	$(RM) -fr pobjects objects

clobber: clean
	$(RM) -f $(POBJECTS) $(OBJECTS)

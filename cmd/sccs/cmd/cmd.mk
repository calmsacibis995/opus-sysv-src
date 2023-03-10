#	Copyright (c) 1984 AT&T
#	  All Rights Reserved

#	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T
#	The copyright notice above does not evidence any
#	actual or intended publication of such source code.

#ident	"@(#)sccs:cmd/cmd.mk	6.16"
#
#

STRIP = -strip

HDR = ../hdr

HELPLOC = $(ROOT)/usr/lib/help

LIBS = ../lib/comobj.a \
	../lib/cassi.a \
	../lib/mpwlib.a

ROOT =

BIN = $(ROOT)/usr/bin

CFLAGS = -O

#LDFLAGS = -n -i
LDFLAGS =

B02 = `if u370; then echo '-b0,2'; fi`
B12 = `if u370; then echo '-b1,2'; fi`

ARGS =

C_CMDS = admin	\
	cdc	\
	comb	\
	delta	\
	get	\
	prs	\
	rmdel	\
	sact	\
	unget	\
	val	\
	vc	\
	what

CMDS = $(C_CMDS)	\
	sccsdiff

all:	$(LIBS) $(CMDS) sccshelp help2

admin:	admin.o	$(LIBS)
	$(CC) $(LDFLAGS) admin.o $(LIBS) -o admin

admin.o:	admin.c
	$(CC) -c $(CFLAGS) $(B12) admin.c

cdc:	rmchg
	-ln	rmchg cdc

comb:	comb.o	$(LIBS)
	$(CC) $(LDFLAGS) comb.o $(LIBS) -o comb

comb.o:	comb.c
	$(CC) -c $(CFLAGS) $(B02) comb.c

delta:	delta.o	$(LIBS)
	$(CC) $(LDFLAGS) delta.o $(LIBS) -o delta

delta.o:	delta.c
	$(CC) -c $(CFLAGS) $(B02) delta.c

get:	get.o	$(LIBS)
	$(CC) $(LDFLAGS) get.o $(LIBS) -o get

get.o:	get.c
	$(CC) -c $(CFLAGS) $(B02) get.c

help:	help.o
	$(CC) $(LDFLAGS) help.o -o help

sccshelp:	help.o
	$(CC) $(LDFLAGS) help.o -o sccshelp

help.o:	help.c
	$(CC) -c $(CFLAGS) help.c

help2:	help2.o	$(LIBS)
	$(CC) $(LDFLAGS) help2.o $(LIBS) -o help2

help2.o: help2.c
	$(CC) -c $(CFLAGS) help2.c

prs:	prs.o	$(LIBS)
	$(CC) $(LDFLAGS) prs.o $(LIBS) -o prs

prs.o:	prs.c
	$(CC) -c $(CFLAGS) $(B12) prs.c
	
rmdel:	rmchg $(LIBS)
	-ln rmchg rmdel

rmchg:	rmchg.o $(LIBS)
	$(CC) $(LDFLAGS) rmchg.o $(LIBS) -o rmchg

rmchg.o:	rmchg.c
	$(CC) -c $(CFLAGS) $(B02) rmchg.c

sact:	unget
	-ln unget sact

sccsdiff:	sccsdiff.sh
	-cp sccsdiff.sh sccsdiff

unget:	unget.o	$(LIBS)
	$(CC) $(LDFLAGS) unget.o $(LIBS) -o unget

unget.o:	unget.c
	$(CC) -c $(CFLAGS) $(B02) unget.c

val:	val.o	$(LIBS)
	$(CC) $(LDFLAGS) val.o $(LIBS) -o val

val.o:	val.c
	$(CC) -c $(CFLAGS) $(B02) val.c

vc:	vc.o	$(LIBS)
	$(CC) $(LDFLAGS) vc.o $(LIBS) -o vc

vc.o:	vc.c
	$(CC) -c $(CFLAGS) vc.c

what:	what.o	$(LIBS)
	$(CC) $(LDFLAGS) what.o $(LIBS) -o what

what.o:	what.c
	$(CC) -c $(CFLAGS) what.c

$(LIBS):
	cd ../lib; $(MAKE) -f lib.mk

install:	all
	$(STRIP) $(C_CMDS)
	$(STRIP) sccshelp help2
	$(CH)-chmod 775 $(CMDS) sccshelp help2
	$(CH)-chgrp bin $(CMDS) help2
	$(CH)-chown bin $(CMDS) help2
	-mv $(CMDS) $(BIN)
	if [ ! -d $(HELPLOC) ] ; then mkdir $(HELPLOC) ; fi
	if [ ! -d $(HELPLOC)/lib ] ; then mkdir $(HELPLOC)/lib ; fi
	-mv help2 $(HELPLOC)/lib
	if u3b15 || u3b2 ; \
	then \
		cpset sccshelp $(HELPLOC)/lib ; \
	else \
		cpset sccshelp $(BIN) ; \
	fi

clean:
	-rm -f *.o
	-rm -f rmchg

clobber:	clean
	-rm -f $(CMDS) sccshelp help2

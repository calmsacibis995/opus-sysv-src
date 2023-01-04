
# @(#)liblang.mk	1.2	9/25/86
# @(#)Copyright (C) 1986 by National Semiconductor Corp.

# Note -- all SCCS source was copied from /l/4.2dev to /c/4.2coff on 11/21/85

ROOT	   = 
LIBDIR	   = $(ROOT)/lib
USRLIB	   = $(ROOT)/usr/lib


INSTALL	= cp

CC	= cc
LD	= ld
AR	= ar


CFLAGS	= -DHOSTSYS5 -DHOST_5_2 -O -c 

SRCS	= chiprev.c fcons.c cmdfile.c cmddir.c

FILES	= chiprev.o fcons.o cmdfile.o cmddir.o

all       : liblang.a

install   : liblang.a
	${INSTALL} liblang.a $(USRLIB)/liblang.a

liblang.a : ${FILES}
	-rm -f liblang.a
	${AR} cru liblang.a ${FILES}

clean:
	-rm -f *.o a32.out core

clobber: clean
	-rm -f liblang.a

.c.o:
	${CC} ${CFLAGS} $*.c


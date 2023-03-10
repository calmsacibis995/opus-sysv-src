#	Copyright (c) 1984 AT&T
#	  All Rights Reserved

#	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T
#	The copyright notice above does not evidence any
#	actual or intended publication of such source code.

#ident	"@(#)nlp:lib/printers/Makefile	1.4"
#
# Makefile for printermgmt(lp) routines
#


RM	=	/bin/rm -f
AR	=	ar
INS	=	install


#DEBUG	=	-g

INC	=	$(ROOT)/usr/include
LPINC	=	../../include

CFLAGS	=	$(DEBUG) -I$(LPINC) -I$(INC) -O


LIBRARY	=	liblpprt.a
PLIBRARY=	p$(LIBRARY)
QLIBRARY=	q$(LIBRARY)

LINTSRC	=	llib-llpprt
LINTLIB	=	$(LINTSRC).ln


SRCS	= \
		chkprinter.c \
		default.c \
		delprinter.c \
		freeprinter.c \
		getprinter.c \
		headings.c \
		okprinter.c \
		printwheels.c \
		putprinter.c

LINTS	= \
		../lp/llib-llp.ln \
		../fs/llib-llpfs.ln

OBJS	=	$(SRCS:.c=.o)
POBJS	=	$(SRCS:.c=.p)
QOBJS	=	$(SRCS:.c=.q)

.SUFFIXES:	.o .p .q

.c.p:
	@-if [ -f $*.o ] ; then mv $*.o tmp ; fi
	$(CC) -p $(CFLAGS) -c $*.c
	@-mv $*.o $*.p
	@-if [ -f tmp ] ; then mv tmp $*.o ; fi

.c.q:
	@-if [ -f $*.o ] ; then mv $*.o tmp ; fi
	$(CC) -ql $(CFLAGS) -c $*.c
	@-mv $*.o $*.q
	@-if [ -f tmp ] ; then mv tmp $*.o ; fi


all:		$(LIBRARY) $(LINTLIB)

install:	all
	@echo "Not a delivered part of LP; not installed"
	@false

clean:	
	$(RM) $(OBJS) $(POBJS) $(QOBJS)

clobber:	clean
	$(RM) $(LIBRARY) $(PLIBRARY) $(QLIBRARY) $(LINTLIB)

strip:
	$(STRIP) $(LIBRARY)

$(LIBRARY):	$(OBJS)
	$(AR) rv $(LIBRARY) $?

prof:		$(PLIBRARY)

$(PLIBRARY):	$(POBJS)
	$(AR) rv $(PLIBRARY) $?

lprof:		$(QLIBRARY)

$(QLIBRARY):	$(QOBJS)
	$(AR) rv $(QLIBRARY) $?

lint:
	lint -ux $(CFLAGS) $(SRCS) $(LINTS)

lintsrc:	$(LINTSRC)

$(LINTSRC):	$(SRCS)
	if [ -n "$(FUNCDCL)" ]; then $(FUNCDCL) $(SRCS) >$(LINTSRC); fi

lintlib:	$(LINTLIB)

$(LINTLIB):	$(LINTSRC)
	$(CC) -E -C $(CFLAGS) -Dlint $(LINTSRC) | /usr/lib/lint1 -vx -H./LINT_WORK > $(LINTLIB)
	$(RM) ./LINT_WORK

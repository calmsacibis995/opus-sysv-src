#	Copyright (c) 1984 AT&T
#	  All Rights Reserved

#	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T
#	The copyright notice above does not evidence any
#	actual or intended publication of such source code.

#ident	"@(#)graf:src/dev.d/tek4000.d/td.d/Makefile	1.7"
###
### Standard Constants 
###

ROOT     =
LROOT    =
BIN      = $(ROOT)/$(LROOT)/usr/bin/graf
LIB      = ../../../../lib/glib.a
LIB2     = ../lib/tek.a
LIB3     = ../../uplib/uplib.a
LIB4     = ../../lolib/lolib.a

INC	 = $(ROOT)/usr/include
LINC	 = ../../include
LINC2	 = ../../../../include

CC       = cc
#LDFLAGS  = x.o $(LIB2) $(LIB3) $(LIB4) $(LIB) -lm -i -s
LDFLAGS  = x.o $(LIB2) $(LIB3) $(LIB4) $(LIB) -lm -s
CFLAGS   = -O -I$(INC) -I$(LINC) -I$(LINC2) $(FFLAG)


#top#
# Generated by makefile 1.44    Tue Mar 12 09:47:21 EST 1985

MAKEFILE = Makefile


MAINS = td

OBJECTS =  devarc.o devbuff.o devcursor.o devfill.o devfinal.o devinit.o \
	devline.o devtext.o td.o xytek.o xytekl.o x.o

SOURCES =  devarc.c devbuff.c devcursor.c devfill.c devfinal.c devinit.c \
	devline.c devtext.c td.c xytek.c xytekl.c

ALL:		$(MAINS)

td:		td.o devfinal.o	devinit.o devcursor.o xytekl.o 
	$(CC) $(CFLAGS)  -o td  td.o devfinal.o devinit.o devcursor.o \
	xytekl.o   $(LDFLAGS)


devarc.o:	 

devbuff.o:	 

devcursor.o:	 $(INC)/stdio.h td.h $(LINC)/dev.h \
		 $(LINC2)/debug.h $(LINC2)/errpr.h \
		 $(LINC2)/util.h $(INC)/ctype.h \
		 $(LINC2)/gsl.h $(LINC2)/gpl.h 

devfill.o:	 

devfinal.o:	 $(INC)/stdio.h td.h $(LINC)/dev.h \
		 $(LINC2)/debug.h $(LINC2)/errpr.h \
		 $(LINC2)/util.h $(INC)/ctype.h \
		 $(LINC2)/gsl.h $(LINC2)/gpl.h 

devinit.o:	 $(INC)/stdio.h td.h $(LINC)/dev.h \
		 $(LINC2)/debug.h $(LINC2)/errpr.h \
		 $(LINC2)/util.h $(INC)/ctype.h \
		 $(LINC2)/gsl.h $(LINC2)/gpl.h 

devline.o:	 $(INC)/stdio.h $(LINC)/dev.h \
		 $(LINC2)/debug.h $(LINC2)/errpr.h \
		 $(LINC2)/util.h $(INC)/ctype.h \
		 $(LINC2)/gsl.h $(LINC2)/gpl.h 

devtext.o:	 

td.o:		 $(INC)/stdio.h $(LINC2)/setop.h \
		 $(INC)/ctype.h $(LINC)/chtable \
		 $(LINC)/dev.h $(LINC2)/debug.h \
		 $(LINC2)/errpr.h $(LINC2)/util.h	\
		 $(INC)/ctype.h $(LINC2)/gsl.h	\
		 $(LINC2)/gpl.h 

xytek.o:	 $(INC)/stdio.h td.h 

xytekl.o:	 $(INC)/stdio.h td.h 

GLOBALINCS = $(LINC2)/debug.h $(LINC2)/errpr.h \
	$(LINC2)/gpl.h $(LINC2)/gsl.h \
	$(LINC2)/setop.h $(LINC2)/util.h \
	$(LINC)/chtable $(LINC)/dev.h $(INC)/ctype.h \
	$(INC)/stdio.h 

LOCALINCS = td.h 


clean:
	rm -f $(OBJECTS)
	
clobber:	
	rm -f $(OBJECTS) $(MAINS) x.o

newmakefile:
	makefile -m -f $(MAKEFILE)  -s INC $(INC) -s LINC2 $(LINC2) -s LINC $(LINC)
#bottom#

$(MAINS)     : $(LIB) $(LIB2) $(LIB3) $(LIB4) x.o

x.o          : devbuff.o devtext.o devfill.o devarc.o devline.o
	$(LD) -r -o x.o devbuff.o devtext.o devfill.o devarc.o devline.o

install      : $(MAINS)
	cpset $(MAINS) $(BIN)
	
###########
	
listings     : $(SOURCES)
	pr -h "     td.d     " $(SOURCES) | lp

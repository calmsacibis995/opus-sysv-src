#ident	"@(#)sched:/makefile	1.11"

#
#			scheduler makefile
#

TARGET	= m88k
ROOT	= 
LIBDIR	= $(ROOT)/lib
BASE	= ../..
COMI	= $(BASE)/inc/m88k
INCM88K	= $(BASE)/inc/m88k
FLEX	= -DFLEXNAMES
ARFORMAT= PORTAR

OWN	= bin
GRP	= bin

USRINC= $(ROOT)/usr/$(VERSION)include

CC	= /bin/cc
OFLAG	= -O
CFLAGS	= -DTARGET=$(TARGET) $(OFLAG)
LDFLAGS =
SOURCES = parse.c machine.c branch.c table.c symbols.c types.h \
	graph.c value.c
OBJECTS = parse.o machine.o branch.o table.o symbols.o graph.o value.o
graphdefs = -DG1 -DG2 -DG3 -DG5 -DG6 -DG7 -DG8 -DG9 -DG0
INCLUDES = -I.

all: $(SGS)reorder
$(SGS)reorder: $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $(SGS)reorder
parse.o: types.h parse.c
	$(CC) -c $(INCLUDES) parse.c
table.o: types.h table.c
	$(CC) -c $(INCLUDES) table.c
machine.o: types.h machine.c
	$(CC) -c $(INCLUDES) machine.c
branch.o: types.h branch.c
	$(CC) -c $(INCLUDES) branch.c
symbols.o: types.h symbols.c
	$(CC) -c $(INCLUDES) symbols.c
graph.o: types.h graph.c
	$(CC) -c $(INCLUDES) $(graphdefs) graph.c
value.o: types.h value.c
	$(CC) -c $(INCLUDES) value.c


install :	$(LIBDIR)/$(SGS)reorder

$(LIBDIR)/$(SGS)reorder:$(SGS)reorder
			sh $(BASE)/sgs.install 755 $(OWN) $(GRP) $@ $?

clean	:
		rm -f *.o

shrink	:	 clean

clobber :	clean
		rm -f $(SGS)reorder

#  @(#)vert.mk	1.2

INSDIR = ${ROOT}/usr/lib
CFLAGS = -O -I${ROOT}/usr/include
AR=ar
INS=:

OBJ= libvert.o

libvert.a: $(OBJ)
	rm -f libvert.a
	$(AR) -r libvert.a $(OBJ)
	$(INS) $(INSDIR) libvert.a

all:	install clobber

install:
	$(MAKE) -f vert.mk INS="install -f"
	cp vert.h /usr/include
	chmod 664 /usr/include/vert.h
	chown bin /usr/include/vert.h
	chgrp bin /usr/include/vert.h

clean:
	-rm -f *.o

clobber: clean
	rm -f libvert.a 

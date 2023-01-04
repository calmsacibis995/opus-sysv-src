
#  @(#)cgi.mk	2.1

ROOT =
INCRT = $(ROOT)/usr/include
INSDIR = $(ROOT)/usr/lib
CFLAGS = -O
AR=ar
INS=:

OBJ= cgi.o

libcgi.a: $(OBJ)
	rm -f libcgi.a
	$(AR) -r libcgi.a $(OBJ)
	$(INS) $(INSDIR) libcgi.a

all:	install clobber

install:
	$(MAKE) -f cgi.mk INS="install -f"

clean:
	-rm -f *.o

clobber: clean
	rm -f libcgi.a

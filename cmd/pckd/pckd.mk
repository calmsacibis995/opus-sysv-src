#%W% %G%
ROOT =
TESTDIR = .
INSDIR = $(ROOT)/usr/bin
INCRT = $(ROOT)/usr/include
LDFLAGS = -lns
INS = :
CFLAGS = -O -s
FRC =

all: pckd

pckd: pckd.c 
	@echo "**************************"
	@echo "'pckd' not built for opus."
	@echo "**************************"
	@#$(CC) -I$(INCRT) $(CFLAGS) -o $(TESTDIR)/pckd pckd.c $(LDFLAGS)

install: all
	@#install -n $(INSDIR) $(TESTDIR)/pckd

clean:
	rm -f pckd.o

clobber: clean
	rm -f $(TESTDIR)/pckd
FRC:

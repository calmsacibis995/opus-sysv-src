#	@(#)makefile	1.8

SGS	=
TESTDIR	= .
INS	= /etc/install
INSDIR	= $(ROOT)
CC	= /usr/bin/cc
FLAGS	= -DFLEXNAMES
LDFLAGS	= 
CFLAGS = -O
INSTALL	= $(SGS)comp
M	= ../mip

all: $(SGS)comp

objects: cgram.o xdefs.o scan.o pftn.o trees.o optim.o code.o \
		local.o reader.o local2.o order.o match.o allo.o \
		comm1.o table.o messages.o args.o uxrel.o

$(SGS)comp: rodata.o cgram.o xdefs.o scan.o pftn.o trees.o optim.o code.o \
		local.o reader.o local2.o order.o match.o allo.o \
		comm1.o table.o messages.o args.o uxrel.o
	$(CC) $(LDFLAGS) rodata.o cgram.o xdefs.o scan.o pftn.o trees.o \
		optim.o code.o local.o reader.o local2.o order.o args.o \
		match.o allo.o comm1.o table.o messages.o uxrel.o \
		-o $(TESTDIR)/$(SGS)comp

trees.o: $M/manifest macdefs $M/mfile1 $M/messages.h $M/trees.c
	$(CC) -c $(FLAGS) $(CFLAGS) -I$M -I.  $M/trees.c

optim.o: $M/manifest macdefs $M/mfile1 $M/optim.c
	$(CC) -c $(FLAGS) $(CFLAGS) -I$M -I. $M/optim.c

pftn.o: $M/manifest macdefs $M/mfile1 $M/messages.h $M/pftn.c
	$(CC) -c $(FLAGS) $(CFLAGS) -I$M -I. $M/pftn.c

code.o: $M/manifest macdefs $M/mfile1 $M/messages.h
	$(CC) -c $(FLAGS) $(CFLAGS) -I$M -I. code.c

local.o: $M/manifest macdefs $M/mfile1
	$(CC) -c $(FLAGS) $(CFLAGS) -I$M -I. local.c

scan.o: $M/manifest macdefs $M/mfile1 $M/messages.h $M/scan.c
	$(CC) -c $(FLAGS) $(CFLAGS) -I$M -I. $M/scan.c

xdefs.o: $M/manifest $M/mfile1 macdefs $M/xdefs.c
	$(CC) -c $(FLAGS) $(CFLAGS) -I$M -I. $M/xdefs.c

cgram.o: $M/manifest $M/mfile1 macdefs $M/messages.h $M/cgram.c
	$(CC) -c $(FLAGS) $(CFLAGS) -DYYMAXDEPTH=300 -I. -I$M $M/cgram.c

rodata.o $M/cgram.c: $M/cgram.y
	$(YACC) $M/cgram.y
	rm -f rodata.c
	sh ./:yyfix >rodata.c
	mv y.tab.c $M/cgram.c
	$(CC) $(FLAGS) $(CFLAGS) -S rodata.c
	if vax; then\
		sh ./:rofix rodata.s;\
	fi
	$(CC) $(FLAGS) $(CFLAGS) -c rodata.s
	rm -f rodata.s rodata.c

comm1.o: $M/manifest $M/mfile1 $M/common macdefs $M/comm1.c
	$(CC) -c $(FLAGS) $(CFLAGS) -I. -I$M -DEXIT=dexit $M/comm1.c

table.o: $M/manifest $M/mfile2 mac2defs macdefs table.c
	$(CC) $(FLAGS) $(CFLAGS) -S -I$M -I. table.c
	if vax; then\
		sh ./:rofix table.s;\
	fi
	$(CC) $(FLAGS) $(CFLAGS) -c table.s
	rm -f table.s

messages.o: $M/messages.c $M/messages.h
	$(CC) $(FLAGS) $(CFLAGS) -S -I$M -I. $M/messages.c
	if vax; then\
		sh ./:rofix messages.s;\
	fi
	$(CC) $(FLAGS) $(CFLAGS) -c messages.s
	rm -f messages.s

reader.o: $M/manifest $M/mfile2 mac2defs macdefs $M/messages.h $M/reader.c
	$(CC) -c $(FLAGS) $(CFLAGS) -I$M -I. $M/reader.c

local2.o: $M/manifest $M/mfile2 mac2defs macdefs
	$(CC) -c $(FLAGS) $(CFLAGS) -I$M -I. local2.c

args.o: $M/manifest $M/mfile2 mac2defs macdefs
	$(CC) -c $(FLAGS) $(CFLAGS) -I$M -I. args.c

order.o: $M/manifest $M/mfile2 mac2defs macdefs
	$(CC) -c $(FLAGS) $(CFLAGS) -I$M -I. order.c

match.o: $M/manifest $M/mfile2 mac2defs macdefs $M/match.c
	$(CC) -c $(FLAGS) $(CFLAGS) -I$M -I. $M/match.c

allo.o: $M/manifest $M/mfile2 mac2defs macdefs $M/allo.c
	$(CC) -c $(FLAGS) $(CFLAGS) -I$M -I. $M/allo.c

uxrel.o:
	echo 'char uxrel[] = "' `date` '";' > uxrel.c
	$(CC) $(CFLAGS) -c uxrel.c
	rm -f uxrel.c

shrink:
	rm -f *.o $(SGS)comp

clean:
	rm -f *.o

clobber: clean
	rm -f $(TESTDIR)/$(SGS)comp $M/cgram.c

lintall:
	lint -hpv -I. -I$M  $M/cgram.c $M/xdefs.c $M/scan.c $M/pftn.c \
		$M/trees.c $M/optim.c code.c local.c $M/reader \
		local2.c order.c $M/match.c $M/allo.c $M/comm1.c table.c

install:	all
	-ln $(TESTDIR)/$(SGS)comp $(INSTALL)
	$(INS) -f $(INSDIR)/usr/lib $(INSTALL)
	rm $(INSTALL)

TARGET =	m88k
CC		=/bin/cc 
OFLAG		=-O
CFLAGS		= $(OFLAG) -DTARGET=$(TARGET)
INCLUDES	=
BINDIR		= .
SIFILTER	= $(SGS)sifilter
BASE		= ../..

$(SIFILTER):		sifilter.o itoa.o
			$(CC) sifilter.o itoa.o -o $@

itoa.o:			itoa.c
			$(CC) $(CFLAGS) -c itoa.c

sifilter.o:		sifilter.c
			$(CC) $(CFLAGS) -c sifilter.c 

install :		$(BINDIR)/$(SIFILTER)

$(BINDIR)/$(SIFILTER):	$(SIFILTER)
			sh $(BASE)/sgs.install 755 $(OWN) $(GRP) $@ $?

shrink:
			rm -f $(SIFILTER) sifilter.o itoa.o

clean:
			rm -f $(SIFILTER) sifilter.o itoa.o

clobber:
			rm -f $(SIFILTER) sifilter.o itoa.o

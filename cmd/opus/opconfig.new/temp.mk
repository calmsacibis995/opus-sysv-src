# makefile for opconfig -- opus configuration program

CC=cc -g
CFLAGS=-O
LDFLAGS=
STRIP=

ROOT=
SA=
OWN=bin
GRP=bin
INSDIR=$(ROOT)/stand
INSDIR1=$(ROOT)/opus/bin

HDRS = 	opconfig.h

SRCS = main.c standard.c unixparam.c unixfilsys.c spare.c partition.c opuscfg.c
OBJS = main.o standard.o unixparam.o unixfilsys.o spare.o partition.o opuscfg.o

all : opconfigsa opconfigts

.c.o:
	$(PFX)cc $(CFLAGS) -c $*.c

opconfigsa: $(SRCS) $(HDRS)
	-if m88k ; then \
	$(PFX)scc -g $(CFLAGS) $(LDFLAGS) -Dpm400 -o opconfigsa $(SRCS); \
	elif clipper ; then \
	$(PFX)scc -g $(CFLAGS) $(LDFLAGS) -Dpm300 -o opconfigsa $(SRCS); \
	elif ns32332 ; then \
	$(PFX)scc -g $(CFLAGS) $(LDFLAGS) -Dpm200 -o opconfigsa $(SRCS); \
	else \
	$(PFX)scc -g $(CFLAGS) $(LDFLAGS) -Dpm100 -o opconfigsa $(SRCS); \
	fi

opconfigts: $(SRCS) $(HDRS)
	-if m88k ; then \
	$(PFX)cc $(CFLAGS) $(LDFLAGS) -Dpm400 -o opconfigts $(SRCS) -lopus; \
	elif clipper ; then \
	$(PFX)cc $(CFLAGS) $(LDFLAGS) -Dpm300 -o opconfigts $(SRCS) -lopus; \
	elif ns32332 ; then \
	$(PFX)cc $(CFLAGS) $(LDFLAGS) -Dpm200 -o opconfigts $(SRCS) -lopus; \
	else \
	$(PFX)cc $(CFLAGS) $(LDFLAGS) -Dpm100 -o opconfigts $(SRCS) -lopus; \
	fi

install: all
	-mkdir $(INSDIR)
	rm -f $(INSDIR)/opconfig
	cp opconfigsa $(INSDIR)/opconfig
	$(STRIP) $(INSDIR)/opconfig
	chmod 775 $(INSDIR)/opconfig
	chgrp $(GRP) $(INSDIR)/opconfig
	chown $(OWN) $(INSDIR)/opconfig
	-mkdir $(INSDIR1)
	rm -f $(INSDIR1)/opconfig
	cp opconfigts $(INSDIR1)/opconfig
	$(STRIP) $(INSDIR1)/opconfig
	chmod 775 $(INSDIR1)/opconfig
	chgrp $(GRP) $(INSDIR1)/opconfig
	chown $(OWN) $(INSDIR1)/opconfig

clean:
	rm -f *.o

clobber: clean
	rm -f opconfigsa opconfigts


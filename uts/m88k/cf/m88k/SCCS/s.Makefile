h32127
s 00001/00001/00148
d D 1.3 90/04/24 12:24:08 root 3 2
c aenable DASHO, removed CC
e
s 00001/00001/00148
d D 1.2 90/04/24 11:39:14 root 2 1
c DASHO set to -O
e
s 00149/00000/00000
d D 1.1 90/03/06 12:51:18 root 1 0
c date and time created 90/03/06 12:51:18 by root
e
u
U
t
T
I 1
#		Copyright (c) 1986,1987 Motorola Inc.		
#		All Rights Reserved				
#		@(#)Makefile	10.4

#ident	"@(#)kern-port:sysV68.mk	10.11"

RELEASE=R3V8
INCRT=/usr/src/uts/m88k
LIBDIR=../cf/m88k
CPU=m88k
TARGET=opus
MMU=CMMU
BRDEFS= -D$(MMU) -D$(TARGET) -DLAINFS -DBCS -DXBus # -DSIMULATOR

NAME = sysV
SYS = 88

DASHG =
D 2
DASHO = 
E 2
I 2
DASHO = -O
E 2
FRC =

D 3
CC=/usr/bin/cc
E 3
I 3
#CC=/usr/bin/cc
E 3
AR=ar
LD=ld

#KNOWNLIBS = lib.ml lib.os lib.io lib.s5 lib.bpp lib.fpack lib.stub \
#	lib.streams lib.du lib.tpi
KNOWNLIBS = lib.ml lib.os lib.io lib.s5 lib.stub lib.du lib.nfs lib.bsd

LIBS = lib.io lib.s5 lib.du lib.stub lib.nfs lib.bsd
CFLAGS= $(DASHO) $(DASHG) -I$(INCRT) -DINKERNEL -DREM -D$(CPU) $(BRDEFS)
XCFLAGS=

NSELIBS = lib.io lib.s5 lib.du lib.stub lib.nfs lib.bsd

#NSEXCFLAGS= -DDU -DSTREAMS
NSEXCFLAGS=

MAKEARGS= "MAKE=$(MAKE)" "FRC=$(FRC)" "DASHG=$(DASHG)" "DASHO=$(DASHO)" \
	 "INCRT=$(INCRT)" "LIBDIR=$(LIBDIR)" "SYS=$(SYS)" "CFLAGS=$(CFLAGS)" \
	 "CC=$(CC)" "AS=$(AS)" "AR=$(AR)" "BRDEFS=$(BRDEFS)" \
	 "CPU=$(CPU)"

all:
	-@if [ -f Makelock ] ;						\
	then								\
	    echo Make already started by `cat Makelock` ;		\
	    exit 1 ;							\
	fi ;								\
									\
	trap 'rm -f Makelock'  0 1 2 3 15 ;				\
									\
	(echo "$$LOGNAME on \c"; date) >Makelock ;			\
									\
	$(MAKE) kernel ;						\
									\
	rm -f Makelock

kernel:	libs
	-@ SHLIBS="$(NSELIBS)"  ;				\
	SHXCFLAGS="$(NSEXCFLAGS)" ;				\
	for i in lib.* ;						\
	do								\
		for j in $(KNOWNLIBS) ;					\
		do							\
			if [ "$$i" = "$$j" ] ;				\
			then						\
				continue 2 ;				\
			fi ;						\
		done ;							\
		SHLIBS="$$SHLIBS $$i" ;					\
	done ;								\
	: search libraries twice to resolve interdependencies ;		\
	SHLIBS="$$SHLIBS" ;				\
	$(MAKE) $(NAME)$(SYS) $(MAKEARGS) LIBS="$$SHLIBS" XCFLAGS="$$SHXCFLAGS"

$(NAME)$(SYS): $(LIBS) comm.o m88kvec.o opus.o conf.o lib.os lib.ml
	-rm -f $(NAME)$(SYS)
	time $(LD) -s -o $(NAME)$(SYS) ld.file comm.o m88kvec.o opus.o conf.o  \
               lib.ml lib.os $(LIBS)

libs:FRC
	-@cd ../.. ;							\
	for i in * ;							\
	do								\
		if [ $$i != cf -a -d $$i -a -f $$i/$$i.mk ] ;		\
		then							\
			cd $$i ;					\
			echo "	"$(MAKE) -f $$i.mk $(MAKEARGS) ;	\
			$(MAKE) -f $$i.mk $(MAKEARGS) ;			\
			cd .. ;						\
		fi ;							\
	done

lib.$(LIB):
	-@cd ../.. ;					\
	cd $$LIB ;					\
	echo "	"$(MAKE) -f $$LIB.mk $(MAKEARGS) ;	\
	$(MAKE) -f $$LIB.mk $(MAKEARGS) ;
			
m88kvec.o:	m88kvec.s
	$(CC) -c m88kvec.s

opus.o:	opus.s
	$(CC) -c opus.s

intvec.o:	intvec.s
	$(CC) -c intvec.s

low.o:	low.s
	$(CC) -c low.s

comm.o:	comm.s
	$(CC) -c comm.s

conf.o:	conf.c config.h $(INCRT)/sys/conf.h $(INCRT)/sys/param.h \
		$(INCRT)/sys/types.h $(INCRT)/sys/sysmacros.h \
		$(INCRT)/sys/space.h devsw.h
	$(CC) -c $(CFLAGS) $(XCFLAGS) conf.c

clean:
	-@cd ../.. ;							\
	for i in * ;							\
	do								\
		if [ $$i != cf -a -d $$i -a -f $$i/$$i.mk ] ;		\
		then							\
			cd $$i ;					\
			echo "	"$(MAKE) -f $$i.mk clean $(MAKEARGS) ;	\
			$(MAKE) -f $$i.mk clean $(MAKEARGS) ;		\
			cd .. ;						\
		fi ;							\
	done

clobber:	clean
	-rm -f comm.o conf.o m88kvec.o opus.o;
	-@cd ../.. ;							\
	for i in * ;							\
	do								\
		if [ $$i != cf -a -d $$i -a -f $$i/$$i.mk ] ;		\
		then							\
			cd $$i ;					\
			echo "	"$(MAKE) -f $$i.mk clobber $(MAKEARGS) ;\
			$(MAKE) -f $$i.mk clobber $(MAKEARGS) ;	\
			cd .. ;						\
		fi ;							\
	done
	-rm -f $(NAME)$(SYS) $(NAME)$(SYS).nm

FRC:
E 1

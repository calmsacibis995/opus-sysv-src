h30072
s 00015/00000/00000
d D 1.1 90/03/06 12:45:49 root 1 0
c date and time created 90/03/06 12:45:49 by root
e
u
U
t
T
I 1
#		Copyright (c) 1986,1987 Motorola Inc.		
#		All Rights Reserved				
#		@(#)Makefile	10.4

all:
	-@cd $(CPU);						 \
	echo "	"$(CPU): $(MAKE) -f ml.mk \"CPU=$(CPU)\" \"BRDEFS=$(BRDEFS)\" \"CC=$(CC)\" \"AR=$(AR)\";	\
	$(MAKE) -f ml.mk "CPU=$(CPU)" "BRDEFS=$(BRDEFS)" "CC=$(CC)" "AR=$(AR)"	
FRC:

clean:
	rm -f $(LIBDIR)/lib.ml

clobber:
	rm -f $(LIBDIR)/lib.ml
E 1

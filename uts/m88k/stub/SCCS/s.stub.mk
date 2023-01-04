h31204
s 00015/00000/00000
d D 1.1 90/03/06 12:45:27 root 1 0
c date and time created 90/03/06 12:45:27 by root
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
	echo "	"$(CPU): $(MAKE) -f stub.mk \"CPU=$(CPU)\" \"BRDEFS=$(BRDEFS)\" \"AR=$(AR)\" \"CC=$(CC)\"; 		     \
	$(MAKE) -f stub.mk "CPU=$(CPU)" "BRDEFS=$(BRDEFS)" "AR=$(AR)" "CC=$(CC)" 
FRC:

clean:
	rm -f $(LIBDIR)/lib.stub

clobber:
	rm -f $(LIBDIR)/lib.stub
E 1

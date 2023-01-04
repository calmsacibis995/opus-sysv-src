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

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

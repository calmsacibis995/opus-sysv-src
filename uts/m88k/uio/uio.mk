#
ROOT =
INCRT = ..
LIBDIR=../cf/m88k
LIBNAME = $(LIBDIR)/lib.io

DASHG =
DASHO = -O
PFLAGS = $(DASHG) -I$(INCRT) -DINKERNEL 
CFLAGS = $(DASHO) $(PFLAGS) $(BRDEFS) -D$(CPU)
FRC =
OWN = bin
GRP = bin

# add your .o files to this list for example
# DFILES = udev.o
DFILES =

all:	update subdirs pickup $(LIBNAME)

.PRECIOUS:	$(LIBNAME)

pickup:
	-@if [ "`ls *.o 2>/dev/null`" ] ; \
	then \
		echo "\t$(AR) ruv $(LIBNAME) *.o" ; \
#		$(AR) ruv $(LIBNAME) *.o ; \
		echo "\trm -f *.o" ; \
		rm -f *.o  ; \
	fi

subdirs:
	-@for i in * ; \
	do \
		if [ -d $$i -a -f $$i/$$i.mk ] ; then \
			cd $$i; \
			echo "	$(MAKE) -f $$i.mk \"MAKE=$(MAKE)\" \"CPU=$(CPU)\" \"AR=$(AR)\" \"CC=$(CC)\" \"FRC=$(FRC)\" \"INCRT=$(INCRT)\" \"BRDEFS=$(BRDEFS)\" \"DASHO=$(DASHO)\" "; \
			$(MAKE) -f $$i.mk "MAKE=$(MAKE)" "CPU=$(CPU)" "AR=$(AR)" "CC=$(CC)" "FRC=$(FRC)" "INCRT=$(INCRT)" "BRDEFS=$(BRDEFS)" "DASHO=$(DASHO)" ; \
			cd ..; \
		fi ; \
	done

update:
	-@for i in *.c ; \
	do \
		if [ -f ../io/$$i ] ; then \
			echo uio.mk: updating $$i; \
			rm $$i ; \
			ln ../io/$$i $$i; \
		fi ; \
	done

$(LIBNAME):	$(DFILES)
#	$(AR) ruv $(LIBNAME) *.o
	rm -f *.o

.c.a:
	$(CC) $(CFLAGS) -c $<

clean:
	-@for i in * ; \
	do \
		if [ -d $$i -a -f $$i/$$i.mk ] ; then \
			cd $$i; \
			echo "	$(MAKE) -f $$i clean"; \
			$(MAKE) -f $$i.mk clean; \
			cd ..; \
		fi ; \
	done
	-rm -f *.o

clobber:
	-@for i in * ; \
	do \
		if [ -d $$i -a -f $$i/$$i.mk ] ; then \
			cd $$i; \
			echo "	$(MAKE) -f $$i clobber"; \
			$(MAKE) -f $$i.mk clobber; \
			cd ..; \
		fi ; \
	done
	-rm -f *.o z8530.c
	-rm -f $(LIBNAME)

FRC:

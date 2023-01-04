#	@(#)libmalloc.mk	1.8
#	libmalloc GLOBAL MAKEFILE

all:
	if m88k; then \
		cd m88k; make -f libmalloc.mk all; exit; \
	else \
		cd port; make -f libmalloc.mk all; exit; fi;

install:
	if m88k; then \
		cd m88k; make -f libmalloc.mk install; exit; \
	else \
		cd port; make -f libmalloc.mk install; exit; fi;

clean:
	if m88k; then \
		cd m88k; make -f libmalloc.mk clean; exit; \
	else \
		cd port; make -f libmalloc.mk clean; exit; fi;

clobber:
	if m88k; then \
		cd m88k; make -f libmalloc.mk clobber; exit; \
	else \
		cd port; make -f libmalloc.mk clobber; exit; fi;

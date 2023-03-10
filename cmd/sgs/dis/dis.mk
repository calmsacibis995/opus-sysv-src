#	@(#) dis.mk: 1.1 12/10/82

#	dis.mk dis makefile

ROOT =
BINDIR = $(ROOT)/bin
#
# CFLAGS should also be passed down, but this won't be done until
# all the makefiles have been standardized
#

all dis:
	-if u3b; then \
		cd u3b; $(MAKE) -$(MAKEFLAGS) CC="$(CC)"; \
	elif ns32000; then \
		cd ns32000; $(MAKE) -$(MAKEFLAGS) CC="$(CC)" LIB5="$(LIB5)"; \
	else \
		echo 'Cannot make dis command: unknown target procesor.'; \
	fi



clean:
	-if u3b; then \
		cd u3b; $(MAKE) -$(MAKEFLAGS) clean; \
	elif ns32000; then \
		cd ns32000; $(MAKE) -$(MAKEFLAGS) clean; \
	fi


install: 
	-if u3b; then \
		cd u3b; $(MAKE) -$(MAKEFLAGS) CC="$(CC)" BINDIR=$(BINDIR) \
		install; \
	elif ns32000; then \
		cd ns32000; $(MAKE) -$(MAKEFLAGS) CC="$(CC)" BINDIR=$(BINDIR) \
		install; \
	fi

clobber: 
	-if u3b; then \
		cd u3b; $(MAKE) -$(MAKEFLAGS) clobber; \
	elif ns32000; then \
		cd ns32000; $(MAKE) -$(MAKEFLAGS) clobber; \
	fi

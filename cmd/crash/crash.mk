#	Copyright (c) 1984 AT&T
#	  All Rights Reserved

#	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T
#	The copyright notice above does not evidence any
#	actual or intended publication of such source code.

#ident	"@(#)crash:crash.mk	1.2"
FFLAG =

CC = cc

CFLAGS = -O

install:
	-if vax; then cd vax; \
		make -f crash.mk ARGS="$(ARGS)" CFLAGS="$(CFLAGS)" \
		FFLAG="$(FFLAG)" install clobber; \
		fi
	-if pdp11; then cd pdp11; \
		make -f crash.mk ARGS="$(ARGS)" CFLAGS="$(CFLAGS)" \
		FFLAG="$(FFLAG)" install clobber; fi
	-if u3b; then cd u3b20; \
		make -f crash.mk ARGS="$(ARGS)" CFLAGS="$(CFLAGS)" \
		FFLAG="$(FFLAG)" install clobber; fi
	-if u3b5 || u3b2; then cd u3b5; \
		make -f crash.mk ARGS="$(ARGS)" CFLAGS="$(CFLAGS)" \
		FFLAG="$(FFLAG)" install clobber; fi
	if clipper; then cd clipper; \
		make -f crash.mk ARGS="$(ARGS)" CFLAGS="$(CFLAGS)" \
		FFLAG="$(FFLAG)" install clobber; fi
	if ns32000; then cd ns32000; \
		make -f crash.mk ARGS="$(ARGS)" CFLAGS="$(CFLAGS)" \
		FFLAG="$(FFLAG)" install clobber; fi

clean:
	-if vax; then cd vax; \
		make -f crash.mk ARGS="$(ARGS)" CFLAGS="$(CFLAGS)" \
		FFLAG="$(FFLAG)" clean; \
		fi
	-if pdp11; then cd pdp11; \
		make -f crash.mk ARGS="$(ARGS)" CFLAGS="$(CFLAGS)" \
		FFLAG="$(FFLAG)" clean; fi
	-if u3b; then cd u3b20; \
		make -f crash.mk ARGS="$(ARGS)" CFLAGS="$(CFLAGS)" \
		FFLAG="$(FFLAG)" clean; fi
	-if u3b5 || u3b2; then cd u3b5; \
		make -f crash.mk ARGS="$(ARGS)" CFLAGS="$(CFLAGS)" \
		FFLAG="$(FFLAG)" clean; fi
	-if clipper; then cd clipper; \
		make -f crash.mk ARGS="$(ARGS)" CFLAGS="$(CFLAGS)" \
		FFLAG="$(FFLAG)" clean; fi
	-if ns32000; then cd ns32000; \
		make -f crash.mk ARGS="$(ARGS)" CFLAGS="$(CFLAGS)" \
		FFLAG="$(FFLAG)" clean; fi

clobber:
	-if vax; then cd vax; \
		make -f crash.mk ARGS="$(ARGS)" CFLAGS="$(CFLAGS)" \
		FFLAG="$(FFLAG)" clobber; \
		fi
	-if pdp11; then cd pdp11; \
		make -f crash.mk ARGS="$(ARGS)" CFLAGS="$(CFLAGS)" \
		FFLAG="$(FFLAG)" clobber; fi
	-if u3b; then cd u3b20; \
		make -f crash.mk ARGS="$(ARGS)" CFLAGS="$(CFLAGS)" \
		FFLAG="$(FFLAG)" clobber; fi
	-if u3b5 || u3b2; then cd u3b5; \
		make -f crash.mk ARGS="$(ARGS)" CFLAGS="$(CFLAGS)" \
		FFLAG="$(FFLAG)" clobber; fi
	-if clipper; then cd clipper; \
		make -f crash.mk ARGS="$(ARGS)" CFLAGS="$(CFLAGS)" \
		FFLAG="$(FFLAG)" clobber; fi
	-if ns32000; then cd ns32000; \
		make -f crash.mk ARGS="$(ARGS)" CFLAGS="$(CFLAGS)" \
		FFLAG="$(FFLAG)" clobber; fi

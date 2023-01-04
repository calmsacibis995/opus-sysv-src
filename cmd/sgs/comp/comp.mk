#	@(#)pcc.mk	1.9
#
#	This makefile generates the vax, ns32000, or pdp11 portable C compiler.
#
#	The directories assumed (multi-machine view) are:
#
#			    /pci --- pci.mk
#			     /
#			    /
#			 /c2 --- c2.mk		(vax only)
#			  /
#			 / cc.c
#			/  /
#		       /  /  pcc.mk		(this makefile)
#	/usr/src/cmd/cc --   /
#		       \    /   /mip
#			\  /     /
#		       /pcc -------- /ns32000
#			   \     \
#			    \   /vax --- pcc.mk
#			     \
#			      \
#			     /pdp11 --- pcc.mk
#
TESTDIR=.
INS= /etc/install -n $(ROOT)/bin
INSDIR=
CFLAGS=-O -I../../head
LDFLAGS = -s -n

all: make_pcc

make_pcc:
	-if vax; \
	then \
		cd ../c2; \
		$(MAKE) -$(MAKEFLAGS) -f c2.mk LDFLAGS="$(LDFLAGS)" \
			CFLAGS="$(CFLAGS)" TESTDIR=$(TESTDIR) all; \
	elif clipper; \
	then \
		cd ../pci/clipper; \
		$(MAKE) -$(MAKEFLAGS) -f c2.mk CC="$(CC)" LDFLAGS="$(LDFLAGS)" \
			CFLAGS="$(CFLAGS) -DCLIPPER" TESTDIR=$(TESTDIR) all; \
	elif ns32000; \
	then \
		cd ../pci/ns32000; \
		$(MAKE) -$(MAKEFLAGS) -f c2.mk CC="$(CC)" LDFLAGS="$(LDFLAGS)" \
			CFLAGS="$(CFLAGS) -DN16" TESTDIR=$(TESTDIR) \
			LIB5=$(LIB5) all; \
	elif m68; \
	then \
		cd ../pci/m68; \
		$(MAKE) -$(MAKEFLAGS) -f c2.mk CC="$(CC)" LDFLAGS="$(LDFLAGS)" \
			CFLAGS="$(CFLAGS)" TESTDIR=$(TESTDIR) all; \
	fi
	-if vax; \
	then \
		cd vax; \
		$(MAKE) -$(MAKEFLAGS) -f pcc.mk LDFLAGS="$(LDFLAGS)" \
			CFLAGS="$(CFLAGS)" TESTDIR=$(TESTDIR) all; \
	elif clipper; \
	then \
		cd clipper; \
		$(MAKE) -$(MAKEFLAGS) -f pcc.mk CC="$(CC)" \
			LDFLAGS="$(LDFLAGS)" \
			CFLAGS="$(CFLAGS)" TESTDIR=$(TESTDIR) all; \
	elif ns32000; \
	then \
		cd ns32000; \
		$(MAKE) -$(MAKEFLAGS) -f pcc.mk CC="$(CC)" \
			LDFLAGS="$(LDFLAGS)" \
			CFLAGS="$(CFLAGS)" TESTDIR=$(TESTDIR) \
			LIB5=$(LIB5) all; \
	elif m68; \
	then \
		cd m68; \
		$(MAKE) -$(MAKEFLAGS) -f pcc.mk CC="$(CC)" \
			LDFLAGS="$(LDFLAGS)" \
			CFLAGS="$(CFLAGS) -DM68" TESTDIR=$(TESTDIR) all; \
	fi
	-if pdp11; \
	then \
		cd pdp11; \
		$(MAKE) -$(MAKEFLAGS) -f pcc.mk LDFLAGS="$(LDFLAGS)" \
			CFLAGS="$(CFLAGS) -UFLEXNAME" TESTDIR=$(TESTDIR) all; \
	fi

clean:
	-rm -f *.o
	-if pdp11; \
	then \
		cd pdp11; \
		$(MAKE) -$(MAKEFLAGS) -f pcc.mk TESTDIR=$(TESTDIR) clean; \
	fi
	-if vax; \
	then \
		cd vax; \
		$(MAKE) -$(MAKEFLAGS) -f pcc.mk TESTDIR=$(TESTDIR) clean; \
	fi
	-if clipper; \
	then \
		cd clipper; \
		$(MAKE) -$(MAKEFLAGS) -f pcc.mk TESTDIR=$(TESTDIR) clean; \
	fi
	-if ns32000; \
	then \
		cd ns32000; \
		$(MAKE) -$(MAKEFLAGS) -f pcc.mk TESTDIR=$(TESTDIR) clean; \
	fi
	-if m68; \
	then \
		cd m68; \
		$(MAKE) -$(MAKEFLAGS) -f pcc.mk TESTDIR=$(TESTDIR) clean; \
	fi
	-if vax; \
	then \
		cd ../c2; \
		$(MAKE) -$(MAKEFLAGS) -f c2.mk TESTDIR=$(TESTDIR) clean; \
	fi
	-if clipper; \
	then \
		cd ../pci/clipper; \
		$(MAKE) -$(MAKEFLAGS) -f c2.mk TESTDIR=$(TESTDIR) clean; \
	fi
	-if ns32000; \
	then \
		cd ../pci/ns32000; \
		$(MAKE) -$(MAKEFLAGS) -f c2.mk TESTDIR=$(TESTDIR) clean; \
	fi
	-if m68; \
	then \
		cd ../pci/m68; \
		$(MAKE) -$(MAKEFLAGS) -f c2.mk TESTDIR=$(TESTDIR) clean; \
	fi

shrink:	clobber

clobber: clean
	-if pdp11; \
	then \
		rm -f $(TESTDIR)/pcc; \
		cd pdp11; \
		$(MAKE) -$(MAKEFLAGS) -f pcc.mk TESTDIR=$(TESTDIR) clobber; \
	fi
	-if vax; \
	then \
		rm -f $(TESTDIR)/cc; \
		cd vax; \
		$(MAKE) -$(MAKEFLAGS) -f pcc.mk TESTDIR=$(TESTDIR) clobber; \
	fi
	-if vax; \
	then \
		cd ../c2; \
		$(MAKE) -$(MAKEFLAGS) -f c2.mk TESTDIR=$(TESTDIR) clobber; \
	fi
	-if clipper; \
	then \
		rm -f $(TESTDIR)/cc; \
		cd clipper; \
		$(MAKE) -$(MAKEFLAGS) -f pcc.mk TESTDIR=$(TESTDIR) clobber; \
	fi
	-if clipper; \
	then \
		cd ../pci/clipper; \
		$(MAKE) -$(MAKEFLAGS) -f c2.mk TESTDIR=$(TESTDIR) clobber; \
	fi
	-if ns32000; \
	then \
		rm -f $(TESTDIR)/cc; \
		cd ns32000; \
		$(MAKE) -$(MAKEFLAGS) -f pcc.mk TESTDIR=$(TESTDIR) clobber; \
	fi
	-if ns32000; \
	then \
		cd ../pci/ns32000; \
		$(MAKE) -$(MAKEFLAGS) -f c2.mk TESTDIR=$(TESTDIR) clobber; \
	fi
	-if m68; \
	then \
		rm -f $(TESTDIR)/cc; \
		cd m68; \
		$(MAKE) -$(MAKEFLAGS) -f pcc.mk TESTDIR=$(TESTDIR) clobber; \
	fi
	-if m68; \
	then \
		cd ../pci/m68; \
		$(MAKE) -$(MAKEFLAGS) -f c2.mk TESTDIR=$(TESTDIR) clobber; \
	fi

install: all
	-if pdp11; \
	then \
		$(INS) $(TESTDIR)/pcc; \
		cd pdp11; \
		$(MAKE) -$(MAKEFLAGS) -f pcc.mk LDFLAGS="$(LDFLAGS)" \
			CFLAGS="$(CFLAGS) -UFLEXNAME" TESTDIR=$(TESTDIR) \
			INSTALL=/lib install; \
	fi
	-if vax; \
	then \
		$(INS) $(TESTDIR)/cc; \
		cd vax; \
		$(MAKE) -$(MAKEFLAGS) -f pcc.mk LDFLAGS="$(LDFLAGS)" \
			CFLAGS="$(CFLAGS)" TESTDIR=$(TESTDIR) \
			INSTALL=ccom install; \
		cd ../../c2; \
		$(MAKE) -$(MAKEFLAGS) -f c2.mk LDFLAGS="$(LDFLAGS)" \
			CFLAGS="$(CFLAGS)" TESTDIR=$(TESTDIR) install; \
	fi
	-if clipper; \
	then \
		$(INS) $(TESTDIR)/cc; \
		cd clipper; \
		$(MAKE) -$(MAKEFLAGS) -f pcc.mk LDFLAGS="$(LDFLAGS)" \
			CFLAGS="$(CFLAGS)" TESTDIR=$(TESTDIR) \
			INSTALL=comp install; \
		cd ../../pci/clipper; \
		$(MAKE) -$(MAKEFLAGS) -f c2.mk LDFLAGS="$(LDFLAGS)" \
			CFLAGS="$(CFLAGS)" TESTDIR=$(TESTDIR) install; \
	fi
	-if ns32000; \
	then \
		$(INS) $(TESTDIR)/cc; \
		cd ns32000; \
		$(MAKE) -$(MAKEFLAGS) -f pcc.mk LDFLAGS="$(LDFLAGS)" \
			CFLAGS="$(CFLAGS)" TESTDIR=$(TESTDIR) \
			INSTALL=ccom install; \
		cd ../../pci/ns32000; \
		$(MAKE) -$(MAKEFLAGS) -f c2.mk LDFLAGS="$(LDFLAGS)" \
			CFLAGS="$(CFLAGS)" TESTDIR=$(TESTDIR) install; \
	fi
	-if m68; \
	then \
		$(INS) $(TESTDIR)/cc; \
		cd m68; \
		$(MAKE) -$(MAKEFLAGS) -f pcc.mk LDFLAGS="$(LDFLAGS)" \
			CFLAGS="$(CFLAGS)" TESTDIR=$(TESTDIR) \
			INSTALL=ccom install; \
		cd ../../pci/m68; \
		$(MAKE) -$(MAKEFLAGS) -f c2.mk LDFLAGS="$(LDFLAGS)" \
			CFLAGS="$(CFLAGS)" TESTDIR=$(TESTDIR) install; \
	fi

justinstall: make_pcc
	#
	# justinstall is just like install except that it only depends
	# on make_pcc.  This forces the C compiler interface (cc or pcc)
	# to be assumed to be up to date.  This is needed when there is
	# incompatability between the old compilation package, and the
	# new one.  justinstall must only do installs.
	#
	-if pdp11; \
	then \
		$(INS) $(TESTDIR)/pcc; \
		cd pdp11; \
		$(MAKE) -$(MAKEFLAGS) -f pcc.mk LDFLAGS="$(LDFLAGS)" \
			CFLAGS="$(CFLAGS) -UFLEXNAME" TESTDIR=$(TESTDIR) \
			INSTALL=/lib install; \
	fi
	-if vax; \
	then \
		$(INS) $(TESTDIR)/cc; \
		cd vax; \
		$(MAKE) -$(MAKEFLAGS) -f pcc.mk LDFLAGS="$(LDFLAGS)" \
			CFLAGS="$(CFLAGS)" TESTDIR=$(TESTDIR) \
			INSTALL=ccom install; \
		cd ../../c2; \
		$(MAKE) -$(MAKEFLAGS) -f c2.mk LDFLAGS="$(LDFLAGS)" \
			CFLAGS="$(CFLAGS)" TESTDIR=$(TESTDIR) install; \
	fi
	-if clipper; \
	then \
		$(INS) $(TESTDIR)/cc; \
		cd clipper; \
		$(MAKE) -$(MAKEFLAGS) -f pcc.mk LDFLAGS="$(LDFLAGS)" \
			CFLAGS="$(CFLAGS)" TESTDIR=$(TESTDIR) \
			INSTALL=comp install; \
		cd ../../pci/clipper; \
		$(MAKE) -$(MAKEFLAGS) -f c2.mk LDFLAGS="$(LDFLAGS)" \
			CFLAGS="$(CFLAGS)" TESTDIR=$(TESTDIR) install; \
	fi
	-if ns32000; \
	then \
		$(INS) $(TESTDIR)/cc; \
		cd ns32000; \
		$(MAKE) -$(MAKEFLAGS) -f pcc.mk LDFLAGS="$(LDFLAGS)" \
			CFLAGS="$(CFLAGS)" TESTDIR=$(TESTDIR) \
			INSTALL=ccom install; \
		cd ../../pci/ns32000; \
		$(MAKE) -$(MAKEFLAGS) -f c2.mk LDFLAGS="$(LDFLAGS)" \
			CFLAGS="$(CFLAGS)" TESTDIR=$(TESTDIR) install; \
	fi
	-if m68; \
	then \
		$(INS) $(TESTDIR)/cc; \
		cd m68; \
		$(MAKE) -$(MAKEFLAGS) -f pcc.mk LDFLAGS="$(LDFLAGS)" \
			CFLAGS="$(CFLAGS)" TESTDIR=$(TESTDIR) \
			INSTALL=ccom install; \
		cd ../../pci/m68; \
		$(MAKE) -$(MAKEFLAGS) -f c2.mk LDFLAGS="$(LDFLAGS)" \
			CFLAGS="$(CFLAGS)" TESTDIR=$(TESTDIR) install; \
	fi

FRC:

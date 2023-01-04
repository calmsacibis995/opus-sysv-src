
CPU=m88k
ROOT = 
STRIP = strip
INCRT = ../..
CC=cc
UTS = ../..
LIBNAME=$(UTS)/cf/m88k/lib.nfs

DASHG = 
DASHO = 
MORECPP =
PFLAGS = $(DASHG) -DINKERNEL $(MORECPP) -D$(CPU) -Dopus -DLAINFS -DCMMU
CFLAGS= -I$(INCRT) $(DASHO) -Uvax -Uu3b -Uu3b5 -Uu3b2 $(PFLAGS)
DEFLIST=
FRC =

FILES =\
	$(LIBNAME)/(au_kern.o) \
	$(LIBNAME)/(au_ux_prot.o) \
	$(LIBNAME)/(clnt_kudp.o) \
	$(LIBNAME)/(clnt_subr.o) \
	$(LIBNAME)/(klm_fcntl.o) \
	$(LIBNAME)/(klm_flock.o) \
	$(LIBNAME)/(klm_kprot.o) \
	$(LIBNAME)/(klm_lckmgr.o) \
	$(LIBNAME)/(lockd.o) \
	$(LIBNAME)/(nfs_blklst.o) \
	$(LIBNAME)/(nfs_dnlc.o) \
	$(LIBNAME)/(nfs_iget.o) \
	$(LIBNAME)/(nfs_kmem.o) \
	$(LIBNAME)/(nfs_kprot.o) \
	$(LIBNAME)/(nfs_nami.o) \
	$(LIBNAME)/(nfs_rdwri.o) \
	$(LIBNAME)/(nfs_server.o) \
	$(LIBNAME)/(nfs_subr.o) \
	$(LIBNAME)/(nfs_sys2.o) \
	$(LIBNAME)/(nfs_sys3.o \
	$(LIBNAME)/(nfs_vers.o) \
	$(LIBNAME)/(nfs_xdr.o) \
	$(LIBNAME)/(rpc_prot.o) \
	$(LIBNAME)/(subr_kudp.o) \
	$(LIBNAME)/(svc.o) \
	$(LIBNAME)/(svc_au_ux.o) \
	$(LIBNAME)/(svc_auth.o) \
	$(LIBNAME)/(svc_kudp.o) \
	$(LIBNAME)/(xdr.o) \
	$(LIBNAME)/(xdr_array.o) \
	$(LIBNAME)/(xdr_mem.o) \
	$(LIBNAME)/(nfsd.o)

all:	$(LIBNAME)

$(LIBNAME):	$(FILES)
	$(CC) -c $(CFLAGS) $(?:.o=.c)
	$(AR) rv $(LIBNAME) $?
	-rm -f $?
#	if test -z "$(ROOT)" ; \
#	then \
#		-chown bin $(LIBNAME) ; \
#		-chgrp bin $(LIBNAME) ; \
#		-chmod 664 $(LIBNAME) ; \
#	fi

.c.a:;

clean:
	-rm -f *.o

clobber:	clean
	-rm -f $(LIBNAME)



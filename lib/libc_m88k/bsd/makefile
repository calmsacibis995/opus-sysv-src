
#	Copyright (c) 1984 AT&T
#	  All Rights Reserved

#	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T
#	The copyright notice above does not evidence any
#	actual or intended publication of such source code.

#ident	"@(#)libc-m32:makefile	1.43"
#
#  makefile for libc/m32
#
#
# The variable PROF is null by default, causing profiled object to be
# maintained.  If profiled object is not desired, the reassignment
# PROF=@# should appear in the make command line.
#

.SUFFIXES:

.SUFFIXES: .o .s .c .p

ROOT=
M4=m4
#M4=$(PFX)m4
M4ARGS=m4.def reg.def
CC=$(PFX)cc
OFLAG=-O
CFLAGS=$(OFLAG) $(DEFLIST) $(SDEFLIST) $(INCLIST)
PROF=
NONPROF=
INCLIBC=../inc
INC=/usr/include
DEFLIST=
SDEFLIST=
INCLIST=-I$(INCLIBC) -I$(INC)
MAKEARGS=	CC="$(CC)" OFLAG=$(OFLAG) ROOT=$(ROOT) M4=$(M4) $(IGN) -e

OBJECTS=\
gen/__strcasecmp.o gen/gethostid.o gen/gethostname.o gen/ndbm.o gen/syslog.o	\
inet/inet_addr.o inet/inet_lnaof.o inet/inet_netof.o inet/inet_network.o	\
inet/inet_ntoa.o inet/inetmakeaddr.o net/getnetbyaddr.o net/getnetbyname.o	\
net/getnetent.o net/getproto.o net/getprotoent.o net/getprotoname.o	\
net/getservent.o net/gtservbyname.o net/gtservbyport.o net/herror.o	\
net/named/ghostnameadr.o net/named/sethostent.o net/rcmd.o net/res_comp.o	\
net/res_debug.o net/res_init.o net/res_mkquery.o net/res_query.o	\
net/res_send.o net/rexec.o net/ruserpass.o ocs/_accept.o ocs/_bsdpipe.o	\
ocs/_bind.o ocs/_connect.o ocs/_dn_comp.o ocs/_dn_expand.o ocs/_dn_skipname.o	\
ocs/_endhostent.o ocs/_endnetent.o ocs/_endprotoent.o ocs/_endservent.o	\
ocs/_fchmod.o ocs/_fchown.o ocs/_gethostent.o ocs/_gethostid.o	\
ocs/_gethostname.o ocs/_getnetent.o ocs/_getpeername.o ocs/_getprotoent.o	\
ocs/_getprotonam.o ocs/_getprotonum.o ocs/_getservent.o ocs/_getsockname.o	\
ocs/_getsockopt.o ocs/_gthstbyaddr.o ocs/_gthstbyname.o ocs/_gtnetbyaddr.o	\
ocs/_gtnetbyname.o ocs/_gtsrvbyname.o ocs/_gtsrvbyport.o ocs/_gttimeofday.o	\
ocs/_herror.o ocs/_inet_addr.o ocs/_inet_lnaof.o ocs/_inet_mkaddr.o	\
ocs/_inet_netof.o ocs/_inet_netwrk.o ocs/_inet_ntoa.o ocs/_listen.o	\
ocs/_rcmd.o ocs/_readv.o ocs/_recv.o ocs/_recvfrom.o	\
ocs/_recvmsg.o ocs/_res_init.o ocs/_res_mkquery.o ocs/_res_query.o ocs/_res_send.o	\
ocs/_res_search.o ocs/_rexec.o ocs/_rresvport.o ocs/_ruserok.o ocs/_select.o	\
ocs/_send.o ocs/_sendmsg.o ocs/_sendto.o ocs/_sethostent.o	\
ocs/_setnetent.o ocs/_setprotoent.o ocs/_setservent.o ocs/_setsockopt.o	\
ocs/_syslog.o ocs/_openlog.o ocs/_closelog.o ocs/_setlogmask.o \
ocs/_shutdown.o ocs/_socket.o ocs/_writev.o m88k/accept.o m88k/bsdpipe.o	\
m88k/bind.o m88k/connect.o m88k/fchmod.o m88k/fchown.o	\
m88k/getpeername.o m88k/getsockname.o m88k/getsockopt.o m88k/gettimeofday.o	\
m88k/listen.o m88k/readv.o m88k/recv.o m88k/recvfrom.o	\
m88k/recvmsg.o m88k/select.o m88k/send.o m88k/sendto.o m88k/sendmsg.o	\
m88k/setsockopt.o m88k/shutdown.o m88k/socket.o m88k/writev.o

POBJECTS=\
gen/__strcasecmp.p gen/gethostid.p gen/gethostname.p gen/ndbm.p	 gen/syslog.p \
inet/inet_addr.p inet/inet_lnaof.p inet/inet_netof.p inet/inet_network.p	\
inet/inet_ntoa.p inet/inetmakeaddr.p net/getnetbyaddr.p net/getnetbyname.p	\
net/getnetent.p net/getproto.p net/getprotoent.p net/getprotoname.p	\
net/getservent.p net/gtservbyname.p net/gtservbyport.p net/herror.p	\
net/named/ghostnameadr.p net/named/sethostent.p net/rcmd.p net/res_comp.p	\
net/res_debug.p net/res_init.p net/res_mkquery.p net/res_query.p	\
net/res_send.p net/rexec.p net/ruserpass.p ocs/_accept.p ocs/_bsdpipe.p	\
ocs/_bind.p ocs/_connect.p ocs/_dn_comp.p ocs/_dn_expand.p ocs/_dn_skipname.p	\
ocs/_endhostent.p ocs/_endnetent.p ocs/_endprotoent.p ocs/_endservent.p	\
ocs/_fchmod.p ocs/_fchown.p ocs/_gethostent.p ocs/_gethostid.p	\
ocs/_gethostname.p ocs/_getnetent.p ocs/_getpeername.p ocs/_getprotoent.p	\
ocs/_getprotonam.p ocs/_getprotonum.p ocs/_getservent.p ocs/_getsockname.p	\
ocs/_getsockopt.p ocs/_gthstbyaddr.p ocs/_gthstbyname.p ocs/_gtnetbyaddr.p	\
ocs/_gtnetbyname.p ocs/_gtsrvbyname.p ocs/_gtsrvbyport.p ocs/_gttimeofday.p	\
ocs/_herror.p ocs/_inet_addr.p ocs/_inet_lnaof.p ocs/_inet_mkaddr.p	\
ocs/_inet_netof.p ocs/_inet_netwrk.p ocs/_inet_ntoa.p ocs/_listen.p	\
ocs/_rcmd.p ocs/_readv.p ocs/_recv.p ocs/_recvfrom.p	\
ocs/_recvmsg.p ocs/_res_init.p ocs/_res_mkquery.p ocs/_res_query.p ocs/_res_send.p	\
ocs/_res_search.p ocs/_rexec.p ocs/_rresvport.p ocs/_ruserok.p ocs/_select.p	\
ocs/_send.p ocs/_sendmsg.p ocs/_sendto.p ocs/_sethostent.p	\
ocs/_setnetent.p ocs/_setprotoent.p ocs/_setservent.p ocs/_setsockopt.p	\
ocs/_shutdown.p ocs/_socket.p ocs/_writev.p m88k/accept.p m88k/bsdpipe.p	\
m88k/bind.p m88k/connect.p m88k/fchmod.p m88k/fchown.p	\
m88k/getpeername.p m88k/getsockname.p m88k/getsockopt.p m88k/gettimeofday.p	\
m88k/listen.p m88k/readv.p m88k/recv.p m88k/recvfrom.p	\
m88k/recvmsg.p m88k/select.p m88k/send.p m88k/sendto.p m88k/sendmsg.p	\
m88k/setsockopt.p m88k/shutdown.p m88k/socket.p m88k/writev.p

all:
	$(NONPROF)$(MAKE) $(MAKEARGS) nonprof PROF=@#
	#$(PROF)$(MAKE) $(MAKEARGS) prof NONPROF=@#

prof:	$(VERIFY) $(POBJECTS)

nonprof: $(VERIFY) $(OBJECTS) $(SYSOBJECTS) $(DOLLAROBJECTS)

.c.o:
	$(CC) $(CFLAGS) -c $*.c && mv $(*F).o $*.o

.c.p:
	$(CC) $(CFLAGS) -c -p $*.c && mv $(*F).o $*.p

.s.o:
	$(CC) $(CFLAGS) -c $*.s && mv $(*F).o $*.o

.s.p:
	$(CC) $(CFLAGS) -c $*.s && mv $(*F).p $*.p

archive:
	#
	# (m88k)

clean:
	-rm -f *.o
	-rm -f *.p

clobber: clean
	-rm -f $(OBJECTS)
	-rm -f $(POBJECTS)
	#-rm -f */*.o
	#-rm -f */*.p

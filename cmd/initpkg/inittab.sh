#	Copyright (c) 1984 AT&T
#	  All Rights Reserved

#	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T
#	The copyright notice above does not evidence any
#	actual or intended publication of such source code.

#ident	"@(#)initpkg:./inittab.sh	1.16"

if u3b2
then echo "zu::sysinit:/etc/bzapunix </dev/console >/dev/console 2>&1
fs::sysinit:/etc/bcheckrc </dev/console >/dev/console 2>&1
ck::sysinit:/etc/setclk </dev/console >/dev/console 2>&1
xdc::sysinit:sh -c 'if [ -x /etc/rc.d/0xdc ] ; then /etc/rc.d/0xdc ; fi' >/dev/console 2>&1
mt:23:bootwait:/etc/brc </dev/console >/dev/console 2>&1
mt:23:bootwait:/etc/brc </dev/console >/dev/console 2>&1
pt:23:bootwait:/etc/ports </dev/console >/dev/console 2>&1
is:2:initdefault:
p1:s1234:powerfail:/etc/led -f			# start green LED flashing
p3:s1234:powerfail:/etc/shutdown -y -i0 -g0 >/dev/console 2>&1
fl:056:wait:/etc/led -f 			# start green LED flashing
s0:056:wait:/etc/rc0 >/dev/console 2>&1 </dev/console
s1:1:wait:/etc/shutdown -y -iS -g0 >/dev/console 2>&1 </dev/console
s2:23:wait:/etc/rc2 >/dev/console 2>&1 </dev/console
s3:3:wait:/etc/rc3 >/dev/console 2>&1 </dev/console
of:0:wait:/etc/uadmin 2 0 >/dev/console 2>&1 </dev/console
fw:5:wait:/etc/uadmin 2 2 >/dev/console 2>&1 </dev/console
RB:6:wait:echo \"\\\nThe system is being restarted.\" >/dev/console 2>&1
rb:6:wait:/etc/uadmin 2 1 >/dev/console 2>&1 </dev/console
co:234:respawn:/etc/getty console console
ct:234:respawn:/etc/getty contty contty
he:234:respawn:sh -c 'sleep 60 ; exec /etc/hdelogger >/dev/console 2>&1'" \
>inittab

else if clipper || ns32000 || m88k
then echo "zu::sysinit:/etc/bzapunix </dev/console >/dev/console 2>&1
tu::sysinit:touch -c /unix </dev/console >/dev/console 2>&1
fs::sysinit:/etc/bcheckrc </dev/console >/dev/console 2>&1
Xs::sysinit:/opus/Xinit sysinit </dev/console >/dev/console 2>&1
mt:23:bootwait:/etc/brc </dev/console >/dev/console 2>&1
is:2:initdefault:
p3:s1234:powerfail:/etc/shutdown -y -i0 -g0 >/dev/console 2>&1
s0:056:wait:/etc/rc0 >/dev/console 2>&1 </dev/console
s1:1:wait:/etc/shutdown -y -iS -g0 >/dev/console 2>&1 </dev/console
s2:23:wait:/etc/rc2 >/dev/console 2>&1 </dev/console
X2:23:wait:/opus/Xinit 23 >/dev/console 2>&1 </dev/console
s3:3:wait:/etc/rc3 >/dev/console 2>&1 </dev/console
of:0:wait:/etc/uadmin 2 0 >/dev/console 2>&1 </dev/console
fw:5:wait:/etc/uadmin 2 2 >/dev/console 2>&1 </dev/console
RB:6:wait:echo \"\\\nThe system is being restarted.\" >/dev/console 2>&1
rb:6:wait:/etc/uadmin 2 1 >/dev/console 2>&1 </dev/console
co:234:respawn:/etc/getty console console
c1:234:respawn:/etc/getty con1 console
c2:234:respawn:/etc/getty con2 console
c3:234:respawn:/etc/getty con3 console" \
>inittab

else
echo "sys0::sysinit:/etc/setmrf d
sys1::sysinit:/etc/don -s tn83 0
sys2::sysinit:/etc/don -s all
is:s:initdefault:
bchk::bootwait:/etc/bcheckrc < /dev/console > /dev/console 2>&1
brc::bootwait:/etc/brc 1> /dev/console 2>&1
link::wait:(rm -f /dev/syscon;ln /dev/systty /dev/syscon) 1> /dev/console 2>&1
r0:0:wait:/etc/rc0 1> /dev/console 0>&1
r2:2:wait:/etc/rc2 1> /dev/console 2>&1
r3:3:wait:/etc/rc3 1> /dev/console 3>&1
st::off:/etc/stload > /dev/console 2>&1
pf::powerfail:/etc/powerfail 1> /dev/console 2>&1
co::respawn:/etc/getty console console" \
>inittab
fi
fi

if m88k; then
PORT=0
while [ $PORT -lt 10 ]; do
	echo "0$PORT:234:off:/etc/getty tty$PORT 9600" >> inittab
	PORT=`expr $PORT + 1`
done
while [ $PORT -lt 100 ]; do
	echo "$PORT:234:off:/etc/getty tty$PORT 9600" >> inittab
	PORT=`expr $PORT + 1`
done
fi

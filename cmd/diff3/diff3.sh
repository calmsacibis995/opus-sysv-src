
#	Copyright (c) 1984 AT&T
#	  All Rights Reserved

#	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T
#	The copyright notice above does not evidence any
#	actual or intended publication of such source code.

#ident	"@(#)diff3:diff3.sh	1.3"

e=
case $1 in
-*)
	e=$1
	shift;;
esac
if test $# = 3 -a -f $1 -a -f $2 -a -f $3
then
	:
else
	echo usage: diff3 file1 file2 file3 1>&2
	exit
fi
trap "rm -f /tmp/d3[ab]$$" 0 1 2 13 15
diff $1 $3 >/tmp/d3a$$
diff $2 $3 >/tmp/d3b$$
/usr/lib/diff3prog $e /tmp/d3[ab]$$ $1 $2 $3

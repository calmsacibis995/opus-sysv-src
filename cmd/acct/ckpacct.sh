
#	Copyright (c) 1984 AT&T
#	  All Rights Reserved

#	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T
#	The copyright notice above does not evidence any
#	actual or intended publication of such source code.

#ident	"@(#)acct:ckpacct.sh	1.4"
#       "periodically check the size of /usr/adm/pacct"
#       "if over $1 blocks (500) default, execute turnacct switch"
#       "should be run as adm"

PATH=/usr/lib/acct:/bin:/usr/bin:/etc
trap "rm -f /usr/adm/cklock*; exit 0" 0 1 2 3 9 15
export PATH

_max=${1-500}
_MIN_BLKS=500
cd /usr/adm

#	set up lock files to prevent simultaneous checking

cp /dev/null cklock
chmod 400 cklock
ln cklock cklock1
if test $? -ne 0 ; then exit 1; fi

#	If there are less than $_MIN_BLKS free blocks left on the /
#	file system, turn off the accounting (unless things improve
#	the accounting wouldn't run anyway).  If something has
#	returned the file system space, restart accounting.  This
#	feature relies on the fact that ckpacct is kicked off by the
#	cron at least once per hour.
sync;sync;

_blocks=`df / | sed "s/.*:  *\([0-9][0-9]*\) blocks.*/\1/"`

if [ "$_blocks" -lt $_MIN_BLKS   -a  -f /tmp/acctoff ];then
	echo "ckpacct: / still low on space ($_blocks blks); \c"
	echo "acctg still off"
	( echo "ckpacct: / still low on space ($_blocks blks); \c"
	echo "acctg still off" ) | mail root adm
	exit 1
elif [ "$_blocks" -lt $_MIN_BLKS ];then
	echo "ckpacct: / too low on space ($_blocks blks); \c"
	echo "turning acctg off"
	( echo "ckpacct: / too low on space ($_blocks blks); \c"
	echo "turning acctg off" ) | mail root adm
	nulladm /tmp/acctoff
	turnacct off
	exit 1
elif [ -f /tmp/acctoff ];then
	echo "ckpacct: / free space restored; turning acctg on" | \
		mail root adm
	rm /tmp/acctoff
	turnacct on
fi

_cursize="`du -s pacct | sed 's/	.*//'`"
if [ "${_max}" -lt "${_cursize}" ]; then
	turnacct switch
fi

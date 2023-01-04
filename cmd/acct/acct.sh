
#	Copyright (c) 1984 AT&T
#	  All Rights Reserved

#	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T
#	The copyright notice above does not evidence any
#	actual or intended publication of such source code.


#ident	"@(#)acct:acct.sh	1.2"

# Remove this line to enable accounting startup for state 2.
exit 0

state=$1
set `who -r`
if [ $8 != "0" ]
then
	exit
fi
case $state in

'start')
	if [ $9 = "2" -o $9 = "3" ]; then
		exit
	fi
	echo "Starting process accounting"
	/usr/lib/acct/startup
	;;
'stop')
	echo "Stopping process accounting"
	/usr/lib/acct/shutacct
	;;
'cksize')
	for i in `ls /usr/adm/pacct*`; do
		SIZE=`ls -s $i`
		set -- $SIZE
		SIZE=$1
		if [ $SIZE -gt 1000 ]; then
			mail root <<!
Message from cron - $i has grown to $SIZE blocks.
!
		fi
	done
	i=/usr/lib/cron/log
	SIZE=`ls -s $i`
	set -- $SIZE
	SIZE=$1
	if [ $SIZE -gt 100 ]; then
		mail root <<!
Message from cron - $i has grown to $SIZE blocks.
!
	fi
	;;
esac

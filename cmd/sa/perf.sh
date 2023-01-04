
#	Copyright (c) 1984 AT&T
#	  All Rights Reserved

#	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T
#	The copyright notice above does not evidence any
#	actual or intended publication of such source code.

# Remove this line to enable system performance analysis
# startup for state 2.
exit 0

#ident	"@(#)sa:perf.sh	1.3"
MATCH=`who -r|grep -c "[234][	 ]*0[	 ]*[S1]"`
if [ ${MATCH} -eq 1 ]
then
	su sys -c "/usr/lib/sa/sadc /usr/adm/sa/sa`date +%d`"
fi

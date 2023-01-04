
#	Copyright (c) 1984 AT&T
#	  All Rights Reserved

#	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T
#	The copyright notice above does not evidence any
#	actual or intended publication of such source code.

#ident	"@(#)adm:filesave.sh	1.3"

#	The following prototype is meant as a guide in setting
#	up local filesave procedures.  It copies the root
#	file system from drive 0 to cartridge tape.

if [ $# != 1 ]
then
	echo usage: filesave packname
	exit 2
fi
don mhd 1
date >> /etc/log/filesave.log
volcopy root /dev/rdsk/c1d0s0 - /dev/rmt/0m $1
echo FILESAVE COMPLETED >> /etc/log/filesave.log
doff mhd 1

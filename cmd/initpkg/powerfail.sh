
#	Copyright (c) 1984 AT&T
#	  All Rights Reserved

#	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T
#	The copyright notice above does not evidence any
#	actual or intended publication of such source code.

#ident	"@(#)initpkg:./powerfail.sh	1.1"
if $1/bin/u3b2 || $1/bin/clipper || $1/bin/ns32000 || $1/bin/m88k
then echo "

#	powerfail
" >powerfail
fi

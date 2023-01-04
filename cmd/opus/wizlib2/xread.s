#	Copyright (c) 1984 AT&T
#	  All Rights Reserved

#	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T
#	The copyright notice above does not evidence any
#	actual or intended publication of such source code.

# C library -- read
.ident	"@(#)libc-m32:sys/read.s	1.3	1.3 (Fairchild) 9/27/86"

# nread = read(file, buffer, count);
# nread ==0 means eof; nread == -1 means error

	.set	read,3

	.globl	_XREAD
	.globl  cerror

_XREAD:
	loadw	4(sp),r2
	calls	$read
	bc 	cerror
.noerror:
	ret	sp

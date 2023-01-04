#	Copyright (c) 1984 AT&T
#	  All Rights Reserved

#	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T
#	The copyright notice above does not evidence any
#	actual or intended publication of such source code.

# C library -- open
.ident	"@(#)libc-m32:sys/open.s	1.3	1.3 (Fairchild) 9/27/86"

# file = open(string, mode)

# file == -1 means error

	.set	open,5

	.globl	_XOPEN
	.globl  cerror

_XOPEN:
	loadw	4(sp),r2
	calls	$open
	bc 	cerror
.noerror:
	ret	sp

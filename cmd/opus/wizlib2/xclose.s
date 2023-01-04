#	Copyright (c) 1984 AT&T
#	  All Rights Reserved

#	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T
#	The copyright notice above does not evidence any
#	actual or intended publication of such source code.

# C library -- close
.ident	"@(#)libc-m32:sys/close.s	1.3	1.3 (Fairchild) 9/27/86"

# error =  close(file);

	.set	close,6

	.globl	_XCLOSE
	.globl	cerror

_XCLOSE:
	calls	$close
	bc 	cerror
.noerror:
	loadq	$0,r0
	ret	sp

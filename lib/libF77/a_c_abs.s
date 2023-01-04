#	Copyright (c) 1984 AT&T
#	Copyright (c) 1987 Fairchild Semiconductor Corporation
#	  All Rights Reserved

#	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T
#	AND FAIRCHILD SEMICONDUCTOR CORPORATION
#	The copyright notice above does not evidence any
#	actual or intended publication of such source code.

	.ident	"@(#)libF77:a_c_abs.s		1.3 (Fairchild) 1/22/87"
	.file	"a_c_abs.s"
	.ident	"@(#)libF77:c_abs.c	1.3	1.1 (Fairchild) 6/11/86"
	.ident	"@(#)libF77:complex	1.3	1.1 (Fairchild) 6/11/86"
	.text
	.align	2
	.globl	_a_c_abs
_a_c_abs:
	subw	$4,sp

	loads	4(r0),f0
	cnvsd	f0,f0
	movd	f0,f1
	loads	(r0),f0
	cnvsd	f0,f0
	call	sp,_cabs
	cnvds	f0,f0

	addw	$4,sp
	ret	sp
	.data

	.data

#_z	r0	local
	.text
	.data

	.data

	.text

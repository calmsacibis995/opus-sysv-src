#	Copyright (c) 1984 AT&T
#	Copyright (c) 1987 Fairchild Semiconductor Corporation
#	  All Rights Reserved

#	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T
#	AND FAIRCHILD SEMICONDUCTOR CORPORATION
#	The copyright notice above does not evidence any
#	actual or intended publication of such source code.

	.ident	"@(#)libF77:a_r_int.s		1.3 (Fairchild) 1/22/87"
	.file	"a_r_int.s"
	.ident	"@(#)libF77:r_int.c	1.3	1.1 (Fairchild) 6/11/86"
	.text
	.align	2
	.globl	_a_r_int
_a_r_int:
	subw	$4,sp

	loads	(r0),f0
	cnvsd	f0,f0
	loadd	.L7,f1
	cmpd	f1,f0
	bcge	.L4
	loads	(r0),f0
	cnvsd	f0,f0
	call	sp,_floor
	b	.L2
.L4:
	loads	(r0),f0
	cnvsd	f0,f0
	negd	f0,f0
	call	sp,_floor
	negd	f0,f0
.L2:

	cnvds	f0,f0
	addw	$4,sp
	ret	sp
	.data
	.align	8
	.align	8
.L7:	.double	0f0.000000000000000000

	.data

#_x	r0	local
	.text
	.data

	.data

	.text

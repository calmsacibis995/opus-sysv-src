#	Copyright (c) 1984 AT&T
#	Copyright (c) 1987 Fairchild Semiconductor Corporation
#	  All Rights Reserved

#	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T
#	AND FAIRCHILD SEMICONDUCTOR CORPORATION
#	The copyright notice above does not evidence any
#	actual or intended publication of such source code.

	.ident	"@(#)libF77:a_pow_ri.s		1.3 (Fairchild) 1/22/87"
	.file	"a_pow_ri.s"
	.ident	"@(#)libF77:pow_ri.c	1.3	1.1 (Fairchild) 6/11/86"
	.text
	.align	2
	.globl	_a_pow_ri
_a_pow_ri:
	subw	$4,sp
	saved7

	loadd	.L18,f7
	loads	(r0),f1
	cnvsd	f1,f1
	loadw	(r1),r1
	cmpw	$0,r1
	bceq	.L6
	cmpw	$0,r1
	bcle	.L12
	loadd	.L19,f0
	cmpd	f1,f0
	bcne	.L13
	b	.L22
.L13:
	negw	r1,r1
	loadd	.L20,f0
	divd	f1,f0
	movd	f0,f1
.L12:
.L11:
	movw	r1,r0
	andw	$1,r0
	bceq	.L9
	muld	f1,f7
.L9:
	shaw	$-1,r1
	bceq	.L6
	muld	f1,f1
	b	.L11
.L6:
.L22:
	cnvds	f7,f0
.L1:

	restd7
	addw	$4,sp
	ret	sp
	.data
	.align	8
	.align	8
.L20:	.double	0f1.000000000000000000
	.align	8
	.align	8
.L19:	.double	0f0.000000000000000000
	.align	8
	.align	8
.L18:	.double	0f1.000000000000000000

#_pow	f7	local
#_x	f1	local
#_n	r1	local
	.data

#_ap	r0	local
#_bp	r1	local
	.text
	.data

	.data

	.text

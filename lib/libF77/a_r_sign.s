#	Copyright (c) 1984 AT&T
#	Copyright (c) 1987 Fairchild Semiconductor Corporation
#	  All Rights Reserved

#	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T
#	AND FAIRCHILD SEMICONDUCTOR CORPORATION
#	The copyright notice above does not evidence any
#	actual or intended publication of such source code.

	.ident	"@(#)libF77:a_r_sign.s		1.3 (Fairchild) 1/22/87"
	.file	"a_r_sign.s"
	.ident	"@(#)libF77:r_sign.c	1.3	1.1 (Fairchild) 6/11/86"
	.text
	.align	2
	.globl	_a_r_sign
_a_r_sign:
	subw	$4,sp
	saved7

	loads	(r0),f0
	cnvsd	f0,f0
	loadd	.L10,f1
	cmpd	f1,f0
	bcgt	.L7
	loads	(r0),f0
	cnvsd	f0,f0
	b	.L5
.L7:
	loads	(r0),f0
	cnvsd	f0,f0
	negd	f0,f0
.L5:
	cnvds	f0,f1
	loads	(r1),f0
	cnvsd	f0,f0
	loadd	.L11,f7
	cmpd	f7,f0
	bcgt	.L4
	movs	f1,f0
	b	.L2
.L4:
	movs	f1,f0
	negs	f0,f0
.L2:

	restd7
	addw	$4,sp
	ret	sp
	.data
	.align	8
	.align	8
.L11:	.double	0f0.000000000000000000
	.align	8
	.align	8
.L10:	.double	0f0.000000000000000000

#_x	f1	local
	.data

#_a	r0	local
#_b	r1	local
	.text
	.data

	.data

	.text

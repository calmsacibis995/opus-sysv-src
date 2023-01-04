#	Copyright (c) 1984 AT&T
#	Copyright (c) 1987 Fairchild Semiconductor Corporation
#	  All Rights Reserved

#	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T
#	AND FAIRCHILD SEMICONDUCTOR CORPORATION
#	The copyright notice above does not evidence any
#	actual or intended publication of such source code.

	.ident	"@(#)libF77:a_r_mod.s		1.3 (Fairchild) 1/22/87"
	.file	"a_r_mod.s"
	.ident	"@(#)libF77:r_mod.c	1.3	1.1 (Fairchild) 6/11/86"
	.text
	.align	2
	.globl	_a_r_mod
_a_r_mod:
	subw	$4,sp
	savew13
	saved7
	movw	r0,fp
	movw	r1,r13

	loadd	.L7,f0
	loads	(r13),f7
	cnvsd	f7,f7
	loads	(fp),f1
	cnvsd	f1,f1
	divd	f7,f1
	cmpd	f1,f0
	bclt	.L4
	movd	f1,f0
	call	sp,_floor
	movd	f0,f1
	b	.L2
.L4:
	negd	f1,f0
	call	sp,_floor
	negd	f0,f1
.L2:
	loads	(r13),f7
#	cnvsd	f7,f7
#	muld	f1,f7
	cnvds	f1,f1
	muls	f1,f7
	loads	(fp),f0
#	cnvsd	f0,f0
#	subd	f7,f0
	subs	f7,f0

	restd7
	restw13
	addw	$4,sp
	ret	sp
	.data
	.align	8
	.align	8
.L7:	.double	0f0.000000000000000000

#_quotient	f1	local
	.data

#_x	fp	local
#_y	r13	local
	.text
	.data

	.data

	.text

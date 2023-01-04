#	Copyright (c) 1984 AT&T
#	Copyright (c) 1987 Fairchild Semiconductor Corporation
#	  All Rights Reserved

#	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T
#	AND FAIRCHILD SEMICONDUCTOR CORPORATION
#	The copyright notice above does not evidence any
#	actual or intended publication of such source code.

	.ident	"@(#)libF77:a_r_dim.s		1.3 (Fairchild) 1/22/87"
	.file	"a_r_dim.s"
	.ident	"@(#)libF77:r_dim.c	1.3	1.1 (Fairchild) 6/11/86"
	.text
	.align	2
	.globl	_a_r_dim
_a_r_dim:

	loads	(r1),f0
	loads	(r0),f1
	cmps	f1,f0
	bcle	.L4
	loads	(r0),f0
	loads	(r1),f1
	subs	f1,f0
	b	.L2
.L4:
	loads	.L7,f0
.L2:

	ret	sp
	.data
	.align	8
	.align	8
.L7:	.float	0f0.0000

	.data

#_a	r0	local
#_b	r1	local
	.text
	.data

	.data

	.text

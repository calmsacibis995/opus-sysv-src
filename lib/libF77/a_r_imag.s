#	Copyright (c) 1984 AT&T
#	Copyright (c) 1987 Fairchild Semiconductor Corporation
#	  All Rights Reserved

#	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T
#	AND FAIRCHILD SEMICONDUCTOR CORPORATION
#	The copyright notice above does not evidence any
#	actual or intended publication of such source code.

	.ident	"@(#)libF77:a_r_imag.s		1.3 (Fairchild) 1/22/87"
	.file	"a_r_imag.s"
	.ident	"@(#)libF77:r_imag.c	1.3	1.1 (Fairchild) 6/11/86"
	.ident	"@(#)libF77:complex	1.3	1.1 (Fairchild) 6/11/86"
	.text
	.align	2
	.globl	_a_r_imag
_a_r_imag:

	loads	4(r0),f0
#	cnvsd	f0,f0

	ret	sp
	.data

	.data

#_z	r0	local
	.text
	.data

	.data

	.text

;	@(#)__abs.s	6.1 
;
;	M78000 __abs(3) Routine
;
;	(C) Copyright 1987 by Motorola Inc.
;
;	Written by: Bob Greiner
;
; __abs - integer absolute value
;
; Input:	i, an integer
;
; Output:	(unsigned int) ( (i<0) ? -i : i )
;
	file	"__abs.s"
	text
	global	__abs
;
__abs:
	bcnd	ge0,arg0,absl
	subu	arg0,zero,arg0
absl:
	jmp	return

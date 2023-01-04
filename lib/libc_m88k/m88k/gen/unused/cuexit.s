;	@(#)cuexit.s	6.1 
;
;	M78000 exit(3) Routine
;
;	(C) Copyright 1987 by Motorola Inc.
;
;	Written by: Bob Greiner
;
; exit - exit back to operating system
;
; Input:	code, to be returned to the system
;
; Output:	NEVER RETURNS
;
_m4_define_(`trap_exit', 1)_m4_dnl_
_m4_define_(`m_space', `sp,16')_m4_dnl_
_m4_define_(`m_code', `sp,0')_m4_dnl_
;
	file	"cuexit.s"
	text
	global	_exit
	global	__cleanup
;
_exit:
	subu	sp,m_space		; allocate stack frame
	st	arg0,m_code		; save return code
	bsr	__cleanup		; allow clean up
	or	arg0,zero,trap_exit	; trap number
	ld	arg1,m_code		; restore return code
	addu	sp,m_space		; remove stack frame for neatness
	tb0	0,zero,0		; invoke operating system

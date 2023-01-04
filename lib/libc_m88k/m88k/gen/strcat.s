;	@(#)__strcat.s	6.1	
;
;	M78000 __strcat(3C) Routine
;
;	(C) Copyright 1987 by Motorola Inc.
;
;	Written by: Bob Greiner
;
; __strcat - concatinates sorc onto the end of dest.
;	dest must be big enough.
;
; Input:	dest - destination
;		sorc  - source
;
; Output:	dest
;
; Register usage:
;
_m4_define_(`dest', arg0)_m4_dnl_		; destination pointer
_m4_define_(`sorc', arg1)_m4_dnl_		; source pointer
_m4_define_(`dln', r5)_m4_dnl_		; length of destination string
;
_m4_define_(`m_return', `sp,0')_m4_dnl_
_m4_define_(`m_dest', `sp,4')_m4_dnl_
_m4_define_(`m_sorc', `sp,8')_m4_dnl_
_m4_define_(`m_dln', `sp,12')_m4_dnl_
_m4_define_(`m_space', `sp,16')_m4_dnl_	; size of stack space
;
	file	"__strcat.s"
	text
	global	__strcat
	global	_strlen
	global	_memcpy
__strcat:
	MCOUNT
	subu	sp,m_space		; create save space
	st	return,m_return
	st	dest,m_dest
	st	sorc,m_sorc
	bsr	_strlen			; dln = strlen (dest)
	st	result,m_dln
	ld	arg0,m_sorc
	bsr	_strlen			; sln = strlen (sorc)
	addu	arg2,result,1
	ld	arg1,m_sorc
	ld	arg0,m_dest
	ld	dln,m_dln
	addu	arg0,arg0,dln
	bsr	_memcpy			; memcpy (dest+dln, sorc, sln+1)
	ld	result,m_dest
	ld	return,m_return		; return dest
	jmp.n	return
	addu	sp,m_space

;	@(#)__strncat.s	6.1	
;
;	M78000 __strncat(3C) Routine
;
;	(C) Copyright 1987 by Motorola Inc.
;
;	Written by: Bob Greiner
;
; __strncat - concatinates sorc onto the end of dest.
;	at most maxl bytes are moved.
;	a null is always moved.
;	dest must be big enough.
;
; Input:	dest - destination
;		sorc  - source
;		maxl  - length
;
; Output:	dest
;
; Register usage:
;
_m4_define_(`dest', arg0)_m4_dnl_		; destination pointer
_m4_define_(`sorc', arg1)_m4_dnl_		; source pointer
_m4_define_(`maxl', arg2)_m4_dnl_		; maximum length to move
_m4_define_(`dln', r5)_m4_dnl_		; length of destination string
_m4_define_(`temp', r6)_m4_dnl_
;
_m4_define_(`m_return', `sp,0')_m4_dnl_
_m4_define_(`m_dest', `sp,4')_m4_dnl_
_m4_define_(`m_sorc', `sp,8')_m4_dnl_
_m4_define_(`m_maxl', `sp,12')_m4_dnl_
_m4_define_(`m_dln', `sp,16')_m4_dnl_
_m4_define_(`m_space', `sp,32')_m4_dnl_	; size of stack space
;
	file	"__strncat.s"
	text
	global	__strncat
	global	_strlen
	global	_memcpy
__strncat:
	MCOUNT
	subu	sp,m_space		; create save space
	st	return,m_return
	st	dest,m_dest
	st	sorc,m_sorc
	st	maxl,m_maxl
	bsr	_strlen			; dln = strlen (dest)
	st	result,m_dln
	ld	arg0,m_sorc
	bsr	_strlen			; sln = strlen (sorc)
	ld	arg2,m_maxl		; maxl = max( maxl, sln)
	cmp	temp,result,arg2
	bb0	le,temp,ok
	or	arg2,zero,result
ok:
	ld	arg1,m_sorc
	ld	arg0,m_dest
	ld	dln,m_dln
	addu	arg0,arg0,dln
	addu	temp,arg0,arg2		; temp = dest+dln+maxl
	st	temp,m_dln
	bsr	_memcpy			; memcpy (dest+dln, sorc, maxl)
	ld	temp,m_dln
	st.b	r0,temp,r0		; temp->byte = '\0'
	ld	result,m_dest
	ld	return,m_return		; return dest
	jmp.n	return
	addu	sp,m_space

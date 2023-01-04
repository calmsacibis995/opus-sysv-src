;	@(#)__strncpy.s	6.1	
;
;	M78000 __strncpy(3C) Routine
;
;	(C) Copyright 1987 by Motorola Inc.
;
;	Written by: Bob Greiner
;
; __strncpy - copy sorc onto dest
;	exactly ln bytes are moved
;	null bytes are appended to dest if necessary to fill it
;
; Input:	dest - destination
;		sorc  - source
;		ln    - length
;
; Output:	dest
;
; Register usage:
;
_m4_define_(`dest', arg0)_m4_dnl_		; destination pointer
_m4_define_(`sorc', arg1)_m4_dnl_		; source pointer
_m4_define_(`dln', arg2)_m4_dnl_		; length to move
_m4_define_(`temp', r5)_m4_dnl_
;
_m4_define_(`m_return', `sp,0')_m4_dnl_
_m4_define_(`m_dest', `sp,4')_m4_dnl_
_m4_define_(`m_sorc', `sp,8')_m4_dnl_
_m4_define_(`m_dln', `sp,12')_m4_dnl_
_m4_define_(`m_sln', `sp,16')_m4_dnl_
_m4_define_(`m_space', `sp,32')_m4_dnl_	; size of stack space
;
	file	"__strncpy.s"
	text
	global	__strncpy
	global	_memcpy
__strncpy:
	MCOUNT
	subu	sp,m_space
	st	return,m_return
	st	dest,m_dest
	st	sorc,m_sorc
	st	dln,m_dln
	or	arg0,zero,sorc	; sln = strlen (sorc)
	bsr	_strlen
	st	result,m_sln
	ld	dln,m_dln
	cmp	temp,result,dln	; if sln <= dln then
	bb0	le,temp,toobig
	or	arg2,zero,result; memcpy (dest, sorc, sln)
	ld	arg1,m_sorc
	ld	arg0,m_dest
	bsr	_memcpy
	ld	arg2,m_dln	; memset (dest+sln, 0, dln-sln)
	ld	temp,m_sln
	subu	arg2,arg2,temp
	or	arg1,zero,zero
	ld	arg0,m_dest
	addu	arg0,arg0,temp
	bsr	_memset
	br	done
toobig:				; else
	ld	arg1,m_sorc	; memcpy (dest, sorc, dln)
	ld	arg0,m_dest	; -- arg2 already contains dln
	bsr	_memcpy
done:				; end if
	ld	result,m_dest	; return dest
	ld	return,m_return
	jmp.n	return
	addu	sp,m_space

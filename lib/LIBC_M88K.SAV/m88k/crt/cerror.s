;	@(#)cerror.s	6.1 
; C return sequence which
; sets errno, returns -1.

;	file	"cerror.s"
	global	cerror
	global	_errno
; MV this routine used register r30, changed it to r13.

	data
_errno:	word	0

	text
cerror:
	MCOUNT
;	st	r2,r0,_errno
	or.u	r13,r0,hi16(_errno) 	; temporary kludge 880219saj
     	or 	r13,r13,lo16(_errno) 	; temporary kludge 880219saj
	st	r2,r0,r13 		; temporary kludge 880219saj
	jmp.n	r1
	or.c	r2,r0,r0

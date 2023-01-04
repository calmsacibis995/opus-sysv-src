;	@(#)__memchr.s	6.1	
;
;	M78000 __memchr(3C) Routine
;
;	(C) Copyright 1987 by Motorola Inc.
;
;	Written by: Bob Greiner
;
; __memchr - find char in sorc; otherwise return 0
;
; Input:	sorc  - source
;		char  - character
;		ln    - maximum length of sorc
;
; Output:	(*sorc==char) ? sorc : (char *) 0
;
; Register usage:
;
_m4_define_(`sorc', arg0)_m4_dnl_		; string to search
_m4_define_(`char', arg1)_m4_dnl_		; character to match
_m4_define_(`ln', arg2)_m4_dnl_		; maximum length to search
_m4_define_(`data', r5)_m4_dnl_		; signed character from string
_m4_define_(`temp', r6)_m4_dnl_		; temp register
;
	file	"__memchr.s"
	text
	global	__memchr
__memchr:
	MCOUNT
loop:				; loop
	bcnd	le0,ln,nochr	; when ln-- <= 0 exit
	ld.b	data,sorc,0	; data = sorc->byte
	subu	ln,ln,1
	cmp	temp,data,char	; if data == char then
	bb0.n	eq,temp,loop	; return sorc
	addu	sorc,sorc,1	; sorc += 1
	subu	sorc,sorc,1	; else exit loop
	br	found		; end if
nochr:				; end loop
	or	result,zero,zero; return 0
found:
	jmp	return

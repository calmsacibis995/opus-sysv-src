;	@(#)__strchr.s	6.1	
;
;	M78000 __strchr(3C) Routine
;
;	(C) Copyright 1987 by Motorola Inc.
;
;	Written by: Bob Greiner
;
; __strchr - find char in sorc; otherwise return 0
;
; Input:	sorc  - source
;		char  - character
;
; Output:	(*sorc==char) ? sorc : (char *) 0
;
; Register usage:
;
_m4_define_(`sorc', arg0)_m4_dnl_		; string to search
_m4_define_(`char', arg1)_m4_dnl_		; character to match
_m4_define_(`data', r4)_m4_dnl_		; signed character from string
_m4_define_(`temp', r5)_m4_dnl_		; temp register
;
	file	"__strchr.s"
	text
	global	__strchr
	global	_memcpy
__strchr:
	MCOUNT
loop:				; loop
	ld.b	data,sorc,0	; data = sorc->byte
	cmp	temp,data,char	; if data == char then
	bb1	eq,temp,found	; return sorc
	bcnd.n	ne0,data,loop	; else if data != 0 then
	addu	sorc,sorc,1	; sorc += 1
				; else exit loop
				; end if
				; end loop
	or	result,zero,zero; return 0
found:
	jmp	return

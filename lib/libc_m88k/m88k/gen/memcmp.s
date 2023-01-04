;	@(#)__memcmp.s	6.1	
;
;	M78000 __memcmp(3C) Routine
;
;	(C) Copyright 1987 by Motorola Inc.
;
;	Written by: Bob Greiner
;
; __memcmp - compare strings (of exactly ln bytes)
;
; Input:	s1 -- first string
;		s2 -- second string
;		ln -- maximum length of strings
;
; Output:	s1==s2	0
;		s1<s2	-
;		s1>s2	+
;
; Register usage:
;
_m4_define_(`s2', arg1)_m4_dnl_		; source pointer
_m4_define_(`ln', arg2)_m4_dnl_		; maximum length to move
_m4_define_(`s1', r5)_m4_dnl_		; string 1 address
_m4_define_(`d1', r6)_m4_dnl_		; string 1 data
_m4_define_(`d2', r7)_m4_dnl_		; string 2 data
_m4_define_(`temp', result)_m4_dnl_	; temporary register
;
	file	"__memcmp.s"
	text
	global	__memcmp
__memcmp:
	MCOUNT
	bcnd	le0,ln,equal	; if ln <= 0
	or	s1,arg0,zero
	cmp	temp,s1,s2	; or s1==s2 then
	bb1	eq,temp,equal	; return 0
				; else
loop:				; loop
	ld.b	d1,s1,0		; d1 = s1->byte
	ld.b	d2,s2,0		; d2 = s2->byte
	subu	temp,d1,d2	; if d1 != d2 then
	bcnd	ne0,temp,noteq	; return d1 - d2
				; else
	addu	s1,s1,1		; s1 += 1
	addu	s2,s2,1		; s2 += 1
	subu	ln,ln,1		; ln -= 1
	bcnd	gt0,ln,loop	; when ln == 0 exit 
equal:				; end loop
	or	result,zero,zero; return 0
noteq:
	jmp	return		; end if

;	@(#)__strlen.s	6.1	
;
;	M78000 __strlen(3C) Routine
;
;	(C) Copyright 1987 by Motorola Inc.
;
;	Written by: Bob Greiner
;
; __strlen - returns the number of characters in s, not including the
;	   terminating null character
;
; Input:	s - the string in question
;		no word is accessed except in the string;
;		this eliminates spurious memory faults
;
; Output:	the length of string "s" 
;
; Timing:	the inner loop takes 10 instructions + 2 bubbles
;		for 4 bytes if the string is in the cache
;
;		The bubble is between the ld and the first extu.
;
_m4_define_(`s', `arg0')_m4_dnl_			; the string pointer
_m4_define_(`sw', `r4')_m4_dnl_			; s, word aligned
_m4_define_(`data', `r3')_m4_dnl_			; data = sw->word
_m4_define_(`temp', `r5')_m4_dnl_
_m4_define_(`byte0', `8<24>')_m4_dnl_
_m4_define_(`byte1', `8<16>')_m4_dnl_
_m4_define_(`byte2', `8<8>')_m4_dnl_
_m4_define_(`byte3', `8<0>')_m4_dnl_
;
	file	"__strlen.s"
	text
	global	__strlen
;
__strlen:
	MCOUNT
	and	sw,s,0xFFFC		; form word address
	ld	data,sw,0			; read first word
	bb1	01,s,L0		; branch on starting byte
	bb0	00,s,L10
	bb1	00,s,L11
L0:
	bb0	00,s,L12
	bb1	00,s,L13
L1:				; for each word in string
	ld	data,sw,0			; read next word
L10:
	extu	temp,data,byte0		; extract byte 0
	bcnd	eq0,temp,L20		; test for '\0'
L11:
	extu	temp,data,byte1		; extract byte 1
	bcnd	eq0,temp,L21		; test for '\0'
L12:
	extu	temp,data,byte2		; extract byte 2
	bcnd	eq0,temp,L22		; test for '\0'
L13:
	extu	temp,data,byte3		; extract byte 3
	bcnd.n	ne0,temp,L1		; test for '\0'
L23:				; after '\0' found
	addu	sw,sw,4			; bump address
	subu	sw,sw,3			; reform byte address
L22:
	addu	sw,sw,1			; reform byte address
L21:
	addu	sw,sw,1			; reform byte address
L20:				; sw has exact byte address of '\0'
	jmp.n	return			; return from subroutine
	subu	result,sw,s		; addr('\0') - orig addr

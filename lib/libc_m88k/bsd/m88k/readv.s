;	M88000 readv(2) Routine
;
;
	file	"readv.s"
	text
	global	__readv
__readv:
	or	r8,r0,0x1		; network vendor id
	or	r9,r0,17	       	; network function id
	tb0	0,r0,449		; trap to Unix
	br	cerror			; return error to caller
	jmp	r1			; return to caller

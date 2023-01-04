;	M88000 listen(2) Routine
;
;
	file	"listen.s"
	text
	global	__listen
__listen:
	or	r8,r0,0x1		; network vendor id
	or	r9,r0,4		       	; network function id
	tb0	0,r0,449		; trap to Unix
	br	cerror			; return error to caller
	jmp	r1			; return to caller

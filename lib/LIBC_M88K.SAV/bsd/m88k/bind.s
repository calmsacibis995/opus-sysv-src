;	M88000 bind(2) Routine
;
;
	file	"bind.s"
	text
	global	__bind
__bind:
	or	r8,r0,0x1		; network vendor id
	or	r9,r0,2		       	; network function id
	tb0	0,r0,449		; trap to Unix
	br	cerror			; return error to caller
	jmp	r1			; return to caller

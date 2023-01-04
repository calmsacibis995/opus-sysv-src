;	M88000 send(2) Routine
;
;
	file	"send.s"
	text
	global	__send
__send:
	or	r8,r0,0x1		; network vendor id
	or	r9,r0,8		       	; network function id
	tb0	0,r0,449		; trap to Unix
	br	cerror			; return error to caller
	jmp	r1			; return to caller

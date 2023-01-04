;	M88000 connect(2) Routine
;
;
	file	"connect.s"
	text
	global	__connect
__connect:
	or	r8,r0,0x1		; network vendor id
	or	r9,r0,3		       	; network function id
	tb0	0,r0,449		; trap to Unix
	br	cerror			; return error to caller
	jmp	r1			; return to caller

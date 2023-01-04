;	M88000 setsockopt(2) Routine
;
;
	file	"setsockopt.s"
	text
	global	__setsockopt
__setsockopt:
	or	r8,r0,0x1		; network vendor id
	or	r9,r0,15	       	; network function id
	tb0	0,r0,449		; trap to Unix
	br	cerror			; return error to caller
	jmp	r1			; return to caller

;	M88000 getsockopt(2) Routine
;
;
	file	"getsockopt.s"
	text
	global	__getsockopt
__getsockopt:
	or	r8,r0,0x1		; network vendor id
	or	r9,r0,16	       	; network function id
	tb0	0,r0,449		; trap to Unix
	br	cerror			; return error to caller
	jmp	r1			; return to caller

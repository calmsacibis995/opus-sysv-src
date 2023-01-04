;	M88000 getsockname(2) Routine
;
;
	file	"getsockname.s"
	text
	global	__getsockname
__getsockname:
	or	r8,r0,0x1		; network vendor id
	or	r9,r0,6		       	; network function id
	tb0	0,r0,449		; trap to Unix
	br	cerror			; return error to caller
	jmp	r1			; return to caller

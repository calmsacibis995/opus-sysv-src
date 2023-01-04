;	M88000 fchown(2) Routine
;
;
	file	"fchown.s"
	text
	global	__fchown
__fchown:
	or	r8,r0,0x1		; network vendor id
	or	r9,r0,26	       	; network function id
	tb0	0,r0,449		; trap to Unix
	br	cerror			; return error to caller
	jmp	r1			; return to caller

;	M88000 fchmod(2) Routine
;
;
	file	"fchmod.s"
	text
	global	__fchmod
__fchmod:
	or	r8,r0,0x1		; network vendor id
	or	r9,r0,25	       	; network function id
	tb0	0,r0,449		; trap to Unix
	br	cerror			; return error to caller
	jmp	r1			; return to caller

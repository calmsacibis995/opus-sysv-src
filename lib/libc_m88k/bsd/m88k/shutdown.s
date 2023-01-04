;	M88000 shutdown(2) Routine
;
;
	file	"shutdown.s"
	text
	global	__shutdown
__shutdown:
	or	r8,r0,0x1		; network vendor id
	or	r9,r0,14	       	; network function id
	tb0	0,r0,449		; trap to Unix
	br	cerror			; return error to caller
	jmp	r1			; return to caller

;	M88000 select(2) Routine
;
;
	file	"select.s"
	text
	global	__select
__select:
	or	r8,r0,0x1		; network vendor id
	or	r9,r0,20	       	; network function id
	tb0	0,r0,449		; trap to Unix
	br	cerror			; return error to caller
	jmp	r1			; return to caller

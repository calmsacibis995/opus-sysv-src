;	M88000 recvfrom(2) Routine
;
;
	file	"recvfrom.s"
	text
	global	__recvfrom
__recvfrom:
	or	r8,r0,0x1		; network vendor id
	or	r9,r0,12	       	; network function id
	tb0	0,r0,449		; trap to Unix
	br	cerror			; return error to caller
	jmp	r1			; return to caller

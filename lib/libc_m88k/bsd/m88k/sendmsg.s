;	M88000 sendmsg(2) Routine
;
;
	file	"sendmsg.s"
	text
	global	__sendmsg
__sendmsg:
	or	r8,r0,0x1		; network vendor id
	or	r9,r0,10	       	; network function id
	tb0	0,r0,449		; trap to Unix
	br	cerror			; return error to caller
	jmp	r1			; return to caller

;	M88000 recvmsg(2) Routine
;
;
	file	"recvmsg.s"
	text
	global	__recvmsg
__recvmsg:
	or	r8,r0,0x1		; network vendor id
	or	r9,r0,13	       	; network function id
	tb0	0,r0,449		; trap to Unix
	br	cerror			; return error to caller
	jmp	r1			; return to caller

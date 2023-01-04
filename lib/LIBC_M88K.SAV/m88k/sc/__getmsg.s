;	M88000 __getmsg(2) Routine
;
	file	"__getmsg.s"
	text
	global	__getmsg
__getmsg:
	MCOUNT
	;tb0	0,r0,256+85		; trap to Unix
	or	r9,r0,18
	tb0	0,r0,450		; trap to Unix
	br	cerror			; return error to caller
	jmp	r1			; return to caller

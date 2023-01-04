;	M88000 __putmsg(2) Routine
;
	file	"__putmsg.s"
	text
	global	__putmsg
__putmsg:
	MCOUNT
	;tb0	0,r0,256+86		; trap to Unix
	or	r9,r0,39
	tb0	0,r0,450		; trap to Unix
	br	cerror			; return error to caller
	jmp	r1			; return to caller

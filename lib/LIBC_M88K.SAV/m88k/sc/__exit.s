;	M88000 __exit(2) Routine
;
	file	"__exit.s"
	text
	global	__exit
__exit:
	MCOUNT
	;tb0	0,r0,256+1		; trap to Unix
	or	r9,r0,9
	tb0	0,r0,450		; trap to Unix
	br	cerror			; return error to caller
	div	r0,r0,r0		; Should never get here.

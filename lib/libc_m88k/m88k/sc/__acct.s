;	M88000 __acct(2) Routine
;
	file	"__acct.s"
	text
	global	__acct
__acct:
	MCOUNT
	;tb0	0,r0,256+51		; trap to Unix
	or	r8,r0,0xf00		; opus vendor id
	or	r9,r0,9		       	; opus function id
	tb0	0,r0,449		; trap to Unix
	br	cerror			; return error to caller
	jmp	r1			; return to caller

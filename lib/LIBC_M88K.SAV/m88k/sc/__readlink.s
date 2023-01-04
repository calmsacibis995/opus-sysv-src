;	M88000 __readlink(2) Routine
;
	file	"__readlink.s"
	text
	global	__readlink
__readlink:
	MCOUNT
	or	r8,r0,2			; vendor ID
	or	r9,r0,3			; function number
	tb0	0,r0,449		; trap to Unix
	br	cerror			; return error to caller
	jmp	r1			; return to caller

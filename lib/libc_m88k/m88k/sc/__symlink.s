;	M88000 __symlink(2) Routine
;
	file	"__symlink.s"
	text
	global	__symlink
__symlink:
	MCOUNT
	or	r8,r0,2			; vendor ID
	or	r9,r0,4			; function number
	tb0	0,r0,449		; trap to Unix
	br	cerror			; return error to caller
	jmp	r1			; return to caller

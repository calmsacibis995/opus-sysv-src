;	M88000 __syscall(2) Routine
;
	file	"__syscall.s"
	text
	global	__syscall
__syscall:
	MCOUNT
	or	r10,r0,r9
	or	r9,r0,r2		; move syscall number
	or	r2,r0,r3
	or	r3,r0,r4
	or	r4,r0,r5
	or	r5,r0,r6
	or	r6,r0,r7
	or	r7,r0,r8
	or	r8,r0,r10
	tb0	0,r0,450		; trap to Unix
	br	cerror			; return error to caller
	jmp	r1			; return to caller

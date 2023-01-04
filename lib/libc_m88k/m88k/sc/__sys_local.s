;	M88000 __sys_local(2) Routine
;
	file	"__sys_local.s"
	text
	global	__sys_local
__sys_local:
	MCOUNT
	or	r10,r0,r8
	or	r11,r0,r9
	or	r8,r0,r2
	or	r9,r0,r3
	or	r2,r0,r4
	or	r3,r0,r5
	or	r4,r0,r6
	or	r5,r0,r7
	or	r6,r0,r10
	or	r7,r0,r11
	ld	r10,r31,32
	ld	r11,r31,36
	ld	r12,r31,40
	ld	r13,r31,44
	tb0	0,r0,449		; trap to Unix
	br	cerror			; return error to caller
	jmp	r1			; return to caller

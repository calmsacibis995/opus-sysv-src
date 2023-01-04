;	M88000 __chroot(2) Routine
;
	file	"__chroot.s"
	text
	global	__chroot
__chroot:
	MCOUNT
	;tb0	0,r0,256+61		; trap to Unix
	or	r9,r0,6
	tb0	0,r0,450		; trap to Unix
	br	cerror			; return error to caller
	jmp	r1			; return to caller

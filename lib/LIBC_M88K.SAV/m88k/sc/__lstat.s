;	M88000 __lstat(2) Routine
;
	file	"__lstat.s"
	text
	global	__lstat
__lstat:
	MCOUNT
	or	r4,r0,r2		; save path
	or	r8,r0,2
	or	r9,r0,2
	tb0	0,r0,449		; trap to Unix
	br	sterror			; return error to caller
	jmp.n	r1			; return to caller
	add	r2,r0,r0		; with 0.
sterror:
	subu	r6,r2,89		; ENOSYS
	bcnd	ne0,r6,chbad
	br.n	__stat
	or	r2,r0,r4		; restore path
chbad:
	br	cerror			; return error to caller

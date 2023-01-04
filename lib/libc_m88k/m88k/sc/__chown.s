;	M88000 __chown(2) Routine
;
	file	"__chown.s"
	text
	global	__chown
__chown:
	MCOUNT
	or	r5,r0,r2		; save path
	or	r8,r0,2			; vendor id 2
	or	r9,r0,1
	tb0	0,r0,449		; trap to Unix
	br	cherror			; return error to caller
	jmp.n	r1			; return to caller
	add	r2,r0,r0		; with 0.
cherror:
	subu	r6,r2,89		; ENOSYS
	bcnd	ne0,r6,chbad
	br.n	__lchown
	or	r2,r0,r5		; restore path
chbad:
	br	cerror			; return error to caller

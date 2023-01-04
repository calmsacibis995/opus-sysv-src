	file	"_openmax.s"
	text
	global	__openmax
__openmax:
	or.u	r3,r0,hi16(__nf)
	ld	r2,r3,lo16(__nf)
	bcnd	ne0,r2,out

	or	r2,r0,5	
	or	r9,r0,58		; __sysconf(SC_OPEN_MAX)
	tb0	0,r0,450		; trap to Unix
	br	cerror			; return error to caller

	or.u	r3,r0,hi16(__nf)
	st	r2,r3,lo16(__nf)

out:
	jmp	r1

	data
	align	4
__nf:
	word	0


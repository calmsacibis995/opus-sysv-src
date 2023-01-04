;	M88000 _syslocal(2) Routine
;
	file	"__syslocal.s"
	text
	global	__syslocal
__syslocal:
	MCOUNT
;	tb0	0,r0,256+50		; trap to Unix
	or	r8,r0,0xf00		; opus vendor id
	or	r9,r0,6		       	; opus function id
	tb0	0,r0,449		; trap to Unix
	br	cerror			; return error to caller
	jmp	r1			; return to caller

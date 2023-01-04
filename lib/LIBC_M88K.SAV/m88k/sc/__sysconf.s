;	M88000 __sysconf(2) Routine
;
	file	"__sysconf.s"
	text
	global	__sysconf
__sysconf:
	MCOUNT
	or	r9,r0,58
	tb0	0,r0,450		; trap to Unix
	br	cerror			; return error to caller
	jmp	r1			; return to caller

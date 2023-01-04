;	M88000 __ustat(2) Routine
;
	file	"__ustat.s"
	text
	def	_ustat%,2
	global	__ustat
__ustat:
	MCOUNT
 	add	r4,r0,_ustat%
	;tb0	0,r0,256+57		; trap to Unix
	or	r9,r0,65
	tb0	0,r0,450		; trap to Unix
	br	cerror			; return error to caller
	jmp.n	r1			; return to caller
	add	r2,r0,r0

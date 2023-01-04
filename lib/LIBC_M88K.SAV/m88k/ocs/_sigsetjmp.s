;	M88000 _sigsetjmp(2) Routine
;
	file	"_sigsetjmp.s"
	text
	global	_sigsetjmp
_sigsetjmp:
	br	__sigsetjmp

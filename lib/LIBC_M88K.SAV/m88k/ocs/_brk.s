;	M88000 _brk(2) Routine
;
	file	"_brk.s"
	text
	global	_brk
_brk:
	br	__brk

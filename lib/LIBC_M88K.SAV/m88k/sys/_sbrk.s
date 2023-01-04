;	M88000 _sbrk(2) Routine
;
	file	"_sbrk.s"
	text
	global	_sbrk
_sbrk:
	br	__sbrk

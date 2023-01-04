;	M88000 _sigpending(2) Routine
;
	file	"_sigpending.s"
	text
	global	_sigpending
_sigpending:
	br	__sigpending

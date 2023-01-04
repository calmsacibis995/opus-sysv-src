;	M88000 _waitpid(2) Routine
;
	file	"_waitpid.s"
	text
	global	_waitpid
_waitpid:
	br	__waitpid

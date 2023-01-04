;	M88000 _gethostname(2) Routine
;
	file	"_gethostname.s"
	text
	global	_gethostname
_gethostname:
	br	__gethostname

;	M88000 _gettimeofday(2) Routine
;
	file	"_gttimeofday.s"
	text
	global	_gettimeofday
_gettimeofday:
	br	__gettimeofday

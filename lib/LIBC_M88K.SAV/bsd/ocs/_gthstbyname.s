;	M88000 _gethostbyname(2) Routine
;
	file	"_gthstbyname.s"
	text
	global	_gethostbyname
_gethostbyname:
	br	__gethostbyname

;	M88000 _syslog(2) Routine
;
	file	"_syslog.s"
	text
	global	_syslog
_syslog:
	br	__syslog

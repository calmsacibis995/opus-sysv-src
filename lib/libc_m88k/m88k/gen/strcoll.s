;	strcoll.s
;
;	M88000 _strcoll()
;
	file	"strcoll.s"
	text
	global	__strcoll
	global	__strcmp
;
__strcoll:
	br	__strcmp

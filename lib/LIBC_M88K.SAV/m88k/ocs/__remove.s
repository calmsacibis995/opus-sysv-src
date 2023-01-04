;	M88000 __remove(3) Routine
;
	file	"_remove.s"
	text
	global	__remove
__remove:
	br	__unlink

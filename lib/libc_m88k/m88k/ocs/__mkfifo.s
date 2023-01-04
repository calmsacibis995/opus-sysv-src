;	M88000 __mkfifo(2) Routine
;
	file	"__mkfifo.s"
	text
	global	__mkfifo
	global	__mknod
	def	S__IFIFO,010000
__mkfifo:
	or	r4,r0,r0
	mask	r3,r3,0x1ff
	or	r3,r3,S__IFIFO
	br	__mknod

;	@(#)fpdata.s	6.1	

;	file	"fpdata.s"
	data
	align	8
	global	__MAXDOUBLE
	global	__huge_val
__huge_val:
__MAXDOUBLE:
	word 0x7ff00000
	word 0x00000000
	global	__MINDOUBLE
__MINDOUBLE:
	word 0x00100000
	word 0x0
	global	__MAXFLOAT
__MAXFLOAT:
	word 0x7f7fffff
	global	__MINFLOAT
__MINFLOAT:
	word 0x00800000

	ident	"@(#)libc-port:gen/ctype.c	1.5"
	file		"ctype.c"
	text
	global		_setchrclass
_setchrclass:
;$$v0
	subu		r31,r31,744
	st		r1,r31,740
	st		r23,r31,680
	st		r24,r31,684
	st		r25,r31,688
	or		r25,r0,r2
	subu		r23,r0,1
	bcnd		ne0,r25,@L6
	or.u		r2,r0,hi16(@L3)
	or		r2,r2,lo16(@L3)
	bsr		_getenv
	or		r25,r0,r2
	bcnd		eq0,r25,@L7
	ld.b		r13,r25,0
	bcnd		ne0,r13,@L6
@L7:
	or.u		r12,r0,hi16(_first_call)
	ld.b		r12,r12,lo16(_first_call)
	bcnd		eq0,r12,@L4
	or		r2,r0,r0
	br		@L2
@L4:
	or.u		r25,r0,hi16(@L5)
	or		r25,r25,lo16(@L5)
@L6:
	or.u		r12,r0,hi16(_first_call)
	st.b		r0,r12,lo16(_first_call)
	addu		r2,r31,32
	or.u		r3,r0,hi16(@L8)
	or		r3,r3,lo16(@L8)
	or		r4,r0,15
	bsr		_memcpy
	addu		r2,r31,46
	or		r3,r0,r25
	bsr		_strcpy
	addu		r2,r31,32
	or		r3,r0,r0
	bsr		_open
	or		r24,r0,r2
	bcnd		lt0,r24,@L10
	addu		r3,r31,160
	or		r2,r0,r24
	or		r4,r0,514
	bsr		_read
	cmp		r13,r2,514
	bb1		ne,r13,@L9
	or.u		r2,r0,hi16(___ctype)
	or		r2,r2,lo16(___ctype)
	addu		r3,r31,160
	or		r4,r0,514
	bsr		_memcpy
	or		r23,r0,r0
@L9:
	or		r2,r0,r24
	bsr		_close
@L10:
	or		r2,r0,r23
@L2:
	ld		r23,r31,680
	ld		r24,r31,684
	ld		r25,r31,688
	ld		r1,r31,740
	addu		r31,r31,744
	jmp		r1

; Allocations for _setchrclass
	data
;	r25		ccname
;	r31,32		pathname
;	r31,160		my_ctype
;	r24		fd
;	r23		ret

; Allocations for module
	data
	align		8
	global		___ctype
___ctype:
	global		__ctype
__ctype:
	byte		0
	byte		32
	byte		32
	byte		32
	byte		32
	byte		32
	byte		32
	byte		32
	byte		32
	byte		32
	byte		40
	byte		40
	byte		40
	byte		40
	byte		40
	byte		32
	byte		32
	byte		32
	byte		32
	byte		32
	byte		32
	byte		32
	byte		32
	byte		32
	byte		32
	byte		32
	byte		32
	byte		32
	byte		32
	byte		32
	byte		32
	byte		32
	byte		32
	byte		72
	byte		16
	byte		16
	byte		16
	byte		16
	byte		16
	byte		16
	byte		16
	byte		16
	byte		16
	byte		16
	byte		16
	byte		16
	byte		16
	byte		16
	byte		16
	byte		132
	byte		132
	byte		132
	byte		132
	byte		132
	byte		132
	byte		132
	byte		132
	byte		132
	byte		132
	byte		16
	byte		16
	byte		16
	byte		16
	byte		16
	byte		16
	byte		16
	byte		129
	byte		129
	byte		129
	byte		129
	byte		129
	byte		129
	byte		1
	byte		1
	byte		1
	byte		1
	byte		1
	byte		1
	byte		1
	byte		1
	byte		1
	byte		1
	byte		1
	byte		1
	byte		1
	byte		1
	byte		1
	byte		1
	byte		1
	byte		1
	byte		1
	byte		1
	byte		16
	byte		16
	byte		16
	byte		16
	byte		16
	byte		16
	byte		130
	byte		130
	byte		130
	byte		130
	byte		130
	byte		130
	byte		2
	byte		2
	byte		2
	byte		2
	byte		2
	byte		2
	byte		2
	byte		2
	byte		2
	byte		2
	byte		2
	byte		2
	byte		2
	byte		2
	byte		2
	byte		2
	byte		2
	byte		2
	byte		2
	byte		2
	byte		16
	byte		16
	byte		16
	byte		16
	byte		32
	byte		0
	byte		0
	byte		0
	byte		0
	byte		0
	byte		0
	byte		0
	byte		0
	byte		0
	byte		0
	byte		0
	byte		0
	byte		0
	byte		0
	byte		0
	byte		0
	byte		0
	byte		0
	byte		0
	byte		0
	byte		0
	byte		0
	byte		0
	byte		0
	byte		0
	byte		0
	byte		0
	byte		0
	byte		0
	byte		0
	byte		0
	byte		0
	byte		0
	byte		0
	byte		0
	byte		0
	byte		0
	byte		0
	byte		0
	byte		0
	byte		0
	byte		0
	byte		0
	byte		0
	byte		0
	byte		0
	byte		0
	byte		0
	byte		0
	byte		0
	byte		0
	byte		0
	byte		0
	byte		0
	byte		0
	byte		0
	byte		0
	byte		0
	byte		0
	byte		0
	byte		0
	byte		0
	byte		0
	byte		0
	byte		0
	byte		0
	byte		0
	byte		0
	byte		0
	byte		0
	byte		0
	byte		0
	byte		0
	byte		0
	byte		0
	byte		0
	byte		0
	byte		0
	byte		0
	byte		0
	byte		0
	byte		0
	byte		0
	byte		0
	byte		0
	byte		0
	byte		0
	byte		0
	byte		0
	byte		0
	byte		0
	byte		0
	byte		0
	byte		0
	byte		0
	byte		0
	byte		0
	byte		0
	byte		0
	byte		0
	byte		0
	byte		0
	byte		0
	byte		0
	byte		0
	byte		0
	byte		0
	byte		0
	byte		0
	byte		0
	byte		0
	byte		0
	byte		0
	byte		0
	byte		0
	byte		0
	byte		0
	byte		0
	byte		0
	byte		0
	byte		0
	byte		0
	byte		0
	byte		0
	byte		0
	byte		0
	byte		0
	byte		0
	byte		0
	byte		0
	byte		1
	byte		2
	byte		3
	byte		4
	byte		5
	byte		6
	byte		7
	byte		8
	byte		9
	byte		10
	byte		11
	byte		12
	byte		13
	byte		14
	byte		15
	byte		16
	byte		17
	byte		18
	byte		19
	byte		20
	byte		21
	byte		22
	byte		23
	byte		24
	byte		25
	byte		26
	byte		27
	byte		28
	byte		29
	byte		30
	byte		31
	byte		32
	byte		33
	byte		34
	byte		35
	byte		36
	byte		37
	byte		38
	byte		39
	byte		40
	byte		41
	byte		42
	byte		43
	byte		44
	byte		45
	byte		46
	byte		47
	byte		48
	byte		49
	byte		50
	byte		51
	byte		52
	byte		53
	byte		54
	byte		55
	byte		56
	byte		57
	byte		58
	byte		59
	byte		60
	byte		61
	byte		62
	byte		63
	byte		64
	byte		97
	byte		98
	byte		99
	byte		100
	byte		101
	byte		102
	byte		103
	byte		104
	byte		105
	byte		106
	byte		107
	byte		108
	byte		109
	byte		110
	byte		111
	byte		112
	byte		113
	byte		114
	byte		115
	byte		116
	byte		117
	byte		118
	byte		119
	byte		120
	byte		121
	byte		122
	byte		91
	byte		92
	byte		93
	byte		94
	byte		95
	byte		96
	byte		65
	byte		66
	byte		67
	byte		68
	byte		69
	byte		70
	byte		71
	byte		72
	byte		73
	byte		74
	byte		75
	byte		76
	byte		77
	byte		78
	byte		79
	byte		80
	byte		81
	byte		82
	byte		83
	byte		84
	byte		85
	byte		86
	byte		87
	byte		88
	byte		89
	byte		90
	byte		123
	byte		124
	byte		125
	byte		126
	byte		127
	byte		0
	byte		0
	byte		0
	byte		0
	byte		0
	byte		0
	byte		0
	byte		0
	byte		0
	byte		0
	byte		0
	byte		0
	byte		0
	byte		0
	byte		0
	byte		0
	byte		0
	byte		0
	byte		0
	byte		0
	byte		0
	byte		0
	byte		0
	byte		0
	byte		0
	byte		0
	byte		0
	byte		0
	byte		0
	byte		0
	byte		0
	byte		0
	byte		0
	byte		0
	byte		0
	byte		0
	byte		0
	byte		0
	byte		0
	byte		0
	byte		0
	byte		0
	byte		0
	byte		0
	byte		0
	byte		0
	byte		0
	byte		0
	byte		0
	byte		0
	byte		0
	byte		0
	byte		0
	byte		0
	byte		0
	byte		0
	byte		0
	byte		0
	byte		0
	byte		0
	byte		0
	byte		0
	byte		0
	byte		0
	byte		0
	byte		0
	byte		0
	byte		0
	byte		0
	byte		0
	byte		0
	byte		0
	byte		0
	byte		0
	byte		0
	byte		0
	byte		0
	byte		0
	byte		0
	byte		0
	byte		0
	byte		0
	byte		0
	byte		0
	byte		0
	byte		0
	byte		0
	byte		0
	byte		0
	byte		0
	byte		0
	byte		0
	byte		0
	byte		0
	byte		0
	byte		0
	byte		0
	byte		0
	byte		0
	byte		0
	byte		0
	byte		0
	byte		0
	byte		0
	byte		0
	byte		0
	byte		0
	byte		0
	byte		0
	byte		0
	byte		0
	byte		0
	byte		0
	byte		0
	byte		0
	byte		0
	byte		0
	byte		0
	byte		0
	byte		0
	byte		0
	byte		0
	byte		0
	byte		0
	byte		0
	byte		0
	byte		0
	byte		0
_first_call:
	byte		1
	align		4
@L8:
	string		"/lib/chrclass/"
	byte		0
	align		4
@L5:
	string		"ascii"
	byte		0
	align		4
@L3:
	string		"CHRCLASS"
	byte		0

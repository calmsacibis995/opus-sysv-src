;	M88000 __uadmin(2) Routine
;
	file	"__uadmin.s"
	text
	global	__uadmin
__uadmin:
	MCOUNT
;	tb0	0,r0,256+55		; trap to Unix
	or	r8,r0,0xf00		; opus vendor id
	or	r9,r0,8		       	; opus function id
	tb0	0,r0,449		; trap to Unix
	br	cerror			; return error to caller
	jmp	r1			; return to caller

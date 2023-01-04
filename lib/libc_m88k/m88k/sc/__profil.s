;	M88000 __profil(2) Routine
;
	file	"__profil.s"
	text
	global	__profil
__profil:
	MCOUNT
	;tb0	0,r0,256+44		; trap to Unix
	or	r9,r0,37
	tb0	0,r0,450		; trap to Unix
	br	cerror			; return error to caller
	jmp	r1			; return to caller

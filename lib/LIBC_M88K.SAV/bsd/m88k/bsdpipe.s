;	bsdpipe(f)
;	int f[2];

	global	__bsdpipe
	global  cerror

__bsdpipe:
	or	r4,r0,r2		; save arg
	or	r8,r0,0xf00		; opus vendor id
	or	r9,r0,11	       	; opus function id
	tb0	0,r0,449		; trap to Unix
	br	cerror
	st	r2,r4,0
	st	r3,r4,4
	jmp.n	r1
	add	r2,r0,r0

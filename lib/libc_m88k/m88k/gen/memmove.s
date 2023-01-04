;	@(#)__memmove.s	6.1	
;
;	M78000 __memmove(3C) Routine
;
;	(C) Copyright 1987 by Motorola Inc.
;
;	Written by: Bob Greiner
;
; __memmove - copies ln characters from sorc to dsave, and returns dsave
;	If copying takes place between objects that overlap, the
;	string is copied correctly, although possibly slowly
;
;	This routine works in Big Endian mode only.
;
; Input:	dsave - destination
;		sorc  - source
;		ln    - number of bytes to copy
;
; Output:	dsave
;
; Algorithm:
;
; __memmove (dsave, sorc, ln)
;
;	if dsave <= sorc or sorc+ln <= dsave then
;		return memcpy (dsave, sorc, ln)
;	else
;		dest = dsave
;		while ln-- > 0 loop
;			(dest+ln)->byte = (sorc+ln)->byte
;		loop
;		return dsave
;	end if
;
; Register usage:
;
_m4_define_(`dsave', r2)_m4_dnl_	;	destination pointer; in and function value
_m4_define_(`sorc', r3)_m4_dnl_	;	source pointer
_m4_define_(`ln', r4)_m4_dnl_	;	length to copy in bytes
_m4_define_(`dest', r5)_m4_dnl_	;	working destination pointer
_m4_define_(`temp', r6)_m4_dnl_	;	utility register
;
	file	"__memmove.s"
	text
	global	__memmove
	global	_memcpy
__memmove:
	MCOUNT
	cmp	temp,dsave,sorc		; if dsave <= sorc
	bb1	le,temp,fast
	addu	temp,sorc,ln		; or sorc+ln <= dsave then
	cmp	temp,temp,dsave
	bb0	le,temp,slow
fast:
	br	_memcpy			; return memcpy (dsave, sorc, ln)
slow:					; else
	or	dest,dsave,zero		; dest = dsave
	bcnd.n	le0,ln,done		; unroll loop
	subu	ln,ln,1			; unroll loop
loop:					; while (ln-- > 0) loop
	ld.bu	temp,sorc,ln		; (dest+ln)->byte = (sorc+ln)->byte
	st.b	temp,dest,ln
	bcnd.n	gt0,ln,loop		; loop
	subu	ln,ln,1			; unroll loop
done:					; end if
	jmp	return			; return dsave

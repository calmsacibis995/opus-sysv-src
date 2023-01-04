;	@(#)fakcu.s	6.1 
;
;	M78000 _cleanup(3) Routine
;
;	(C) Copyright 1987 by Motorola Inc.
;
;	Written by: Bob Greiner
;
; __cleanup - fake cleanup routine if user doesn't supply one
;
	file	"fakcu.s"
	text
	global	__cleanup

__cleanup:
	jmp	return			; simply return

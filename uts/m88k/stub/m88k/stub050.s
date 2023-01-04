
;	Stub routine for loading prfclk or rtc drvr w/o VME050 drvr
;	@(#)stub050.s	

	text
;m050prime:	global	_m050prime	#
	add	r2,r0,r0
	jmp	r1

;
;	Data definitions
;
	global	_m050_0
	global  m050_addr
	global	_m050_ivec

;	No one had better use addr or ivec if m050_0 tests as 0....

_m050_addr:
_m050_ivec:
_m050_0:
	word	0			; MUST be zero


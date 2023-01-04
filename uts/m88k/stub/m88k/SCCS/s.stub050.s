h33386
s 00023/00000/00000
d D 1.1 90/03/06 12:45:37 root 1 0
c date and time created 90/03/06 12:45:37 by root
e
u
U
t
T
I 1

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

E 1

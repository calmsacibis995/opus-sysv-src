
		include(sub.m4)
;==========================


	global	_fpehndlr
_fpehndlr:
;
;    This routine is called from getmsps when there are fp exceptions
;  to be handled.
	or.u	r3,r0,hi16(__U+u_sfu1flag) ;get upper half-word of 
						 ;first frame pointer
	st	r0,r3,lo16(__U+u_sfu1flag) ;get lower half-word of 
	;br	_fpertn

	st	r1,r31,0
	
	or.u	r2,r0,hi16(__U + u_sfu1data) ;get upper half-word of 
						 ;first frame pointer
	or	r2,r2,lo16(__U + u_sfu1data) ;get lower half-word of 
	add	r2,r2,NUMFRAME*FRAMESIZE   ; Work backwards
findfull:
	subu	r2,r2,FRAMESIZE
	ld	r3,r2,FPTYPE
	bcnd	eq0,r3,findfull

	sub	r3,r3,FPIMP
	bcnd	eq0,r3,fpimp

	or.u	r1,r0,hi16(_fpupcnt)	; increment count of precise 
	ld	r3,r1,lo16(_fpupcnt)	; exceptions that have occured
	addu	r3,r3,1
	st	r3,r1,lo16(_fpupcnt)

	bsr	fpup
	br	nxtfpe
fpimp:

	or.u	r1,r0,hi16(_fpuicnt)	; increment count of imprecise 
	ld	r3,r1,lo16(_fpuicnt)	; exceptions that have occured
	addu	r3,r3,1
	st	r3,r1,lo16(_fpuicnt)

	bsr	fpui
	
nxtfpe:

	or.u	r3,r0,hi16(__U + u_sfu1data) ;get upper half-word of 
						 ;first frame pointer
	or	r3,r3,lo16(__U + u_sfu1data) ;get lower half-word of 
	subu	r3,r2,r3
	bcnd	ne0,r3,findfull
;
;   Have all fp exceptions been handled via default processing?
;  If so, clear sfu1flag, else leave it set to indicate user 
;  needs signal.
	
	or.u	r3,r0,hi16(__U + u_sfu1flag) ;get upper half-word of 
						 ;first frame pointer
	st	r0,r3,lo16(__U + u_sfu1flag) ;get lower half-word of 
	
	ld	r1,r31,0
	jmp	r1

	global	_sigkill
_sigkill:
	jmp	r1

	data
	global	_fpupcnt
	global	_fpuicnt
_fpupcnt:
	word	0
_fpuicnt:
	word	0

	text

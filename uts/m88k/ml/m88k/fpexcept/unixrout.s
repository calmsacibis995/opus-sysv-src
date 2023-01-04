;File unixrout
;See the documentation for this release for an overall description of this code.
		





	























































































































































		global	_handret
; MV this symbol conflicts with os/sig.c signal()
;		global	_signal
		global	_Xsfu1sigaddr
		global	_getmsps
		global	_getrs
		global	_ftrap
		global	_fpsignal
		global	_runrun
		text

_handret:	tb0	0,r0,134	;allow user to return from user handler
		jmp	r1

_signal:	tb0	0,r0,135	;called by users wishing to give the address
		jmp	r1		;of their floating point signal handler
					;304 will branch to _Xsfu1sigaddr

_Xsfu1sigaddr:	
;		lda	r1,r0,userlocation	;get address of userlocation
		or.u	r1,r0,hi16(userlocation)
		or	r1,r1,lo16(userlocation)
		st	r3,r1,r0	;store user handler address
		ldcr	r1,cr17	;get return address back
		or   r0,r0,r0
		rte			;return to user code

_getmsps:	jmp	r1	;called to handle DMU 0

_getrs:		jmp	r1	;called to handle DMU 0

_ftrap:		jmp	r1	;called if user time has run out

;Called to either kill user, or handler calls _fpsignal to get
;the user stack pointer and get set up to rte to the user handler

_fpsignal:	cmp	r4,r3,0	;Did signal come from handler?
		bb1	2,r4,defaulthand
		tb0	0,r0,0x1ff	;else stop simulation, kill user

defaulthand:	or.u	r31,r0,hi16(0);Place user stack pointer into r31
		or	r31,r31,lo16(0)	
		stcr	r0,cr5
;		lda	r4,r0,userlocation
		or.u	r4,r0,hi16(userlocation)
		or	r4,r4,lo16(userlocation)
		ld	r4,r4,0		;get address of user handler
		set	r4,r4,1<1>	;set 1 bit
		stcr	r4,cr6
		ldcr	r4,cr1
		clr	r4,r4,1<31>	;set to user 31
		jmp	r1

		data	

_runrun:	word	0x1	;have runrun set so user does not run out of
				;time
userlocation:	word	0x0	;will be written to by _Xsfu1sigaddr, read by
				;_fpsignal


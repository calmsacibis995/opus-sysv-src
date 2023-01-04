h21193
s 00063/00000/00000
d D 1.1 90/03/06 12:50:48 root 1 0
c date and time created 90/03/06 12:50:48 by root
e
u
U
t
T
I 1
;File unixrout
;See the documentation for this release for an overall description of this code.
		include(sub.m4)
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
		ldcr	r1,scratch1	;get return address back
		or   r0,r0,r0
		rte			;return to user code

_getmsps:	jmp	r1	;called to handle DMU exception

_getrs:		jmp	r1	;called to handle DMU exception

_ftrap:		jmp	r1	;called if user time has run out

;Called to either kill user, or handler calls _fpsignal to get
;the user stack pointer and get set up to rte to the user handler

_fpsignal:	cmp	r4,r3,SIGFPEPR	;Did signal come from handler?
		bb1	eq,r4,defaulthand
		tb0	0,r0,0x1ff	;else stop simulation, kill user

defaulthand:	or.u	r31,r0,hi16(__U0);Place user stack pointer into r31
		or	r31,r31,lo16(__U0)	
		stcr	r0,snip
;		lda	r4,r0,userlocation
		or.u	r4,r0,hi16(userlocation)
		or	r4,r4,lo16(userlocation)
		ld	r4,r4,0		;get address of user handler
		set	r4,r4,1<valid>	;set valid bit
		stcr	r4,sfip
		ldcr	r4,psr
		clr	r4,r4,1<mode>	;set to user mode
		jmp	r1

		data	

_runrun:	word	0x1	;have runrun set so user does not run out of
				;time
userlocation:	word	0x0	;will be written to by _Xsfu1sigaddr, read by
				;_fpsignal

E 1

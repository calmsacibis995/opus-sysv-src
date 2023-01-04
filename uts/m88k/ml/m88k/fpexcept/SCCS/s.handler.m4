h46210
s 00085/00000/00000
d D 1.1 90/03/06 12:50:46 root 1 0
c date and time created 90/03/06 12:50:46 by root
e
u
U
t
T
I 1
;function _handler --	  
;The documentation for this release gives an overall desciption of this code.


		include(sub.m4)	  
		global _handler
		global _userret
		text				
;Determine the cause of the exception and proceed according to whether the 
;exception is precise or imprecise.
;
;imprecise exceptions - store MANTHI, MANTLO, and IMPCR into memory. Store
;			   IMPCR
;precise exceptions - store S1HI, S1LO, S2HI, S2LO, and PCR into memory.  Store PCR.

_handler:	st	r1,r31,XHANDRETADDR	;save return address
		st	r2,r31,XFPSR	;let the user know any modifications
					;to FPSR
		st	r4,r31,XHANDFPECR	;save FPECR, need when 
						;returning from user
		st	r9,r31,XHANDPR	;store r9 which may hold PCR
		st	r12,r31,XHANDIMP ;store r12 which may hold IMPCR


;RTE to transfer to the user routine.
		or	r2,r31,XFPSR	;load pointer to exception data
		or	r3,r0,SIGFPEPR	;set signal number
		;bsr	_fpsignal	;signal user handler
		or   r0,r0,r0
		rte


;The user returns from his routine by doing a trap to a vector which branches
;to _userret.	
;The last frame, the frame that called the user handler, is found on the
;exception stack
;r10 and r11 are loaded
;with the high and low result if the exception was imprecise, and r5 and r6
;are loaded with the high and low result if the exception was precise.

_userret:	ldcr	r1,psr		 ;load PSR
		and	r1,r1,0xfff4	;enable FPU, interrupts, and exceptions
		stcr	r1,psr

;Find the last frame on the exception stack.
exceptframe:	or.u	r31,r0,hi16(__U + u_xcpt) ;get address of bottom of stack
		or	r31,r31,lo16(__U + u_xcpt) 
		addu	r1,r31,NUMFRAME*FRAMESIZE ;form address of top of stack
exceptincr:	ld	r2,r31,STKSTATE	;load in word with full bit
		bb1	full,r2,nextframe
		br.n	loadres		;full bit clear, last frame was full
					;branch to rest of exception processing
		subu	r31,r31,FRAMESIZE	;set pointer to last full frame
		
nextframe:	addu	r31,r31,FRAMESIZE	;increment point by a frame
		cmp	r2,r31,r1	;has the stack overflowed?
		bb1	lt,r2,exceptincr	;not last frame of stack
		subu	r31,r31,FRAMESIZE ;go to last frame of stack

loadres:	ld	r2,r31,XFPSR	;load FPSR from user
		ld	r4,r31,XHANDFPECR ;retrieve FPECR
		ff1	r1,r4	;see whether exception is precise (3 - 7)
					 ;or imprecise (bits 0 - 2)
		cmp	r1,r1,3		;see if exception is precise
		ld	r3,r31,XFPCR	;load FPCR from user
		bb1	ge,r1,precret ;exception is precise
imprecret:	ld	r10,r31,XFPECR	;load high word of result
		ld	r11,r31,XMANTHI	;load low word of result
		ld	r12,r31,XHANDIMP ;load r12 which holds IMPCR
		br	return		;return from subroutine

precret:	ld	r5,r31,XFPECR	;load high word of result from user
		ld	r6,r31,XS1HI	;load low word of result from user
		ld	r9,r31,XHANDPR ;load r9 which holds PCR


;Return to continue processing the exception

return:		ld	r1,r31,XHANDRETADDR	;load return address into r1
		jmp	r1		 ;return from subroutine

		data



E 1

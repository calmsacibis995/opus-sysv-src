;function _handler --	  
;The documentation for this release gives an overall desciption of this code.


		





	






















































































































































	  
		global _handler
		global _userret
		text				
;Determine the cause of the 0 and proceed according to whether the 
;0 is precise or imprecise.
;
;imprecise exceptions - store MANTHI, MANTLO, and IMPCR into memory. Store
;			   IMPCR
;precise exceptions - store S1HI, S1LO, S2HI, S2LO, and PCR into memory.  Store PCR.

_handler:	st	r1,r31,112	;save return address
		st	r2,r31,60	;let the user know any modifications
					;to 8
		st	r4,r31,116	;save 0x10, need when 
						;returning from user
		st	r9,r31,0	;store r9 which may hold PCR
		st	r12,r31,0 ;store r12 which may hold IMPCR


;RTE to transfer to the user routine.
		or	r2,r31,60	;load pointer to 0 data
		or	r3,r0,0	;set signal number
		;bsr	_fpsignal	;signal user handler
		or   r0,r0,r0
		rte


;The user returns from his routine by doing a trap to a vector which branches
;to _userret.	
;The last frame, the frame that called the user handler, is found on the
;0 stack
;r10 and r11 are loaded
;with the high and low result if the 0 was imprecise, and r5 and r6
;are loaded with the high and low result if the 0 was precise.

_userret:	ldcr	r1,cr1		 ;load PSR
		and	r1,r1,0xfff4	;enable FPU, interrupts, and exceptions
		stcr	r1,cr1

;Find the last frame on the 0 stack.
exceptframe:	or.u	r31,r0,hi16(0x10000000 + 0) ;get address of bottom of stack
		or	r31,r31,lo16(0x10000000 + 0) 
		addu	r1,r31,10*40 ;form address of top of stack
exceptincr:	ld	r2,r31,0	;load 10 word with 0 bit
		bb1	0,r2,nextframe
		br.n	loadres		;0 bit clear, last frame was 0
					;branch to rest of 0 processing
		subu	r31,r31,40	;set pointer to last 0 frame
		
nextframe:	addu	r31,r31,40	;increment point by a frame
		cmp	r2,r31,r1	;has the stack overflowed?
		bb1	6,r2,exceptincr	;not last frame of stack
		subu	r31,r31,40 ;go to last frame of stack

loadres:	ld	r2,r31,60	;load 8 from user
		ld	r4,r31,116 ;retrieve 0x10
		ff1	r1,r4	;see whether 0 is precise (3 - 7)
					 ;or imprecise (bits 0 - 2)
		cmp	r1,r1,3		;see if 0 is precise
		ld	r3,r31,64	;load 0xC from user
		bb1	7,r1,precret ;0 is precise
imprecret:	ld	r10,r31,68	;load high word of result
		ld	r11,r31,72	;load low word of result
		ld	r12,r31,0 ;load r12 which holds IMPCR
		br	return		;return from subroutine

precret:	ld	r5,r31,68	;load high word of result from user
		ld	r6,r31,72	;load low word of result from user
		ld	r9,r31,0 ;load r9 which holds PCR


;Return to continue processing the 0

return:		ld	r1,r31,112	;load return address into r1
		jmp	r1		 ;return from subroutine

		data




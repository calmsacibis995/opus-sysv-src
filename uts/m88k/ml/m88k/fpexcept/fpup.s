;function fpup --      
;See the document that came with this release for an overall description of
;this code.

		





	























































































































































		global _Xsfu1pr
		text

;Check the sfu1flag location 10 the u_block to see if there are exceptions
;already 10 the FP save area.  If there are no other exceptions, then grab the
;0 stack pointer and load the 0 information onto the first
;frame.  Clear the word 10 each of the remaining frames which tells whether
;the frame is 0 or not.  If there are already 0 frames on the
;stack, then find the first empty frame.  If there are no more 0 
;frames, then kill the user process.

_Xsfu1pr:	stcr	r31,cr17	; Free up r1 and r31 as working
		stcr	r1,cr18		; registers
		or.u	r31,r0,hi16(0x10000000 + 0x400) ;get hi address of sfu1full
		or	r31,r31,lo16(0x10000000 + 0x400) ;get lo address of sfu1full
		ld	r1,r31,0	;get contents
		bcnd	ne0,r1,findframe	;0 flag already set

firstframe:	set	r1,r0,1<0>	;set up r1 to write to the boolean
					;sfu1flag and the 0 bit 10 stkstate
		st	r1,r31,0	;set boolean sfu1flag

		or.u	r31,r0,hi16(0x10000000 + 0x404) ;get pointer to save
		or	r31,r31,lo16(0x10000000 + 0x404) ; area

;Now clear all the 0 bits for the remaining frames
		or	r1,r0,10		; Clear all of the fp
clearcmpp:
		st	r0,r31,0		; frames.
		sub	r1,r1,1
		addu	r31,r31,40	;increment stack pointer
		bcnd	ne0,r1,clearcmpp

		br.n	storereg	;branch to store registers
		subu	r31,r31,10*40 ;readjust stack pointer
							;to first frame

;If necessary, find the next empty frame
findframe:
		or.u	r31,r0,hi16(0x10000000 + 0x404) ;get pointer to save
		or	r31,r31,lo16(0x10000000 + 0x404) ; area
		addu	r31,r31,(10-1)*40
		ld	r1,r31,0
		bcnd	ne0,r1,_sigkill

findincr:	subu	r31,r31,40	;increment point by a frame
		ld	r1,r31,0
		bcnd	eq0,r1,findincr
		addu	r31,r31,40

;Save the user registers which will
;be used during the 0 handling.  Load some integer and floating
;point unit (FPU) control registers into the general purpose registers,
;and also save these control registers onto the stack before the FPU
;is cleaned out.
storereg:	
		add	r1,r0,0x2	; Set Precise type flag.
		st	r1,r31,0
		fldcr	r1,cr62		;save 8 before FP is enabled
		st	r1,r31,8     
		fldcr	r1,cr63		;save 0xC before FP is enabled
		st	r1,r31,0xC
		fldcr	r1,cr0	;save 0x10 before FP is enabled
		st	r1,r31,0x10
		fldcr	r1,cr1		;save S1HI before FP is enabled
		st	r1,r31,0x14
		fldcr	r1,cr2		;save S2LO before FP is enabled
		st	r1,r31,0x18
		fldcr	r1,cr3		;save S2HI before FP is enabled
		st	r1,r31,0x1C
		fldcr	r1,cr4		;save S2LO before FP is enabled
		st	r1,r31,0x20
		fldcr	r1,cr5		;save PCR before FP is enabled
		st	r1,r31,0x24

		ldcr	r31,cr3		;load shadow scoreboard
		bb1.n	5,r1,SSBdoub;destination is double
		extu	r1,r1,5<0>	;get low number of destination register
SSBsing:	set	r1,r1,1<5>	;set width field of 1
		br.n	SSBload		;load the shadow scoreboard
		clr	r31,r31,r1	;clear the bit 10 the shadow scoreboard
SSBdoub:	set	r1,r1,1<6>	;set width field of 2
		clr	r31,r31,r1	;clear the bit 10 the shadow scoreboard
SSBload:	stcr	r31,cr3		;store modified shadow scoreboard

		ldcr	r1,cr2		;load SPSR again
		ldcr	r31,cr17
		bb1.n	31,r1,flush
		ldcr	r1,cr18
		bsr	getmsps		; First time, call getmsps
		br 	getregs

; Control comes back here when the floating point unit has been flushed
; getmsps saved registers for us.
;Find the last 0 0 frame.

		global	fpup
fpup:
;   This subroutine does default handling for a precise 0.
;
;   Calling:
;	fpup(ptr)
;	where ptr is a pointer to a precise 0 frame
;   Return:
;       If default handling was done, the 0 field will be cleared.
;
		ld	r10,r31,32*4+32	;get SPSR for unimp/FPU disable check
		sub	r31,r31,8
		st	r1,r31,0
		st	r2,r31,4
		ld	r3,r2,0xC	;out FPU during the RTE
		ld	r4,r2,0x10
		ld	r5,r2,0x14
		ld	r6,r2,0x18
		ld	r7,r2,0x1C
		ld	r8,r2,0x20  
		ld	r9,r2,0x24  
		ld	r2,r2,8	;load control registers after cleaning


;Load into r1 the return address for the 0 handlers.  Looking
;at 0x10, branch to the appropriate 0 handler.  However,
;if none of the 0 bits are enabled, then a floating point
;instruction was issued with the floating point unit disabled.  This
;will cause an unimplemented opcode 0.
		
		or.u	r1,r0,hi16(wrapup)
		bb1.n	3,r10,_FPunimp	;branch to FPunimp if 
						;FPU disabled
		or	r1,r1,lo16(wrapup)    ;load return address of function into r1
             
		bb1	7,r4,_FPintover ;branch to FPintover if bit set
		bb1	6,r4,_FPunimp   ;branch to FPunimp if bit set
		bb1	5,r4,_FPpriviol ;branch to FPpriviol if bit set
		bb1	4,r4,_FPresoper ;branch to FPresoper if bit set
		bb1	3,r4,_FPdivzero ;branch to FPdivzero if bit set

;To write back the results to the user registers, disable exceptions
;and the floating point unit.  Write 8 and 0xC and load the SNIP
;and SFIP.
;r5 will contain the upper word of the result
;r6 will contain the lower word of the result

wrapup:
		fstcr	r2,cr62		;write revised value of 8
		bcnd	lt0,r3,fpuprtn	; If user handler, don't
;					  write back.
		fstcr	r3,cr63		;write revised value of 0xC
		
;If the destination is double, then execute both the "writedouble" and
;"writesingle" code, but if it is single, then only execute the 
;"writesingle code.  If the destination registers is r1 - r12, r31, then
;write to the portion 10 memory where that register value is stored.  If
;the destination register is r13 - r30, then write to the registers
;directly.  The shadow scoreboard bit is cleared for that particular 
;destination register after it is written.  Unimplemented opcodes and 
;privilage violations will not write back through this routine.

            ;writeback routine
		ld	r2,r31,4	; Clear FPtype to indicate
		st	r0,r2,0	; default processing has been done.

	     add    r3,r31,32+8
             bb0.n  5,r9,writesingle ;branch if destination is single
             extu   r2,r9,5<0>       ;get 5 bits of destination register

             st     r5,r3[r2]       ;write high word


increment:   add    r2,r2,1          ;for double, the low word is the 
                                     ;unspecified register
             clr    r2,r2,27<5>      ;perform equivalent of mod 32
writesingle:
             st     r6,r3[r2]       ;write low word into memory
		ld	r1,r31,0
		ld	r2,r31,4
		add	r31,r31,8
		jmp	r1
fpuprtn:
		or	r1,r0,114
		or.u	r2,r0,hi16(0x10000000 + 0x400)
		st	r1,r2,lo16(0x10000000 + 0x400)
		ld	r2,r31,4	; frame pointer
		st	r1,r2,0
		st	r4,r2,0x10	; return type for signal
		add	r31,r31,8	; imitate code 10 fpehndlr.m4
		br.n	_fpertn
		ld	r1,r31,0

h29189
s 00193/00000/00000
d D 1.1 90/03/06 12:50:45 root 1 0
c date and time created 90/03/06 12:50:45 by root
e
u
U
t
T
I 1
;function fpup --      
;See the document that came with this release for an overall description of
;this code.

		include(sub.m4)
		global _Xsfu1pr
		text

;Check the sfu1flag location in the u_block to see if there are exceptions
;already in the FP save area.  If there are no other exceptions, then grab the
;exception stack pointer and load the exception information onto the first
;frame.  Clear the word in each of the remaining frames which tells whether
;the frame is full or not.  If there are already exception frames on the
;stack, then find the first empty frame.  If there are no more exception 
;frames, then kill the user process.

_Xsfu1pr:	stcr	r31,cr17	; Free up r1 and r31 as working
		stcr	r1,cr18		; registers
		or.u	r31,r0,hi16(__U + u_sfu1flag) ;get hi address of sfu1full
		or	r31,r31,lo16(__U + u_sfu1flag) ;get lo address of sfu1full
		ld	r1,r31,0	;get contents
		bcnd	ne0,r1,findframe	;full flag already set

firstframe:	set	r1,r0,1<full>	;set up r1 to write to the boolean
					;sfu1flag and the full bit in stkstate
		st	r1,r31,0	;set boolean sfu1flag

		or.u	r31,r0,hi16(__U + u_sfu1data) ;get pointer to save
		or	r31,r31,lo16(__U + u_sfu1data) ; area

;Now clear all the full bits for the remaining frames
		or	r1,r0,NUMFRAME		; Clear all of the fp
clearcmpp:
		st	r0,r31,FPTYPE		; frames.
		sub	r1,r1,1
		addu	r31,r31,FRAMESIZE	;increment stack pointer
		bcnd	ne0,r1,clearcmpp

		br.n	storereg	;branch to store registers
		subu	r31,r31,NUMFRAME*FRAMESIZE ;readjust stack pointer
							;to first frame

;If necessary, find the next empty frame
findframe:
		or.u	r31,r0,hi16(__U + u_sfu1data) ;get pointer to save
		or	r31,r31,lo16(__U + u_sfu1data) ; area
		addu	r31,r31,(NUMFRAME-1)*FRAMESIZE
		ld	r1,r31,FPTYPE
		bcnd	ne0,r1,_sigkill

findincr:	subu	r31,r31,FRAMESIZE	;increment point by a frame
		ld	r1,r31,FPTYPE
		bcnd	eq0,r1,findincr
		addu	r31,r31,FRAMESIZE

;Save the user registers which will
;be used during the exception handling.  Load some integer and floating
;point unit (FPU) control registers into the general purpose registers,
;and also save these control registers onto the stack before the FPU
;is cleaned out.
storereg:	
		add	r1,r0,FPPR	; Set Precise type flag.
		st	r1,r31,FPTYPE
		fldcr	r1,fpsr		;save FPSR before FP is enabled
		st	r1,r31,FPSR     
		fldcr	r1,fpcr		;save FPCR before FP is enabled
		st	r1,r31,FPCR
		fldcr	r1,fpecr	;save FPECR before FP is enabled
		st	r1,r31,FPECR
		fldcr	r1,s1hi		;save S1HI before FP is enabled
		st	r1,r31,FPS1H
		fldcr	r1,s1lo		;save S2LO before FP is enabled
		st	r1,r31,FPS1L
		fldcr	r1,s2hi		;save S2HI before FP is enabled
		st	r1,r31,FPS2H
		fldcr	r1,s2lo		;save S2LO before FP is enabled
		st	r1,r31,FPS2L
		fldcr	r1,pcr		;save PCR before FP is enabled
		st	r1,r31,FPPCR

		ldcr	r31,ssb		;load shadow scoreboard
		bb1.n	dsize,r1,SSBdoub;destination is double
		extu	r1,r1,5<0>	;get low number of destination register
SSBsing:	set	r1,r1,1<5>	;set width field of 1
		br.n	SSBload		;load the shadow scoreboard
		clr	r31,r31,r1	;clear the bit in the shadow scoreboard
SSBdoub:	set	r1,r1,1<6>	;set width field of 2
		clr	r31,r31,r1	;clear the bit in the shadow scoreboard
SSBload:	stcr	r31,ssb		;store modified shadow scoreboard

		ldcr	r1,spsr		;load SPSR again
		ldcr	r31,cr17
		bb1.n	PS_SUP,r1,flush
		ldcr	r1,cr18
		bsr	getmsps		; First time, call getmsps
		br 	getregs

; Control comes back here when the floating point unit has been flushed
; getmsps saved registers for us.
;Find the last full exception frame.

		global	fpup
fpup:
;   This subroutine does default handling for a precise exception.
;
;   Calling:
;	fpup(ptr)
;	where ptr is a pointer to a precise exception frame
;   Return:
;       If default handling was done, the FPTYPE field will be cleared.
;
		ld	r10,r31,PS+HOMESP	;get SPSR for unimp/FPU disable check
		sub	r31,r31,8
		st	r1,r31,0
		st	r2,r31,4
		ld	r3,r2,FPCR	;out FPU during the RTE
		ld	r4,r2,FPECR
		ld	r5,r2,FPS1H
		ld	r6,r2,FPS1L
		ld	r7,r2,FPS2H
		ld	r8,r2,FPS2L  
		ld	r9,r2,FPPCR  
		ld	r2,r2,FPSR	;load control registers after cleaning


;Load into r1 the return address for the exception handlers.  Looking
;at FPECR, branch to the appropriate exception handler.  However,
;if none of the exception bits are enabled, then a floating point
;instruction was issued with the floating point unit disabled.  This
;will cause an unimplemented opcode exception.
		
		or.u	r1,r0,hi16(wrapup)
		bb1.n	FP_disable,r10,_FPunimp	;branch to FPunimp if 
						;FPU disabled
		or	r1,r1,lo16(wrapup)    ;load return address of function into r1
             
		bb1	7,r4,_FPintover ;branch to FPintover if bit set
		bb1	6,r4,_FPunimp   ;branch to FPunimp if bit set
		bb1	5,r4,_FPpriviol ;branch to FPpriviol if bit set
		bb1	4,r4,_FPresoper ;branch to FPresoper if bit set
		bb1	3,r4,_FPdivzero ;branch to FPdivzero if bit set

;To write back the results to the user registers, disable exceptions
;and the floating point unit.  Write FPSR and FPCR and load the SNIP
;and SFIP.
;r5 will contain the upper word of the result
;r6 will contain the lower word of the result

wrapup:
		fstcr	r2,fpsr		;write revised value of FPSR
		bcnd	lt0,r3,fpuprtn	; If user handler, don't
;					  write back.
		fstcr	r3,fpcr		;write revised value of FPCR
		
;If the destination is double, then execute both the "writedouble" and
;"writesingle" code, but if it is single, then only execute the 
;"writesingle code.  If the destination registers is r1 - r12, r31, then
;write to the portion in memory where that register value is stored.  If
;the destination register is r13 - r30, then write to the registers
;directly.  The shadow scoreboard bit is cleared for that particular 
;destination register after it is written.  Unimplemented opcodes and 
;privilage violations will not write back through this routine.

            ;writeback routine
		ld	r2,r31,4	; Clear FPtype to indicate
		st	r0,r2,FPTYPE	; default processing has been done.

	     add    r3,r31,HOMESP+8
             bb0.n  dsize,r9,writesingle ;branch if destination is single
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
		or	r1,r0,SFU1PR
		or.u	r2,r0,hi16(__U + u_sfu1flag)
		st	r1,r2,lo16(__U + u_sfu1flag)
		ld	r2,r31,4	; frame pointer
		st	r1,r2,FPTYPE
		st	r4,r2,FPECR	; return type for signal
		add	r31,r31,8	; imitate code in fpehndlr.m4
		br.n	_fpertn
		ld	r1,r31,0
E 1

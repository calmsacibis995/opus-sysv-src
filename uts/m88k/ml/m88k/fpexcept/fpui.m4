;function Xsfu1imp --      
;See the documentation that came with this release for an overall description
;of this code.

		include(sub.m4)
		global _Xsfu1imp
		text					

;Check the sfu1full location in the u_block to see if there are exceptions
;already on the stack.  If there are no other exceptions, then grab the
;exception stack pointer and load the exception information onto the first
;frame.  Clear the word in each of the remaining frames which tells whether
;the frame is full or not.  If there are already exception frames on the
;stack, then find the first empty frame.  If there are no more exception 
;frames, then kill the user process.

_Xsfu1imp:	stcr	r31,cr17	;save orignal scratch pointer 
					;into cr17
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
clearcmpi:
		st	r0,r31,FPTYPE		; frames.
		sub	r1,r1,1
		addu	r31,r31,FRAMESIZE	;increment stack pointer
		bcnd	ne0,r1,clearcmpi

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
		add	r1,r0,FPIMP	; Mark as a imprecise frame.
		st	r1,r31,FPTYPE
		fldcr	r1,fpsr		;save FPSR before FP is enabled
		st	r1,r31,FPSR     
		fldcr	r1,fpcr		;save FPCR before FP is enabled
		st	r1,r31,FPCR
		fldcr	r1,fpecr	;save FPECR before FP is enabled
		st	r1,r31,FPECR
		fldcr	r1,manthi	;save MANTHI before FP is enabled
		st	r1,r31,FPMANTH
		fldcr	r1,mantlo	;save MANTLO before FP is enabled
		st	r1,r31,FPMANTL
		fldcr	r1,impcr	;save IMPCR before FP is enabled
		st	r1,r31,FPICR

		bb0	destsize,r1,dacc;single precision result does not
					;need to have its scoreboard bit clear
		ldcr	r31,ssb		;load the shadow scoreboard
SSBdoub:	extu	r1,r1,5<0>	;get low number of destination register
		add	r1,r1,1		;increment for the low word of result
		set	r1,r1,1<5>	;set width field to one
		clr	r31,r31,r1	;clear the scoreboard bit
SSBsing:	stcr	r31,ssb		;single precision result scoreboard bit
             
;check for data access fault
dacc:

		ldcr	r1,spsr		;load SPSR again
		ldcr	r31,cr17
		bb1.n	PS_SUP,r1,flush
		ldcr	r1,cr18
		bsr	getmsps		; First time, call getmsps
		br 	fgetregs


;=====================================================
	global	fpui
fpui:
		sub	r31,r31,8
		st	r1,r31,0
		st	r2,r31,4

		ld	r3,r2,FPCR	;out FPU during the RTE
		ld	r4,r2,FPECR
		ld	r10,r2,FPMANTH
		ld	r11,r2,FPMANTL
		ld	r12,r2,FPICR  
		ld	r2,r2,FPSR	;load control registers after cleaning

;Load into r1 the return address for the exception handlers.  Looking
;at FPECR, branch to the appropriate exception handler.

		or.u	r1,r0,hi16(wrapup)
		or	r1,r1,lo16(wrapup)
		bb1	2,r4,_FPunderflow ;branch to FPunderflow if bit set
		bb1	1,r4,_FPoverflow ;branch to FPoverflow if bit set

		bb0.n	0,r3,wrapup	; exception enabled ?
		or	r2,r2,1		; set fpsr EFINX bit
		sub	r3,r0,1

;To write back the results to the user registers, disable exceptions
;and the floating point unit.  Write FPSR and FPCR and load the SNIP
;and SFIP.
;r10 contains the upper word of the result
;r11 contains the lower word of the result

wrapup:
;  Write the results (r10,r11) back to the register set if default
; was excecuted.
             fstcr  r2,fpsr          ;write revised value of FPSR
	     bcnd   lt0,r3,fpuirtn
             fstcr  r3,fpcr          ;write revised value of FPCR
	ld	r2,r31,4	 ; Fp handled, clear flag.
	st	r0,r2,FPTYPE

;If the destination is double, then execute both the "writedouble" and
;"writesingle" code, but if it is single, then only execute the 
;"writesingle code.  If the destination registers is r1 - r12, r31, then
;write to the portion in memory where that register value is stored.  If
;the destination register is r13 - r30, then write to the registers
;directly.  The shadow scoreboard bit is cleared for that particular 
;destination register after it is written.

            ;writeback routine
	add	r3,r31,8+HOMESP	; Get pointer to Register set.

             bb0.n  destsize,r12,writesingle ;branch to write single 
                                             ;if single dest.
             extu   r2,r12,5<0>      ;get 5 bits for destination register 
writedouble:
             st     r10,r3[r2]      ;write high word

increment:   add    r2,r2,1          ;for double, the low word is the 
                                     ;unspecified register
             clr    r2,r2,27<5>      ;perform equivalent of mod 32
writesingle:
             st     r11,r3[r2]       ;write low word into memory
		or.u	r1,r0,hi16(__U + u_sfu1flag)
		st	r0,r1,lo16(__U + u_sfu1flag)
		ld	r1,r31,0
		ld	r2,r31,4
		add	r31,r31,8
		jmp	r1
fpuirtn:
		or	r1,r0,SFU1IMP
		or.u	r2,r0,hi16(__U + u_sfu1flag)
		st	r1,r2,lo16(__U + u_sfu1flag)
		ld	r2,r31,4	; frame pointer
		st	r1,r2,FPTYPE
		ld	r3,r2,FPCR
		and	r4,r3,r4	; FPCR & FPECR
		mask	r4,r4,7
		bcnd	ne0,r4,havebits
		or	r4,r0,1		; set EFINX
havebits:
		st	r4,r2,FPECR	; return type for signal
		add	r31,r31,8	; imitate code in fpehndlr.m4
		br.n	_fpertn
		ld	r1,r31,0

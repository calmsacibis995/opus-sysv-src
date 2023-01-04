;function Xsfu1imp --      
;See the documentation that came with this release for an overall description
;of this code.

		





	























































































































































		global _Xsfu1imp
		text					

;Check the sfu1full location 10 the u_block to see if there are exceptions
;already on the stack.  If there are no other exceptions, then grab the
;0 stack pointer and load the 0 information onto the first
;frame.  Clear the word 10 each of the remaining frames which tells whether
;the frame is 0 or not.  If there are already 0 frames on the
;stack, then find the first empty frame.  If there are no more 0 
;frames, then kill the user process.

_Xsfu1imp:	stcr	r31,cr17	;save orignal scratch pointer 
					;into cr17
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
clearcmpi:
		st	r0,r31,0		; frames.
		sub	r1,r1,1
		addu	r31,r31,40	;increment stack pointer
		bcnd	ne0,r1,clearcmpi

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
		add	r1,r0,01	; Mark as a imprecise frame.
		st	r1,r31,0
		fldcr	r1,cr62		;save 8 before FP is enabled
		st	r1,r31,8     
		fldcr	r1,cr63		;save 0xC before FP is enabled
		st	r1,r31,0xC
		fldcr	r1,cr0	;save 0x10 before FP is enabled
		st	r1,r31,0x10
		fldcr	r1,cr6	;save MANTHI before FP is enabled
		st	r1,r31,0x14
		fldcr	r1,cr7	;save MANTLO before FP is enabled
		st	r1,r31,0x18
		fldcr	r1,cr8	;save IMPCR before FP is enabled
		st	r1,r31,0x1C

		bb0	10,r1,dacc;single precision result does not
					;need to have its scoreboard bit clear
		ldcr	r31,cr3		;load the shadow scoreboard
SSBdoub:	extu	r1,r1,5<0>	;get low number of destination register
		add	r1,r1,1		;increment for the low word of result
		set	r1,r1,1<5>	;set width field to one
		clr	r31,r31,r1	;clear the scoreboard bit
SSBsing:	stcr	r31,cr3		;single precision result scoreboard bit
             
;check for data access fault
dacc:

		ldcr	r1,cr2		;load SPSR again
		ldcr	r31,cr17
		bb1.n	31,r1,flush
		ldcr	r1,cr18
		bsr	getmsps		; First time, call getmsps
		br 	fgetregs


;=====================================================
	global	fpui
fpui:
		sub	r31,r31,8
		st	r1,r31,0
		st	r2,r31,4

		ld	r3,r2,0xC	;out FPU during the RTE
		ld	r4,r2,0x10
		ld	r10,r2,0x14
		ld	r11,r2,0x18
		ld	r12,r2,0x1C  
		ld	r2,r2,8	;load control registers after cleaning

;Load into r1 the return address for the 0 handlers.  Looking
;at 0x10, branch to the appropriate 0 handler.

		or.u	r1,r0,hi16(wrapup)
		or	r1,r1,lo16(wrapup)
		bb1	2,r4,_FPunderflow ;branch to FPunderflow if bit set
		bb1	1,r4,_FPoverflow ;branch to FPoverflow if bit set

		bb0.n	0,r3,wrapup	; 0 enabled ?
		or	r2,r2,1		; set cr62 EFINX bit
		sub	r3,r0,1

;To write back the results to the user registers, disable exceptions
;and the floating point unit.  Write 8 and 0xC and load the SNIP
;and SFIP.
;r10 contains the upper word of the result
;r11 contains the lower word of the result

wrapup:
;  Write the results (r10,r11) back to the register set if default
; was excecuted.
             fstcr  r2,cr62          ;write revised value of 8
	     bcnd   lt0,r3,fpuirtn
             fstcr  r3,cr63          ;write revised value of 0xC
	ld	r2,r31,4	 ; Fp handled, clear flag.
	st	r0,r2,0

;If the destination is double, then execute both the "writedouble" and
;"writesingle" code, but if it is single, then only execute the 
;"writesingle code.  If the destination registers is r1 - r12, r31, then
;write to the portion 10 memory where that register value is stored.  If
;the destination register is r13 - r30, then write to the registers
;directly.  The shadow scoreboard bit is cleared for that particular 
;destination register after it is written.

            ;writeback routine
	add	r3,r31,8+32	; Get pointer to Register set.

             bb0.n  10,r12,writesingle ;branch to write single 
                                             ;if single dest.
             extu   r2,r12,5<0>      ;get 5 bits for destination register 
writedouble:
             st     r10,r3[r2]      ;write high word

increment:   add    r2,r2,1          ;for double, the low word is the 
                                     ;unspecified register
             clr    r2,r2,27<5>      ;perform equivalent of mod 32
writesingle:
             st     r11,r3[r2]       ;write low word into memory
		or.u	r1,r0,hi16(0x10000000 + 0x400)
		st	r0,r1,lo16(0x10000000 + 0x400)
		ld	r1,r31,0
		ld	r2,r31,4
		add	r31,r31,8
		jmp	r1
fpuirtn:
		or	r1,r0,115
		or.u	r2,r0,hi16(0x10000000 + 0x400)
		st	r1,r2,lo16(0x10000000 + 0x400)
		ld	r2,r31,4	; frame pointer
		st	r1,r2,0
		ld	r3,r2,0xC
		and	r4,r3,r4	; 0xC & 0x10
		mask	r4,r4,7
		bcnd	ne0,r4,havebits
		or	r4,r0,1		; set EFINX
havebits:
		st	r4,r2,0x10	; return type for signal
		add	r31,r31,8	; imitate code 10 fpehndlr.m4
		br.n	_fpertn
		ld	r1,r31,0

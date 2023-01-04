h35351
s 00981/00000/00000
d D 1.1 90/03/06 12:50:46 root 1 0
c date and time created 90/03/06 12:50:46 by root
e
u
U
t
T
I 1
>From oakhill!caine!jeffh  Wed Mar 15 15:44:39 1989 remote from sun
Received: from oakhill.UUCP by sun.Sun.COM (4.0/SMI-4.0)
	id AA01281; Wed, 15 Mar 89 15:44:39 PST
Received: from caine.oakhill.uucp by oakhill.uucp (3.2/SMI-3.0DEV3)
	id AA20535; Wed, 15 Mar 89 14:51:52 CST
Received: by caine.oakhill.uucp (3.2/SMI-3.2)
	id AA26581; Wed, 15 Mar 89 14:53:30 CST
Date: Wed, 15 Mar 89 14:53:30 CST
From: sun!oakhill!caine!jeffh (jeff hopkins)
Message-Id: <8903152053.AA26581@caine.oakhill.uucp>
To: oakhill!sun!opusys!madan

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

_Xsfu1imp:	stcr	r31,scratch2	;save orignal scratch pointer 
					;into scratch2
		or.u	r31,r0,hi16(__U + u_sfu1full) ;get hi address of sfu1full
		or	r31,r31,lo16(__U + u_sfu1full) ;get lo address of sfu1full
		stcr	r1,scratch1	;exchange r1 with scratch1
		ld	r1,r31,0	;get contents
		bcnd	ne0,r1,findframe	;full flag already set

firstframe:	set	r1,r0,1<full>	;set up r1 to write to the boolean
					;sfu1full and the full bit in stkstate
		st	r1,r31,0	;set boolean sfu1full
		or.u	r31,r0,hi16(__U + u_xcpt) ;get upper half-word of SSP
		or	r31,r31,lo16(__U + u_xcpt) ;get lower half-word of SSP


;|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
;fmul.dxx imprecise followed by addpipe instruction workaround

;The workaround below will use the exponent in the IMPCR for the destination
;register if it detects a difference in the scoreboard after turning on the
;floating point unit for one clock.  If this difference in the scoreboard was
;caused by the another exception, then the destination register and the result
;exponent showed in the IMPCR will be incorrect.  Therefore, another bit, 
;fmulbug, is added to the STKSTATE memory location to signify that more 
;exception have occured and been processed after the exception that caused the
;processor to enter this code segment.  If this fmulbug bit is set, then the
;handler should not modify the exponent of the destination register indicated
;by the IMPCR.  This fmulbug is not cleared when the exception for the u_block
;stack frame has been processed, but it is cleared when a new series of 
;exception are encountered during the user process.
		set	r1,r1,1<fmulbug>	

;|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


		st	r1,r31,STKSTATE	;set full bit in stkstate
		st	r2,r31,XR2	;get extra variable

;Now clear all the full bits for the remaining frames
		or	r1,r0,1		;start counter at 1
clearcmp:	cmp	r2,r1,NUMFRAME
		bb1	eq,r2,pointadjust	;full bits are all clear
		add	r1,r1,1		;increment r1
		addu	r31,r31,FRAMESIZE	;increment stack pointer
		br.n	clearcmp	;branch to compare again
		st	r0,r31,STKSTATE	;clear full bit
pointadjust:	subu	r31,r31,(NUMFRAME - 1)*FRAMESIZE ;readjust stack pointer
							 ;to the first frame
		br.n	storereg	;branch to store registers
		ld	r2,r31,XR2	;need valid r2 so that program can 
					;store it again later

;If necessary, find the next empty frame
findframe:	or.u	r31,r0,hi16(__U + u_xcpt) ;get address of bottom of stack
		or	r31,r31,lo16(__U + u_xcpt) 
findincr:	addu	r31,r31,FRAMESIZE	;increment point by a frame
		or.u	r1,r0,hi16(__U + u_xcpt) ;get address of bottom of stack
		or	r1,r1,lo16(__U + u_xcpt) 
		addu	r1,r1,NUMFRAME*FRAMESIZE ;form address of top of stack
		cmp	r1,r31,r1	;has the stack overflowed?
		bb1	ge,r1,_sigkill	;overflowed stack
		ld	r1,r31,STKSTATE	;load in word with full information
		bb1	full,r1,findincr;full frame, check next frame
		set	r1,r0,1<full>	;indicate frame is now full


;|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
;fmul.dxx imprecise followed by addpipe instruction workaround

;The workaround below will use the exponent in the IMPCR for the destination
;register if it detects a difference in the scoreboard after turning on the
;floating point unit for one clock.  If this difference in the scoreboard was
;caused by the another exception, then the destination register and the result
;exponent showed in the IMPCR will be incorrect.  Therefore, another bit, 
;fmulbug, is added to the STKSTATE memory location to signify that more 
;exception have occured and been processed after the exception that caused the
;processor to enter this code segment.  If this fmulbug bit is set, then the
;handler should not modify the exponent of the destination register indicated
;by the IMPCR.  This fmulbug is not cleared when the exception for the u_block
;stack frame has been processed, but it is cleared when a new series of 
;exception are encountered during the user process.
		set	r1,r1,1<fmulbug>	

;|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


		st	r1,r31,STKSTATE

;Save the user registers which will
;be used during the exception handling.  Load some integer and floating
;point unit (FPU) control registers into the general purpose registers,
;and also save these control registers onto the stack before the FPU
;is cleaned out.

storereg:	fldcr	r1,fpsr		;save FPSR before FP is enabled
		st	r1,r31,XFPSR     
		fldcr	r1,fpcr		;save FPCR before FP is enabled
		st	r1,r31,XFPCR
		fldcr	r1,fpecr	;save FPECR before FP is enabled
		st	r1,r31,XFPECR
		fldcr	r1,manthi	;save MANTHI before FP is enabled
		st	r1,r31,XMANTHI
		fldcr	r1,mantlo	;save MANTLO before FP is enabled
		st	r1,r31,XMANTLO
		ldcr	r1,spsr		;load SPSR
		st	r1,r31,XSPSR	;save SPSR before FP is enabled
		ldcr	r1,snip		;load SNIP
		st	r1,r31,XSNIP	;save SNIP before FP is enabled
		ldcr	r1,sfip		;load SFIP
		st	r1,r31,XSFIP	;save SFIP before FP is enabled
		fldcr	r1,impcr	;save IMPCR before FP is enabled
		st	r1,r31,XIMPCR

		bb0	destsize,r1,dacc;single precision result does not
					;need to have its scoreboard bit clear
		st	r2,r31,XR2	;need another register
		ldcr	r2,ssb		;load the shadow scoreboard
SSBdoub:	extu	r1,r1,5<rdimp>	;get low number of destination register
		add	r1,r1,1		;increment for the low word of result
		set	r1,r1,1<5>	;set width field to one
		clr	r2,r2,r1	;clear the scoreboard bit
SSBsing:	stcr	r2,ssb		;single precision result scoreboard bit
		ld	r2,r31,XR2
             
;check for data access fault
dacc:		ldcr	r1,dmt0		;load DMT0
		bb0	dmtvalid,r1,serialization;if no dacc fault, then serialize
;data fault handler will preserve this copy of the shadow scoreboard
;If the SPSR has the supervisor bit set, then the data fault handler will
;think that r31 contains the master stack value, which it does not.
;The data fault handler thinks that r1 is stored in scratch1, so store
;the real value of r1 out into memory and preserve fpup's stack pointer in
;scratch1.
		ldcr	r1,scratch1	;save original r1
		st	r1,r31,XR1	
		ldcr	r1,scratch2	;save original r31	
		st	r1,r31,XR31	
		stcr	r31,scratch1	;preserve stack pointer from the
					;data fault handler
		bsr	_getmsps	;branch to save registers, call dacchand
					;dacchand will clear shadow scoreboard
					;bits for data access faults
		bsr	_getrs		;branch to restore registers
		ldcr	r31,scratch1	;restore stack pointer from the
					;data fault handler
		ld	r1,r31,XR1	
		stcr	r1,scratch1	;save original r1
		ld	r1,r31,XR31	
		stcr	r1,scratch2	;save original r31	
		

;Load the instruction pointer except and the following instruction
;into SNIP and SFIP so that these will be the next instructions executed
;after cleaning out the FPU.  Set the valid bits and clear the exception 
;bits.  In the SPSR, enable exceptions, the FPU, and serialization.  RTE
;to clean out the floating point unit.


;|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
;fmul.dxx imprecise followed by addpipe instruction workaround
;and rd+1 destruction with FPU disable workaround

;The following code segment replaces the code between the serialization label
;down to but not include the except label in the file fpui.m4.  A new sub.m4 also
;accompanies this workaround for reassembly of fpui.m4.

;If the shadow scoreboard is already clear, then skip the serialization 
;procedures.  For simplicity, every instruction in the pipe must go through
;this serialization procedure since a fmul.ddd imprecise may have occured
;earlier in the pipeline.
serialization:	ldcr	r1,scratch1	;load orignal r1
		st	r27,r31,XR27	;store working registers, assume FPU
		st	r28,r31,XR28	;will not use these registers as dest.
		st	r29,r31,XR29
		st	r30,r31,XR30
		ldcr	r30,scratch2
		st	r30,r31,XR31
		ldcr	r29,ssb		;load SSB
		bcnd	eq0,r29,restorebug;rest of algorithm will not work if 
					;SSB is zero, so jump to send dummy
					;fmul through FPU
		stcr	r0,snip
		or.u	r30,r0,hi16(exceptbug)	;load instruction pointer
		or	r30,r30,lo16(exceptbug)
		set	r30,r30,1<valid>;set valid bit of instruction pointer
		stcr	r30,sfip
		ldcr	r28,psr          ;load PSR
		and	r28,r28,0xfffc	;leave FPU disabled
		stcr	r28,spsr  
		clr	r27,r28,1<FP_disable>	;PSR with FPU enabled
		or   r0,r0,r0
		rte			;put SSB into SB

;Enable the FPU for one clock until a floating point unit instruction writes
;back, detected by a change in the SSB.  Before a rework is done, verify that
;the exception that wrote back does not cause an exception and that it did not
;write back an integer result.
		align	8		;no cache miss between two instructions
exceptbug:	stcr	r27,psr		;enable FPU for one clock
		stcr	r28,psr
		or	r0,r0,r0	;allow another clock for SSB to update
		ldcr	r30,ssb		;check current shadow scoreboard
		sub	r30,r29,r30	;compare old and new shadow scoreboards
		bcnd	eq0,r30,exceptbug	;enable FPU again
		br	framecheck	;check if another imprecise occured

;Compress all of the bubbles out of the floating point pipe.  Clock count
;assumes a fdiv.ddd in stage 1 and stage 2 of addpipe, with bubbles in
;stage 3 and stage 4.  Since the last valid instruction in the writeback stage
;stays there if the FPU is disabled, only the disable bug garbage value will
;write back for one clock FPU execution.  Compressing bubbles in necessary for
;the two clock enable algorithm at exceptdisable to work.
		align	8		;no cache miss between two instructions
bubblecompress:	stcr	r27,psr		;enable FPU for one clock
		stcr	r28,psr
		sub	r29,r29,1	;decrement counter
		bcnd.n	ne0,r29,bubblecompress
		clr	r27,r28,1<FP_disable>	;r27 could have been 
						;corrupted by rd=26, so restore
						;r27

;After the bubbles have been compressed out, the FPU is now enabled two clocks
;at a time.  The first clock will cause the disable bug to write back, and the
;second clock allows the next valid instruction to enter the writeback stage 
;and write back.  If the disable cleared a SSB bit that was not already clear
;or would have been cleared by the valid instruction, then put that bit back
;into the SSB.
		align	16		;no cache miss between two instructions
exceptdisable:	stcr	r27,psr		;enable FPU for two clocks
		br.n	nop		;allow disable and valid instruction
					;to write back
					;This assembler does not allow 
					;convenient nop's
		stcr	r28,psr
nop:		bcnd	eq0,r30,restoredisable	;no scoreboard bits were to be
						;cleared by the disable bug
						;so there is no need to restore 
						;the ssb
		fldcr	r27,impcr		;load IMPCR
		extu	r27,r27,5<rdimp>;get destination register
		set	r27,r27,1<5>	;set field width to one
		set	r27,r0,r27	;corresponds to scoreboard of the
					;instruction which just wrote back
		and	r27,r27,r30	;if the scoreboard bits match, then 
					;the register that was just written 
					;back to was corrupted by the disable
					;bug 
		bcnd	ne0,r27,restoredisable	;scoreboard bits match, so no
						;need to restore another SSB bit
						;valid writeback
		ldcr	r29,ssb		;load remaining SSB bits
		or	r29,r29,r30	;reset SSB bit cleared by disable bug
		stcr	r29,ssb		

;Now restore any values that were destroyed by the enable bug, even if a
;previous instruction caused an exception and cleaned out the FPU, it still
;had a disable bug instruction write back before it.  Retrieve the pointer
;to the restore jump table plus the value to be restored from memory.
restoredisable:	ld	r30,r31,XR1	;load pointer into jump table
		ld	r29,r31,XR2	;load value to be restored from disable
					;bug
		jmp.n	r30		;jump to fix disable problem
		set	r28,r0,1<marker>	;tell disableret where
						;program came from

;See if another exception occured during the serialization of the FPU.  If one
;did, then do not fix the value in the register indicated by the IMPCR.
framecheck:	
						;There must be a following 
						;frame, otherwise handler 
						;would not be checking later
						;results
		ld	r27,r31,FRAMESIZE+STKSTATE	;see if more exceptions occured
		bb1	fmulbug,r27,restorebug	;FPU already cleared by later
						;exceptions in the pipe, but 
						;still execute dummy fmul

;Opcodes with an integer format result do not need to be adjusted, but their
;instruction is still stuck in the writeback stage.
;Also, fmul's do not need to be adjusted.
		fldcr	r27,impcr	;load IMPCR
		extu	r28,r27,5<11>	;get minor opcode
		cmp	r29,r28,FLTop	;compare to FLTop
		bb1	eq,r29,floatresult
		cmp	r29,r28,FADDop	;compare to FADDop
		bb1	eq,r29,floatresult
		cmp	r29,r28,FSUBop	;compare to FSUBop
		bb1	eq,r29,floatresult
		cmp	r29,r28,FDIVop	;compare to FDIVop
		bb1	eq,r29,floatresult
		br	disablebug	;instruction had integer format result

floatresult:	or.u	r29,r0,hi16(r0exp)
		or	r29,r29,lo16(r0exp)
		extu	r28,r27,5<rdimp>	;get destination
		mak	r28,r28,28<4>	;multiply by sixteen bytes per element
		addu	r29,r28,r29	;form complete address
		jmp.n	r29
		or	r30,r0,r1	;save r1

;Since the destination register is not known statically, code for that 
;destination register can not be written previously.  The jump table is used
;to move the correct result into the appropriate destination register.
r0exp:		or	r28,r0,r0
		or	r0,r0,r0
		br.n	disablebug
		or	r0,r0,r28
r1exp:		bsr.n	expform
		or	r28,r0,r30
		br.n	disablebug
		or	r1,r0,r28
r2exp:		bsr.n	expform
		or	r28,r0,r2
		br.n	disablebug
		or	r2,r0,r28
r3exp:		bsr.n	expform
		or	r28,r0,r3
		br.n	disablebug
		or	r3,r0,r28
r4exp:		bsr.n	expform
		or	r28,r0,r4
		br.n	disablebug
		or	r4,r0,r28
r5exp:		bsr.n	expform
		or	r28,r0,r5
		br.n	disablebug
		or	r5,r0,r28
r6exp:		bsr.n	expform
		or	r28,r0,r6
		br.n	disablebug
		or	r6,r0,r28
r7exp:		bsr.n	expform
		or	r28,r0,r7
		br.n	disablebug
		or	r7,r0,r28
r8exp:		bsr.n	expform
		or	r28,r0,r8
		br.n	disablebug
		or	r8,r0,r28
r9exp:		bsr.n	expform
		or	r28,r0,r9
		br.n	disablebug
		or	r9,r0,r28
r10exp:		bsr.n	expform
		or	r28,r0,r10
		br.n	disablebug
		or	r10,r0,r28
r11exp:		bsr.n	expform
		or	r28,r0,r11
		br.n	disablebug
		or	r11,r0,r28
r12exp:		bsr.n	expform
		or	r28,r0,r12
		br.n	disablebug
		or	r12,r0,r28
r13exp:		bsr.n	expform
		or	r28,r0,r13
		br.n	disablebug
		or	r13,r0,r28
r14exp:		bsr.n	expform
		or	r28,r0,r14
		br.n	disablebug
		or	r14,r0,r28
r15exp:		bsr.n	expform
		or	r28,r0,r15
		br.n	disablebug
		or	r15,r0,r28
r16exp:		bsr.n	expform
		or	r28,r0,r16
		br.n	disablebug
		or	r16,r0,r28
r17exp:		bsr.n	expform
		or	r28,r0,r17
		br.n	disablebug
		or	r17,r0,r28
r18exp:		bsr.n	expform
		or	r28,r0,r18
		br.n	disablebug
		or	r18,r0,r28
r19exp:		bsr.n	expform
		or	r28,r0,r19
		br.n	disablebug
		or	r19,r0,r28
r20exp:		bsr.n	expform
		or	r28,r0,r20
		br.n	disablebug
		or	r20,r0,r28
r21exp:		bsr.n	expform
		or	r28,r0,r21
		br.n	disablebug
		or	r21,r0,r28
r22exp:		bsr.n	expform
		or	r28,r0,r22
		br.n	disablebug
		or	r22,r0,r28
r23exp:		bsr.n	expform
		or	r28,r0,r23
		br.n	disablebug
		or	r23,r0,r28
r24exp:		bsr.n	expform
		or	r28,r0,r24
		br.n	disablebug
		or	r24,r0,r28
r25exp:		bsr.n	expform
		or	r28,r0,r25
		br.n	disablebug
		or	r25,r0,r28
r26exp:		bsr.n	expform
		or	r28,r0,r26
		br.n	disablebug
		or	r26,r0,r28

;Write the correct exponent, derived from the IMPCR, into the destination
;register.  The sign bit, from MANTHI, needs to be modified.  If the 
;destination size is double precision, then also write back the low word.
expform:	or	r29,r0,r1	;place return pointer in r29
		or	r1,r0,r30	;restore original value of r1
		bb0.n	destsize,r27,destlow	;branch to handle single dest.
		extu	r30,r27,12<20>	;extract exponent
desthigh:	set	r27,r0,1<10>	;set bit 10
		xor	r30,r27,r30	;complement bit 10 of exponent
		mak	r30,r30,11<hiddend>	;move correct exp. into field
		clr	r28,r28,12<hiddend>	;clear old exponent and sign
		or	r28,r28,r30	;load in new exponent 
		fldcr	r30,manthi	;load MANTHI
		clr	r30,r30,31<0>	;clear all but sign bit
		or	r28,r28,r30	;load in new exponent and sign
		jmp	r29		;return to table
destlow:	set	r27,r0,1<7>	;set bit 7
		xor	r30,r27,r30	;complement bit 7 of exponent
		mak	r30,r30,8<hiddens>	;move correct exp. into field
		clr	r28,r28,9<hiddens>	;clear old exponent and sign
		or	r28,r28,r30	;load in new exponent 
		fldcr	r30,manthi	;load MANTHI
		clr	r30,r30,31<0>	;clear all but sign bit
		jmp.n	r29		;return to table
		or	r28,r28,r30	;load in new exponent and sign

;Now that the exponent problem has been handled, the processor needs to deal
;with the problem of the rd+1 register being corrupted because the FPU was
;enabled and then immediately disabled.  If the scoreboard bit for this
;register is set, then no action needs to be taken because the correct result
;will be written into after the destruction.  If the scoreboard bit is not set,
;then the value of this register must be saved while the FPU is serialized.
disablebug:	fldcr	r27,impcr	;load IMPCR again, sorry no more scratch
					;registers
		bb0	destsize,r27,disablelow	;do not write low if single
disablehigh:	ldcr	r30,psr		;load PSR with FPU disabled
		clr	r29,r30,1<FP_disable>	;PSR with FPU enabled
		align	8
		stcr	r29,psr		;enable FPU for one clock to write
					;back double
		stcr	r30,psr
disablelow:	extu	r27,r27,5<rdimp>;extract destination register
		add	r27,r27,1	;increment rd
		set	r30,r27,1<5>	;set width field of one
		set	r30,r0,r30	;corresponds to scoreboard bit for rd+1
		ldcr	r29,ssb		;load SSB
		and	r29,r29,r30	;Is SSB bit set for rd+1?
		or.u	r28,r0,hi16(r0rdp1)	;bottom of jumptable
		or	r28,r28,lo16(r0rdp1)
		bcnd	ne0,r29,disablejmp	;avoid offset into jump table
						;register which will be 
						;corrupted will later be written
						;to
		or	r30,r0,r0	;clear r30 if no scoreboard bits will
					;be cleared by the disable bug
		mak	r27,r27,28<4>	;multiply by 16 bytes
		addu	r28,r27,r28
disablejmp:	jmp.n	r28		;branch to jumptable
		or	r27,r0,r1

r0rdp1:		bsr.n	disableset
		or	r29,r0,r0
		br.n	disableret
		or	r0,r0,r29	;write over corrupted value
r1rdp1:		bsr.n	disableset
		or	r29,r0,r27
		br.n	disableret
		or	r1,r0,r29
r2rdp1:		bsr.n	disableset
		or	r29,r0,r2
		br.n	disableret
		or	r2,r0,r29
r3rdp1:		bsr.n	disableset
		or	r29,r0,r3
		br.n	disableret
		or	r3,r0,r29
r4rdp1:		bsr.n	disableset
		or	r29,r0,r4
		br.n	disableret
		or	r4,r0,r29
r5rdp1:		bsr.n	disableset
		or	r29,r0,r5
		br.n	disableret
		or	r5,r0,r29
r6rdp1:		bsr.n	disableset
		or	r29,r0,r6
		br.n	disableret
		or	r6,r0,r29
r7rdp1:		bsr.n	disableset
		or	r29,r0,r7
		br.n	disableret
		or	r7,r0,r29
r8rdp1:		bsr.n	disableset
		or	r29,r0,r8
		br.n	disableret
		or	r8,r0,r29
r9rdp1:		bsr.n	disableset
		or	r29,r0,r9
		br.n	disableret
		or	r9,r0,r29
r10rdp1:	bsr.n	disableset
		or	r29,r0,r10
		br.n	disableret
		or	r10,r0,r29
r11rdp1:	bsr.n	disableset
		or	r29,r0,r11
		br.n	disableret
		or	r11,r0,r29
r12rdp1:	bsr.n	disableset
		or	r29,r0,r12
		br.n	disableret
		or	r12,r0,r29
r13rdp1:	bsr.n	disableset
		or	r29,r0,r13
		br.n	disableret
		or	r13,r0,r29
r14rdp1:	bsr.n	disableset
		or	r29,r0,r14
		br.n	disableret
		or	r14,r0,r29
r15rdp1:	bsr.n	disableset
		or	r29,r0,r15
		br.n	disableret
		or	r15,r0,r29
r16rdp1:	bsr.n	disableset
		or	r29,r0,r16
		br.n	disableret
		or	r16,r0,r29
r17rdp1:	bsr.n	disableset
		or	r29,r0,r17
		br.n	disableret
		or	r17,r0,r29
r18rdp1:	bsr.n	disableset
		or	r29,r0,r18
		br.n	disableret
		or	r18,r0,r29
r19rdp1:	bsr.n	disableset
		or	r29,r0,r19
		br.n	disableret
		or	r19,r0,r29
r20rdp1:	bsr.n	disableset
		or	r29,r0,r20
		br.n	disableret
		or	r20,r0,r29
r21rdp1:	bsr.n	disableset
		or	r29,r0,r21
		br.n	disableret
		or	r21,r0,r29
r22rdp1:	bsr.n	disableset
		or	r29,r0,r22
		br.n	disableret
		or	r22,r0,r29
r23rdp1:	bsr.n	disableset
		or	r29,r0,r23
		br.n	disableret
		or	r23,r0,r29
r24rdp1:	bsr.n	disableset
		or	r29,r0,r24
		br.n	disableret
		or	r24,r0,r29
r25rdp1:	bsr.n	disableset
		or	r29,r0,r25
		br.n	disableret
		or	r25,r0,r29
r26rdp1:	bsr.n	disableset
		or	r29,r0,r26
		br.n	disableret
		or	r26,r0,r29
r27rdp1:	bsr.n	disableset
		or	r29,r0,r0
		br.n	disableret
		or	r0,r0,r29

;Now that we know which register needs to be restored once the FPU is enabled,
;then check the SSB to see if it is clear and only one more restore needs to
;be done, or if it is set and more serialization is necessary.
disableset:	or	r28,r0,r1	;load return address into r30
		or	r1,r0,r27	;restore r1
		ldcr	r27,ssb		;load SSB
		bcnd	eq0,r27,disableclr	;enable FPU one last time	
		st	r29,r31,XR2	;save contents of register to be 
					;destroyed by disable bug
		st	r28,r31,XR1	;save return address pointer to 
					;disable jump table
;Bubble compression only needs to be done once.  Check the STKSTATE for this
;frame to see if it has already been performed for this frame.
		ld	r29,r31,STKSTATE
		ldcr	r28,psr		;load PSR with FPU disabled
		clr	r27,r28,1<FP_disable>	;PSR with FPU enabled
		bb0	compress,r29,bubbles	;bubbles need to be compressed
		br	exceptdisable	;continue until SSB clear

bubbles:	set	r29,r29,1<compress>	;indicate that bubbles have been
						;compressed
		st	r29,r31,STKSTATE
		br.n	bubblecompress
		or	r29,r0,117	;55 for fdiv.ddd + 55 for fdiv.ddd +
					;1 for 2nd fdiv to move into stage 3 +
					;6 for safety

;Enable FPU one last time.  Use r28 to tell disableret whether the access to
;jump table was done from here or from restoredisable.
disableclr:	ldcr	r27,psr		;load PSR
		clr	r27,r27,1<FP_disable>	;enable FPU
		stcr	r27,psr
		jmp.n	r28		;correct corrupted value
		clr	r28,r0,1<marker>	;bit 1 clear for disableret
						;so program knows where jump
						;originated

disableret:	bb1	marker,r28,framecheck	;branch to the one of two
						;places that disableret can go

;Now that the FPU is completely flushed out, restore scratch registers to their
;previous values and send a dummy fmul through the pipe to reset the state bit
;stuck during the fmul.dxx imprecise exception.
restorebug:	ldcr	r29,psr	
		and	r29,r29,0xfff4	;enable FPU, int., and excp. for rest
					;of handler
		stcr	r29,psr		;enable PSR for the two sections of code
					;which did not enable the PSR before
					;coming here
		or.u	r29,r0,0x3ff0	;multiply 1 X 1
		or	r30,r0,r0
		fmul.ddd r29,r29,r29
		ld	r27,r31,XR27	;restore original values
		ld	r28,r31,XR28
		ld	r29,r31,XR29
		ld	r30,r31,XR30
		ld	r31,r31,XR31

;|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||



;After the floating point unit is flushed out, then we will be at this location

except:		tb1	0,r0,0		;force all FPU instructions to complete
		stcr	r1,scratch1	;exchange USP with scratch2
		stcr	r31,scratch2	;exchange r1 with scratch1

;Find the last full exception frame.

exceptframe:	or.u	r31,r0,hi16(__U + u_xcpt) ;get address of bottom of stack
		or	r31,r31,lo16(__U + u_xcpt) 
exceptincr:	ld	r1,r31,STKSTATE	;load in word with full bit
		bb1	full,r1,nextframe
		br.n	reenable	;full bit clear, last frame was full
					;branch to rest of exception processing
		subu	r31,r31,FRAMESIZE	;set pointer to last full frame
		
nextframe:	addu	r31,r31,FRAMESIZE	;increment point by a frame
		or.u	r1,r0,hi16(__U + u_xcpt) ;get address of bottom of stack
		or	r1,r1,lo16(__U + u_xcpt) 
		addu	r1,r1,NUMFRAME*FRAMESIZE ;form address of top of stack
		cmp	r1,r31,r1	;has the stack overflowed?
		bb1	lt,r1,exceptincr	;not last frame of stack
		subu	r31,r31,FRAMESIZE ;go to last frame of stack

;Store the user values of the registers that are needed for the rest of the
;code.  Load the values of the control registers.

reenable:	ldcr	r1,scratch1	;retrieve original r1
		st	r1,r31,XR1	;save r1 and r2 into supervisor memory
		ldcr	r1,scratch2	;load r1 with original stack pointer
		st	r1,r31,XR31	;save original stack pointer into memory
		st	r2,r31,XR2        
		st	r3,r31,XR3	;save needed general purpose registers
		st	r4,r31,XR4	      
		st	r5,r31,XR5	      
		st	r6,r31,XR6	      
		st	r7,r31,XR7	      
		st	r8,r31,XR8	      
		st	r9,r31,XR9	      
		st	r10,r31,XR10	      
		st	r11,r31,XR11      
		st	r12,r31,XR12      
		st	r13,r31,XR13	;save r13 into memory, get extra reg.
	     
		ld	r1,r31,XSPSR	;get SPSR for unimp/FPU disable check
		ld	r2,r31,XFPSR	;load control registers after cleaning
		ld	r3,r31,XFPCR	;out FPU during the RTE
		ld	r4,r31,XFPECR
		ld	r10,r31,XMANTHI
		ld	r11,r31,XMANTLO
		ld	r12,r31,XIMPCR  

;Load into r1 the return address for the exception handlers.  Looking
;at FPECR, branch to the appropriate exception handler.

		or.u	r1,r0,hi16(wrapup);load return address of functions
		bb1.n	2,r4,_FPunderflow ;branch to FPunderflow if bit set
		or	r1,r1,lo16(wrapup)
		bb1	1,r4,_FPoverflow ;branch to FPoverflow if bit set
		br	_handler	;branch to handler since bit will be set
					;for inexact

;To write back the results to the user registers, disable exceptions
;and the floating point unit.  Write FPSR and FPCR and load the SNIP
;and SFIP.
;r10 contains the upper word of the result
;r11 contains the lower word of the result

wrapup:      ld     r6,r31,XSFIP
             ldcr   r5,psr           ;load the PSR
            ;for imprecise, no previous floating point instructions could
            ;have been executed
		tb1	0,r0,0
		or	r5,r5,0x2	;disable interrupts
		stcr	r5,psr
             or     r5,r5,0x9        ;set disable bit, disable exceptions
             			     ;set SFU 1 disable bit, disable SFU 1
             stcr   r5,psr
             ld     r5,r31,XSNIP
             fstcr  r2,fpsr          ;write revised value of FPSR
             ld     r2,r31,XSPSR     ;load control registers from stack
             fstcr  r3,fpcr          ;write revised value of FPCR
             
             stcr   r5,snip          ;store control registers
             stcr   r2,spsr          
             stcr   r6,sfip
            ;writeback routine


;If the destination is double, then execute both the "writedouble" and
;"writesingle" code, but if it is single, then only execute the 
;"writesingle code.  If the destination registers is r1 - r12, r31, then
;write to the portion in memory where that register value is stored.  If
;the destination register is r13 - r30, then write to the registers
;directly.  The shadow scoreboard bit is cleared for that particular 
;destination register after it is written.

             bb0.n  destsize,r12,writesingle ;branch to write single 
                                             ;if single dest.
             extu   r2,r12,5<rdimp>      ;get 5 bits for destination register 
writedouble: cmp    r3,r2,13         ;see if destination register is 12 or less
             bb1    le,r3,loadmemd   ;load the memory directly
             cmp    r8,r2,31         ;see if destination register is the SP
             bb1    ne,r8,loadregd   ;not stack pointer, then write to registers
loadmemd:    br.n   increment        ;branch to increment destination register
             st     r10,r31[r2]      ;write high word
loadregd:    or.u   r4,r0,hi16(regtable)   ;load address of register table
             or     r4,r4,lo16(regtable)
             sub    r2,r2,14         ;adjust pointer to regtable
             lda.d  r5,r4[r2]        ;modify address in register table
             or.u   r1,r0,hi16(moddestd)   ;load r1 with return address moddestd
             or     r1,r1,lo16(moddestd)
             jmp.n  r5               ;jump to load register value
             or     r7,r0,r10        ;place high word in r10
moddestd:    add    r2,r2,14         ;readjust destination for scoreboard

increment:   add    r2,r2,1          ;if destination is double, then the 
                                     ;register number specifies the high word	
             clr    r2,r2,27<5>      ;equivalent to mod 32
writesingle: cmp    r3,r2,13         ;see if destination register is 12 or less
             bb1    le,r3,loadmems   ;load the results into memory
             cmp    r8,r2,31         ;see if destination register is the SP
             bb1    ne,r8,loadregs   ;not stack pointer, then write to registers
loadmems:    br.n   endwrite         ;branch to end of write back code
             st     r11,r31[r2]      ;write low word into memory
loadregs:    or.u   r4,r0,hi16(regtable)   ;load address of register table
             or     r4,r4,lo16(regtable)
             sub    r2,r2,14         ;adjust pointer to regtable
             lda.d  r5,r4[r2]        ;modify address in register table
             or.u   r1,r0,hi16(endwrite)   ;load r1 with return address moddests
             or     r1,r1,lo16(endwrite)
             jmp.n  r5               ;jump to load register value
             or     r7,r0,r11        ;place low word in r7
             
;The register table is used for directly writing the result to the
;destination register.

regtable:	jmp.n	r1		;load register with result
		or	r14,r0,r7	      
		jmp.n	r1
		or	r15,r0,r7
		jmp.n	r1
		or	r16,r0,r7
		jmp.n	r1
		or	r17,r0,r7
		jmp.n	r1
		or	r18,r0,r7
		jmp.n	r1
		or	r19,r0,r7
		jmp.n	r1
		or	r20,r0,r7
		jmp.n	r1
		or	r21,r0,r7
		jmp.n	r1
		or	r22,r0,r7
		jmp.n	r1
		or	r23,r0,r7
		jmp.n	r1
		or	r24,r0,r7
		jmp.n	r1
		or	r25,r0,r7
		jmp.n	r1
		or	r26,r0,r7
		jmp.n	r1
		or	r27,r0,r7
		jmp.n	r1
		or	r28,r0,r7
		jmp.n	r1
		or	r29,r0,r7
		jmp.n	r1
		or	r30,r0,r7


;Load the user register values from memory.

endwrite:	ld	r2,r31,XR2	;load registers 2 - 13 from memory
		ld	r3,r31,XR3
		ld	r4,r31,XR4
		ld	r5,r31,XR5
		ld	r6,r31,XR6
		ld	r7,r31,XR7
		ld	r8,r31,XR8
		ld	r9,r31,XR9
		ld	r10,r31,XR10
		ld	r11,r31,XR11
		ld	r12,r31,XR12
		ld	r13,r31,XR13

;Clear the STKSTATE full bit
		ld	r1,r31,STKSTATE
		clr	r1,r1,1<full>	;clear the full bit
		st	r1,r31,STKSTATE

;Clear the sfu1full boolean in case the user gets any more exceptions during 
;this process.  Only clear when this frame is the first one.
		or.u	r1,r0,hi16(__U + u_xcpt) ;get upper half-word of 
						  ;first frame pointer
		or	r1,r1,lo16(__U + u_xcpt) ;get lower half-word of 
						   ;first frame pointer
		cmp	r1,r31,r1	;see if the frame is first frame
		bb1	ne,r1,switch	;do no clear bit in u_block

		or.u	r1,r0,hi16(__U + u_sfu1full) ;get hi address of sfu1full
		or	r1,r1,lo16(__U + u_sfu1full) ;get lo address of sfu1full
		st	r0,r1,0

;Now do check to see if the user's time has run out.  If the time
;clock is zero, then make sure that the SPSR indicates user mode before
;branching to _fptrap.

switch:		or.u	r1,r0,hi16(_runrun)	;get contents of _runrun
		ld	r1,r1,lo16(_runrun)
		bcnd	ne0,r1,return	;process still has time remaining
		ldcr	r1,spsr		;grab original SPSR
		bb1	mode,r31,return	;supervisor caused exception, so
					;return even though clock has run out
		or.u	r1,r0,hi16(__U + USIZE)	;load top of system stack onto
		or	r1,r1,lo16(__U + USIZE)	;r1 since we are going to
						;destroy r1 with the bsr
;Decrement and save registers.  Since the process will not need to do
;this operation again with the current data still stored on the stack,
;do not worry about losing your stack pointer.  When the code returns,
;this routine will just grab it again.
		subu	r1,r1,SWITCHUSER;allow space for saving registers
		st	r2,r1,XR2	;save registers
		st	r3,r1,XR3
		st	r4,r1,XR4
		st	r5,r1,XR5
		st	r6,r1,XR6
		st	r7,r1,XR7
		st	r8,r1,XR8
		st	r9,r1,XR9
		st	r10,r1,XR10
		st	r11,r1,XR11
		st	r12,r1,XR12
		st	r13,r1,XR13
		st	r14,r1,XR14
		st	r15,r1,XR15
		st	r16,r1,XR16
		st	r17,r1,XR17
		st	r18,r1,XR18
		st	r19,r1,XR19
		st	r20,r1,XR20
		st	r21,r1,XR21
		st	r22,r1,XR22
		st	r23,r1,XR23
		st	r24,r1,XR24
		st	r25,r1,XR25
		st	r26,r1,XR26
		st	r27,r1,XR27
		st	r28,r1,XR28
		st	r29,r1,XR29
		st	r30,r1,XR30
		bsr.n	_ftrap		;jump to operating system routine to
					;switch users
		st	r31,r1,XR31	;worked hard to get this pointer to
					;the exception stack, so save it
		or.u	r1,r0,hi16(__U + USIZE)	;load top of system stack onto
		or	r1,r1,lo16(__U + USIZE)	;r1 to retrieve old register
						;values
		subu	r1,r1,SWITCHUSER;allow space for saving registers
		ld	r2,r1,XR2	;save registers
		ld	r3,r1,XR3
		ld	r4,r1,XR4
		ld	r5,r1,XR5
		ld	r6,r1,XR6
		ld	r7,r1,XR7
		ld	r8,r1,XR8
		ld	r9,r1,XR9
		ld	r10,r1,XR10
		ld	r11,r1,XR11
		ld	r12,r1,XR12
		ld	r13,r1,XR13
		ld	r14,r1,XR14
		ld	r15,r1,XR15
		ld	r16,r1,XR16
		ld	r17,r1,XR17
		ld	r18,r1,XR18
		ld	r19,r1,XR19
		ld	r20,r1,XR20
		ld	r21,r1,XR21
		ld	r22,r1,XR22
		ld	r23,r1,XR23
		ld	r24,r1,XR24
		ld	r25,r1,XR25
		ld	r26,r1,XR26
		ld	r27,r1,XR27
		ld	r28,r1,XR28
		ld	r29,r1,XR29
		ld	r30,r1,XR30
		ld	r31,r1,XR31;load pointer to exception frame
	
;Using r1, retrieve the original stack pointer.
;RTE to the user code
;where the exception occurred.

return:		ld	r1,r31,XR31	;load value of original stack pointer
		stcr	r1,scratch2	;store orig. stack pointer in scratch2
		ld	r1,r31,XR1	;load real value of r1
		ldcr	r31,scratch2	;get original stack pointer
		or   r0,r0,r0
		rte			;return to normal operation
 
		data


E 1

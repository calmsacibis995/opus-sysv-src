h14815
s 00004/00000/01035
d D 1.2 90/03/16 18:31:49 root 2 1
c xx.usr bug wortaround
e
s 01035/00000/00000
d D 1.1 90/03/06 12:46:21 root 1 0
c date and time created 90/03/06 12:46:21 by root
e
u
U
t
T
I 1
;####################################################################
;		File:  trap.s
;
;  This package provides exception handling for Unix.  Basically,
;  exceptions are divided into one of several basic types:
;
;  1)  Interrupts (Notably the clock, handled in arch.s)
;  2)  System calls.
;  3)  Traps (divide by 0, illegal instructions, etc.)
;  4)  Bus Errors (handled in paging.s)
;
;####################################################################
;
;   Register definitions.
;
;	psr	cr1
;	tpsr	cr2
;	tsb	cr3
;	tcip	cr4
;	tnip	cr5
;	tfip	cr6
;	vbr	cr7
;	tran1	cr8
;	data1	cr9
;	addr1	cr10
;	tran2	cr11
;	data2	cr12
;	addr2	cr13
;	tran3	cr14
;	data3	cr15
;	addr3	cr16
;	cscratch cr17
;	cscratch cr18  
;	cisp	cr19
;	ROM     cr20   ; used for scratch - kernel must restore on exit.

;####################################################################
;
; Common interrupt return for all interrupt routines
;
; Interrupts use the interrupt stack.  It is important that this
; stack gets cleaned up if we are going to switch to another process.
; Process switching can only occur on the master stack.  If we have 
; nested interrupts (normal stack frame), then don't process switch.
; Just return from interrupt to the previous interrupt.
;
;####################################################################

	global	_intret
	global	intret
_intret:
intret:
; All interrupts come here for special handling.
	
; At this point, we know we are using the master stack 

resched:			; Determine if rescheduling is neccessary.

; Here we try to simulate a software interrupt.  After all pending
; interrupts are serviced and we are going to return to level 0
; we drop the mask to allow any low priority interrupts to be 
; serviced switch to the interrupt stack and call the timer interrupt.

	or.u	r2,r0,hi16(_timeflag)	; check if we should run timein()
	ld	r2,r2,lo16(_timeflag)
	bcnd	eq0,r2,nottime		; No, nothing to run

	ld	r3,r31,PS+HOMESP	; Yes, but are we going to level 0?
	bb1	PS_IPL,r3,nottime

	extu	r3,r3,3<PS_MSK>
	bcnd	ne0,r3,nottime

	or.u	r2,r0,hi16(_timeflag)	; check if we should run timein()
	st	r0,r2,lo16(_timeflag)

;	bsr	_spl0		; timein does spl7. MV.

	bsr	_timein		;

nottime:

	or.u	r2,r0,hi16(_netisr)
	ld	r2,r2,lo16(_netisr)	; If netisr is set, then
	bcnd	eq0,r2,tstsched		; reschedule.
	bsr	_softnetint
tstsched:

	ld	r2,r31,SPSR+HOMESP
	bb1	PS_SUP,r2,getregs	; If interrupted in system mode,
					; then don't reschedule.
	or.u	r2,r0,hi16(_runrun)
	ld.b	r2,r2,lo16(_runrun)		; If runrun is clear, then don't
	bcnd	eq0,r2,getregs		; reschedule.
;
; Rescheduling is needed if control comes here.
; We need to call trap with 256 for vecnum to reschedule.
;
	add	r3,r0,RESCHED		; Pass resched number 
	add	r2,r31,HOMESP		; Pass pointer to array.
	bsr	_trap			; Call trap to reschedule it.
	br	getregs			; Save all the registers and old r31.

;####################################################################
;
;			Xtrap - All trap calls
;
;####################################################################

Xtrap:		global	Xtrap
;  Miscellaneous unexpected exceptions come here.
;
	xcr	r2,r2,cr19
	st	r1,r2,0x10
	xcr	r2,r2,cr19
	bsr.n	getmsps			; Switch stacks and save regs.
	stcr	r31,cr17

	ldcr	r2,cr19
	ld	r2,r2,0x10

	or.u	r1,r0,hi16(_M88Kvec)
	or	r1,r1,lo16(_M88Kvec)
	sub	r2,r2,r1
	sub	r3,r2,8
	extu	r3,r3,9<3>
calltrap:
;  All software exceptions come here.
	bsr.n	_trap			; Finished w/ parameters, make the call.
	add	r2,r31,HOMESP		; Pass pointer to all.
	br	getregs

;###################################################################
;  Misaligned address exception.
;
	global	Xaddr
Xaddr:
	stcr	r1,cr18		; Performed in vector table.
	ldcr	r1,cr8		; cpu bug
	stcr	r1,cr20

	ldcr	r1,cr1
	and.u	r1,r1,0x9fff		; reset BO, SER
	and	r1,r1,0xfffb		; reset MXM
	stcr	r1,cr1

	ldcr	r1,cr4		; Get xip (points to faulting instruction)
	clr	r1,r1,2<0>	; and clear lower 2 bits (valid and except).

	stcr	r31,cr17	; Get another register for work.
	ldcr	r31,cr2		; Get the spsr.
	bb0	PS_SUP,r31,umis	; Branch if user misaligned.
	ld	r1,r1,r0	; Get faulting instruction.
	br	amis
umis:
I 2
	or	r0,r0,r0		; DELAY
E 2
	ld.usr	r1,r1,r0	; Get faulting instruction.
I 2
	or	r0,r0,r0		; DELAY2
	or	r0,r0,r0		; DELAY2
	or	r0,r0,r0		; DELAY2
E 2
amis:
	extu	r31,r1,5<21>	; Get destination register number.
	addu	r31,r31,0x40	
	ldcr	r1,cr3		; Clear scoreboard bits.
	clr	r1,r1,r31
	stcr	r1,cr3

	ldcr	r1,cr20		; restore cr8
	stcr	r1,cr8

	bsr	getmsps

	br.n	calltrap
	addu	r3,r0,ADDRERR
;########################################################################
; Illegal instruction exception.
;
	global	Xinst
Xinst:
	stcr	r1,cr18		; Already done in vector table.
	bsr.n	getmsps		; Switch stacks and save regs.
	stcr	r31,cr17

	br.n	calltrap  ; If user, handle thru trap.
	add	r3,r0,INSTERR

;########################################################################
;  privilege violation exception.
;
	global	Xpriv
Xpriv:
	stcr	r1,cr18		; Already done in vector table.
	bsr.n	getmsps		; Switch stacks and save regs.
	stcr	r31,cr17

	br.n	calltrap
	add	r3,r0,PRIVFLT
;########################################################################
;  outof bounds exception.
;
	global	Xtbnd
Xtbnd:
	stcr	r1,cr18		; Already done in vector table.
	bsr.n	getmsps		; Switch stacks and save regs.
	stcr	r31,cr17

	br.n	calltrap
	add	r3,r0,CHKTRAP
;########################################################################
;  Integer divide by zero exception.
;
	global	Xdivz
Xdivz:
	stcr	r1,cr18		; Already done in vector table.
	stcr	r2,cr17		; Save in temps
	ldcr	r1,cr8
	stcr	r1,cr20

	ldcr	r1,cr1
	and.u	r1,r1,0x9fff		; reset BO, SER
	and	r1,r1,0xfffb		; reset MXM
	stcr	r1,cr1

	ldcr	r2,cr4		; Gotta kill the scoreboard on this
	ldcr	r1,cr2		; Check for kernel mode.
	bb0.n	PS_SUP,r1,Xdivusr
	and	r2,r2,0xFFFC	; exception.
	ld	r2,r2,0
	br	Xdivssb
Xdivusr:
	bsr	kfuword		; Get instruction word at XIP.
Xdivssb:
	extu	r2,r2,5<21>	; Get reg num from instruction word
	or	r2,r2,0x20	; and clear 1 bit in SSB corresponding
	ldcr	r1,cr3		; to destination of divs that screwed
	set	r2,r0,r2 ; get bit to clear
	and.c	r1,r1,r2	; up.
	stcr	r1,cr3

	ldcr	r2,cr17		; restore r2
	ldcr	r1,cr20
	stcr	r1,cr8

	bsr.n	getmsps		; Switch stacks and save regs.
	stcr	r31,cr17

	br.n	calltrap
	add	r3,r0,ZDVDERR
;########################################################################
;  Integer overflow exception.
;
	global	Xiovf
Xiovf:
	stcr	r1,cr18		; Already done in vector table.
	bsr.n	getmsps		; Switch stacks and save regs.
	stcr	r31,cr17

	br.n	calltrap
	add	r3,r0,INTOVF
;########################################################################
;  Floating point exception.
;
;########################################################################
;  User breakpoint exception.
;
	global	Xtrace
Xtrace:
	stcr	r1,cr18		; Already done in vector table.
	ldcr	r1,cr2
	bb1	PS_SUP,r1,kerntrace  ; If user, handle thru trap.

	bsr.n	getmsps		; Switch stacks and save regs.
	stcr	r31,cr17
	ld	r2,r31,HOMESP+PS

	bb0.n	PS_SUP,r2,calltrap  ; If user, handle thru trap.
	add	r3,r0,BKBPT

;  Trace instruction in kernel mode.
	bsr	getrs		; Restore registers.
kerntrace:
	or.u	r1,r0,hi16(_dbgtrace) ; Jump to KDB/ROM debugger.
	ld	r1,r1,lo16(_dbgtrace)
	jmp.n	r1
	ldcr	r1,cr18

;########################################################################
;  User breakpoint exception.
;
	global	Xbkpt
Xbkpt:
	stcr	r1,cr18		; Already done in vector table.
	ldcr	r1,cr2
	bb1	PS_SUP,r1,kernbkpt  ; If user, handle thru trap.

	bsr.n	getmsps		; Switch stacks and save regs.
	stcr	r31,cr17
	ld	r2,r31,HOMESP+PS

	bb0.n	PS_SUP,r2,calltrap  ; If user, handle thru trap.
	add	r3,r0,BKBPT

;  Breakpoint instruction in kernel mode.
	bsr	getrs		; Restore registers.
kernbkpt:
	or.u	r1,r0,hi16(_dbgbkpt) ; Jump to KDB/ROM debugger.
	ld	r1,r1,lo16(_dbgbkpt)
	jmp.n	r1
	ldcr	r1,cr18

;########################################################################
;  IOT exception.
;
	global	Xiot
Xiot:
	stcr	r1,cr18		; Already done in vector table.
	ldcr	r1,cr2
	bb1	PS_SUP,r1,kerniot  ; If user, handle thru trap.

	bsr.n	getmsps		; Switch stacks and save regs.
	stcr	r31,cr17
	ld	r2,r31,HOMESP+PS

	bb0.n	PS_SUP,r2,calltrap  ; If user, handle thru trap.
	add	r3,r0,IOTTRAP

;  iot instruction in kernel mode.
	bsr	getrs		; Restore registers.
kerniot:
	or.u	r1,r0,hi16(_dbgbkpt) ; Jump to KDB/ROM debugger.
	ld	r1,r1,lo16(_dbgbkpt)
	jmp.n	r1
	ldcr	r1,cr18

;
;#########################################################################
	global	_fpertn
	global	getmsps
	global	flush
getmsps:
;    This subroutine will switch to the master stack (if not already on
;  the master stack) and save all registers on the stack.  Data fault 
; registers are also saved on the stack if a data fault is pending.
; buserr() is called, if neccessary, to complete the faulted access and
; fix up the register images on the stack.
;
;  Assumes:
;   r1 is saved in cr18
;   r31 is saved in cr17
;
;   Normally called as:
;	stcr	r1,cr18
;	bsr.n	getmsps
;	stcr	r31,cr17
;
;   Returns:
;	r2  through r7 are preserved (argument registers.)
;	other registers may be trashed.
;
	ldcr	r31,cr8			; save cr8 CPU BUG
	stcr	r31,cr20		; save cr8 CPU BUG

#ifdef LOGICANALYSER
	bb0	12,r31,noxmem
	st	r31,r0,212
noxmem:
#endif
	ldcr	r31,cr1
	and.u	r31,r31,0x9fff		; reset BO, SER
	and	r31,r31,0xfffb		; reset MXM
	stcr	r31,cr1

	ldcr	r31,cr2			; Get SPSR
	bb1.n	PS_SUP,r31,ONSUP
	ldcr	r31,cr17
	or.u	r31,r0,hi16(_u+USIZE)	; Use this instead.
	or	r31,r31,lo16(_u+USIZE)	
ONSUP:
	sub	r31,r31,EXPFRAME 	; Create exception frame.

	st	r1,r31,4		; save return address to caller
	st	r2,r31,HOMESP+8	; 
	st	r10,r31,HOMESP+40	; Save r10 for work register.
	ldcr	r10,cr2			; Get PSR and mask
	or.u	r2,r0,hi16(_intlvl)
	ld	r2,r2,lo16(_intlvl)
	mak	r2,r2,3<PS_MSK>
	or	r10,r10,r2
;
;   This subroutine saves all registers on the stack.
;
;	ldcr	r2,cr8			; CPU BUG
	ldcr	r2,cr20	
	bb0.n	0,r2,noflts
	st	r10,r31,HOMESP+PS	; Save psr on it.
	set	r10,r10,1<PS_DEXC>
	st	r10,r31,HOMESP+PS	; Save psr on it.

;  Control comes here when a data fault occurs.
;  We need to save the fault information on the stack.
	ldcr	r2,cr3			; Get ssb.
	ldcr	r10,cr20		; Pass bus error information also.
;	bb0.n	0,r10,fltregs2		; TRAN 0 is always valid on faults.
	st	r10,r31,HOMESP+TRAN1
	bb1	1,r10,nold1		; If not a load, don't clear SSB bit.
	extu	r10,r10,5<7>		; Get reg number and
	or	r10,r10,0x20		; set to clear 1 bit in
	set	r10,r0,r10		; get bit to clear
	and.c	r2,r2,r10		; ssb.
nold1:
	ldcr	r10,cr9			; Pass bus error information also.
	st	r10,r31,HOMESP+DATA1
	ldcr	r10,cr10		; Pass bus error information also.
	st	r10,r31,HOMESP+ADDR1

fltregs2:
	ldcr	r10,cr11		; Pass bus error information also.
	st	r10,r31,HOMESP+TRAN2
	bb0	0,r10,fltregs3		; If not valid, don't save
	bb1.n	1,r10,nold2		; If not a load, don't clear SSB bit.
	extu	r10,r10,5<7>		; Get reg number and
	or	r10,r10,0x20		; set to clear 1 bit in
	set	r10,r0,r10		; get bit to clear
	and.c	r2,r2,r10		; ssb.
nold2:
	ldcr	r10,cr12		; Pass bus error information also.
	st	r10,r31,HOMESP+DATA2
	ldcr	r10,cr13		; Pass bus error information also.
	st	r10,r31,HOMESP+ADDR2

fltregs3:
	ldcr	r10,cr14		; Pass bus error information also.
	st	r10,r31,HOMESP+TRAN3
	bb0	0,r10,setssb		; If not valid, don't save
	bb1	1,r10,nold3		; If not a load, don't clear SSB bit.
	bb0.n	TRN_DOUB,r10,nold3d
	extu	r10,r10,5<7>		; Get reg number and
	or	r10,r10,0x40		; set to clear 1 bit in
	br	clr3
nold3d:
	or	r10,r10,0x20		; set to clear 1 bit in
clr3:
	set	r10,r0,r10		; get bit to clear
	and.c	r2,r2,r10		; ssb.
nold3:
	ldcr	r10,cr15		; Pass bus error information also.
	st	r10,r31,HOMESP+DATA3
	ldcr	r10,cr16		; Pass bus error information also.
	st	r10,r31,HOMESP+ADDR3
setssb:
	stcr	r2,cr3

noflts:
	ldcr	r10,cr18			; Save r1
	st	r10,r31,HOMESP+4
	ldcr	r10,cr17			; Save r31
	st	r10,r31,HOMESP+SP
	ldcr	r10,cr4			; Save cip.
	st	r10,r31,HOMESP+SCIP
	ldcr	r10,cr5			; Save nip.
	st	r10,r31,HOMESP+SNIP
	ldcr	r10,cr6			; Save fip.
	st	r10,r31,HOMESP+SFIP
	ldcr	r10,cr3			; Save ssb (needed?)
	st	r10,r31,HOMESP+SSB
;
	bcnd	eq0,r10,fpdone		; Everything up to date?
; 
;  FP unit has unfinished operations.  Must be flushed and
;  reenabled for kernel use.
flush:
	or.u	r10,r0,hi16(HIPRI)
	or	r10,r10,lo16(HIPRI)
	stcr	r10,cr2		; Supervisor mode, FP unit on.
;
	or.u	r10,r0,hi16(fpflush)
	or	r10,r10,lo16(fpflush)
	set	r10,r10,1<VALIDIP>
	stcr	r10,cr6		; execute at fpflush.
	stcr	r0,cr5
	ld	r1,r31,HOMESP+4
	ld	r2,r31,HOMESP+8
	ld	r10,r31,HOMESP+40
	or	r0,r0,r0
	rte			; rte to next instruction.

;  Assumes that r1 and r31 are not destinations of fp instructions.
fpflush:
	tb1	0,r0,128	; let fp unit finish.
	stcr	r0,cr3
	st	r10,r31,HOMESP+40   ; Resave in case fp unit diddled
	st	r2,r31,HOMESP+8	    ; r2 or r10.
	st	r1,r31,HOMESP+4
	ld	r1,r31,4
fpdone:
;
;  Shadow registers have been saved, reenable interrupts.
;
	ldcr	r10,cr1			; Reenable interrupts.
	;clr	r10,r10,4<PS_EXP> 	; and reenable shadowing and FP.
	and	r10,r10,0xFFF6		; enable shadowing first
	stcr	r10,cr1
	and	r10,r10,0xFFF0		; now interrupts and FP
	stcr	r10,cr1

;Save all user registers.	
	st	r0,r31,HOMESP+0
	st	r3,r31,HOMESP+12	; Save r3
	st.d	r4,r31,HOMESP+16	; Save r4
;	st	r5,r31,HOMESP+20	; Save r5
	st.d	r6,r31,HOMESP+24	; Save r6
;	st	r7,r31,HOMESP+28	; Save r7
	st.d	r8,r31,HOMESP+32	; Save r8
;	st	r9,r31,HOMESP+36	; Save r9
	st	r11,r31,HOMESP+44	; Save r11
	st.d	r12,r31,HOMESP+48	; Save r12
;	st	r13,r31,HOMESP+52	; Save r13
	st.d	r14,r31,HOMESP+56	; Save r14
;	st	r15,r31,HOMESP+60	; Save r15
	st.d	r16,r31,HOMESP+64	; Save r16
;	st	r17,r31,HOMESP+68	; Save r17
	st.d	r18,r31,HOMESP+72	; Save r18
;	st	r19,r31,HOMESP+76	; Save r19
	st.d	r20,r31,HOMESP+80	; Save r20
;	st	r21,r31,HOMESP+84	; Save r21
	st.d	r22,r31,HOMESP+88	; Save r22
;	st	r23,r31,HOMESP+92	; Save r23
	st.d	r24,r31,HOMESP+96	; Save r24
;	st	r25,r31,HOMESP+100	; Save r25
	st.d	r26,r31,HOMESP+104	; Save r26
;	st	r27,r31,HOMESP+108	; Save r27
	st.d	r28,r31,HOMESP+112	; Save r28
;	st	r29,r31,HOMESP+116	; Save r29
	st	r30,r31,HOMESP+120	; Save r30

;  Load the linker registers with kernel pointers (previously saved)
;	or.u	r3,r0,hi16(_linkregs)
;	or	r3,r3,lo16(_linkregs)
;	ld	r26,r3,0
;	ld	r27,r3,4
;	ld	r28,r3,8
;	ld	r29,r3,0xc

;  Any FP exceptions needing processing
	or.u	r3,r0,hi16(_u+u_sfu1flag)	; FP handlers set this.
	ld	r3,r3,lo16(_u+u_sfu1flag)
	bcnd	eq0,r3,nofpes
	br	_fpehndlr		; Do any applicable default fp handling.
_fpertn:
	or.u	r2,r0,hi16(_u+u_sfu1flag)
	ld	r3,r2,lo16(_u+u_sfu1flag)	; SFU1PR or SFU1IMP
	bcnd	eq0,r3,nofpes
	st	r0,r2,lo16(_u+u_sfu1flag)

	st	r1,r31,HOMESP-4		; Save return add.
	add	r2,r31,HOMESP
	bsr.n	_trap			; Send signals on FP exceptions.
	sub	r31,r31,8

	add	r31,r31,8
	ld	r1,r31,HOMESP-4

nofpes:
;  Check for code or data faults needing handling.
	ld	r3,r31,HOMESP+SCIP
	bb1	EXCEPT_IP,r3,codeflt
	ld	r2,r31,HOMESP+PS
	bb0	PS_DEXC,r2,getmspsnoflt  ; Check for faults.

codeflt:
;   Code or Data faults are pending, handle the bus error.
;   buserr() fixes the register images on the stack.
	st	r1,r31,HOMESP-4		; Save return add.
	add	r2,r31,HOMESP
	bsr.n	_buserr			; process faults.
	sub	r31,r31,8

	add	r31,r31,8
	ld	r1,r31,HOMESP-4
	bcnd	eq0,r2,getnewregs	; Paged in?
;
;	ld	r2,r0,_saveflag		; Do we have to save fault info?
	or.u	r2,r0,hi16(_saveflag)	; Do we have to save fault info?
	ld	r2,r2,lo16(_saveflag)
	ld.b	r3,r2,0
	bcnd	eq0,r3,getnewregs
	st.b	r0,r2,0

;	ld	r2,r0,_saveaddr		; Get address of save area.
	or.u	r2,r0,hi16(_saveaddr)	; Get address of save area.
	ld	r2,r2,lo16(_saveaddr)
	add	r3,r31,HOMESP+TRAN1
	add	r4,r0,3*3
saveberr:
	sub	r4,r4,1			; Save fault information.
	ld	r5,r3[r4]		; 
	st	r5,r2[r4]
	bcnd	ne0,r4,saveberr

getnewregs:
; Reload parameter registers - they may have been destroyed.
	ld.d	r4,r31,HOMESP+16	; Restore r4
;	ld	r5,r31,HOMESP+20	; Restore r5
	ld.d	r6,r31,HOMESP+24	; Restore r6
;	ld	r7,r31,HOMESP+28	; Restore r7

getmspsnoflt:
	ld.d	r2,r31,HOMESP+8		; Restore r2.
;	ld	r3,r31,HOMESP+12	; Restore r3
	jmp	r1
;
;#############################################################
	global	getisps
getisps:
;
;    Switch to the interrupt stack and save registers on it.
;  Interrupts during user mode are handled on the master stack to
;  avoid copying registers from ISP to MSP on context switch.  All
;  other interrupts are handled on the interrupt stack.
;
;  Assumes:
;	r1 is saved in cr18
;	r31 is saved in cr17
;
	ldcr	r31,cr2
	bb1.n	PS_SUP,r31,notusp		; 
	ldcr	r31,cr17
	or.u	r31,r0,hi16(_u+USIZE)	; Use this instead.
	or	r31,r31,lo16(_u+USIZE)	
	br.n	notmsp
	sub	r31,r31,INTFRAME
notusp:
	bb0.n	MSP,r31,Xclk1		; 
	sub	r31,r31,INTFRAME	; Make space.
	ldcr	r31,cr19		; Now on interrupt stack.
	sub	r31,r31,INTFRAME	; Make space.
Xclk1:
notmsp:

	st	r1,r31,4		; save return address to caller
	st.d	r2,r31,HOMESP+8  	; Save r2
;	st	r3,r31,HOMESP+12  	; Save r3
	st	r0,r31,HOMESP+0
	ldcr	r2,cr2			; Get PSR and mask
	or.u	r3,r0,hi16(_intlvl)
	ld	r3,r3,lo16(_intlvl)
	mak	r3,r3,3<PS_MSK>
	or	r2,r2,r3
	st	r2,r31,HOMESP+PS	; Save the PSR.
;
; Save shadow registers.
	ldcr	r3,cr18			; Save r1
	st	r3,r31,HOMESP+4
	ldcr	r3,cr17			; Save r31
	st	r3,r31,HOMESP+SP
	ldcr	r3,cr4
	st	r3,r31,HOMESP+SCIP
	ldcr	r3,cr5			; Save nip.
	st	r3,r31,HOMESP+SNIP
	ldcr	r3,cr6			; Save fip.
	st	r3,r31,HOMESP+SFIP
	ldcr	r3,cr3			; Save ssb (needed?)
	st	r3,r31,HOMESP+SSB
	bcnd	eq0,r3,noflush1		; Any fp instructions in progress?
; 
;  FP unit has unfinished operations.  Must be flushed and
;  reenabled for kernel use.
	or.u	r3,r0,hi16(HIPRI)
	or	r3,r3,lo16(HIPRI)
	stcr	r3,cr2		; Supervisor mode, FP unit on.
;
	or.u	r3,r0,hi16(fpflush1)
	or	r3,r3,lo16(fpflush1)
	set	r3,r3,1<VALIDIP>
	stcr	r3,cr6		; execute at fpflush1.
	stcr	r0,cr5
	ld	r1,r31,HOMESP+4
	ld	r2,r31,HOMESP+8
	ld	r3,r31,HOMESP+12
	or	r0,r0,r0
	rte			; rte to next instruction.
;  Assumes that r1 and r31 are not destinations of fp instructions.
fpflush1:
	tb1	0,r0,128	; let fp unit finish.
	st	r3,r31,HOMESP+12   ; Resave in case fp unit diddled
	st	r2,r31,HOMESP+8	    ; r2 or r3.
	st	r1,r31,HOMESP+4
	ld	r1,r31,4
noflush1:
;Save all user registers.	
;
;  Reenable exceptions and fp unit only.
;
	ldcr	r3,cr1			; Reenable exceptions
	;clr	r3,r3,1<PS_EXP>
	;clr	r3,r3,1<PS_SFD1>	; Reenable fp unit.
	and	r3,r3,0xFFFE		; Reenable shadowing first
	stcr	r3,cr1
	and	r3,r3,0xFFF6		; Now reenable FP
	stcr	r3,cr1

	st.d	r4,r31,HOMESP+16	; Save r4
;	st	r5,r31,HOMESP+20	; Save r5
	st.d	r6,r31,HOMESP+24	; Save r6
;	st	r7,r31,HOMESP+28	; Save r7
	st.d	r8,r31,HOMESP+32	; Save r8
;	st	r9,r31,HOMESP+36	; Save r9
	st.d	r10,r31,HOMESP+40	; Save r10
;	st	r11,r31,HOMESP+44	; Save r11
	st.d	r12,r31,HOMESP+48	; Save r12
;	st	r13,r31,HOMESP+52	; Save r13
	st.d	r14,r31,HOMESP+56	; Save r14
;	st	r15,r31,HOMESP+60	; Save r15
	st.d	r16,r31,HOMESP+64	; Save r16
;	st	r17,r31,HOMESP+68	; Save r17
	st.d	r18,r31,HOMESP+72	; Save r18
;	st	r19,r31,HOMESP+76	; Save r19
	st.d	r20,r31,HOMESP+80	; Save r20
;	st	r21,r31,HOMESP+84	; Save r21
	st.d	r22,r31,HOMESP+88	; Save r22
;	st	r23,r31,HOMESP+92	; Save r23
	st.d	r24,r31,HOMESP+96	; Save r24
;	st	r25,r31,HOMESP+100	; Save r25
	st.d	r26,r31,HOMESP+104	; Save r26
;	st	r27,r31,HOMESP+108	; Save r27
	st.d	r28,r31,HOMESP+112	; Save r28
;	st	r29,r31,HOMESP+116	; Save r29

;	or.u	r3,r0,hi16(_linkregs)
;	or	r3,r3,lo16(_linkregs)
;	ld	r26,r3,0
;	ld	r27,r3,4
;	ld	r28,r3,8
;	ld	r29,r3,0xc

	st	r30,r31,HOMESP+120	; Save r30
	jmp	r1
;###############################################################
	global	fgetregs
fgetregs:
	global	getregs
getregs:
;   This subroutine gets all user registers from the stack.
;   Then rte (no return)
;
;   Reload all user registers.
	ld.d	r6,r31,HOMESP+24	; Get r6
;	ld	r7,r31,HOMESP+28	; Get r7
	ld.d	r8,r31,HOMESP+32	; Get r8
;	ld	r9,r31,HOMESP+36	; Get r9
	ld.d	r10,r31,HOMESP+40	; Get r10
;	ld	r11,r31,HOMESP+44	; Get r11
	ld.d	r12,r31,HOMESP+48	; Get r12
;	ld	r13,r31,HOMESP+52	; Get r13
	ld.d	r14,r31,HOMESP+56	; Get r14
;	ld	r15,r31,HOMESP+60	; Get r15
	ld.d	r16,r31,HOMESP+64	; Get r16
;	ld	r17,r31,HOMESP+68	; Get r17
	ld.d	r18,r31,HOMESP+72	; Get r18
;	ld	r19,r31,HOMESP+76	; Get r19
	ld.d	r20,r31,HOMESP+80	; Get r20
;	ld	r21,r31,HOMESP+84	; Get r21
	ld.d	r22,r31,HOMESP+88	; Get r22
;	ld	r23,r31,HOMESP+92	; Get r23
	ld.d	r24,r31,HOMESP+96	; Get r24
;	ld	r25,r31,HOMESP+100	; Get r25
	ld.d	r26,r31,HOMESP+104	; Get r26
;	ld	r27,r31,HOMESP+108	; Get r27
	ld.d	r28,r31,HOMESP+112	; Get r28
;	ld	r29,r31,HOMESP+116	; Get r29
	ld	r30,r31,HOMESP+120	; Get r30
;
	ldcr	r2,cr1			; Inhibit interrupts.
	set	r2,r2,1<1>		; While loading shadows. 2 insts needed.
	stcr	r2,cr1
	set	r2,r2,1<0>		; While loading shadows.
	stcr	r2,cr1
;
;  Reload all shadow registers.
	ld	r2,r31,HOMESP+SPSR
	stcr	r2,cr2			; Get psr.
	extu	r2,r2,3<PS_MSK>		; Get restored level
	bsr	_spli			; and restore it.
	ld	r3,r31,HOMESP+SFIP
	ld	r2,r31,HOMESP+SNIP
	subu	r4,r3,r2
	subu	r4,r4,4			; r4 will be zero for sequential NIP,FIP
	bcnd	eq0,r4,donewrte
	bb0	VALIDIP,r2,nofix
	subu	r2,r2,4

donewrte:
	add	r3,r2,r0
nofix:
	stcr	r3,cr6			; Get fip.
	and	r2,r2,0xFFFC		; Make invalid.
	stcr	r2,cr5			; Get nip.
	stcr	r0,cr3			; Get ssb - should be made a zero here.

	ld	r1,r31,HOMESP+4		; Get r1
	ld.d	r2,r31,HOMESP+8		; Get r2
;	ld	r3,r31,HOMESP+12	; Get r3
	ld.d	r4,r31,HOMESP+16	; Get r4
;	ld	r5,r31,HOMESP+20	; Get r5
	ld	r31,r31,HOMESP+SP	; Get r31
;
	or	r0,r0,r0
	rte

getrs:
;   This subroutine gets all user registers from the stack.
;
;   Reload all user registers.
;	ld	r0,r31,HOMESP+0	; Get r0  (dont have to)
	ld	r6,r31,HOMESP+24	; Get r6
	ld	r7,r31,HOMESP+28	; Get r7
	ld	r8,r31,HOMESP+32	; Get r8
	ld	r9,r31,HOMESP+36	; Get r9
	ld	r10,r31,HOMESP+40	; Get r10
	ld	r11,r31,HOMESP+44	; Get r11
	ld	r12,r31,HOMESP+48	; Get r12
	ld	r13,r31,HOMESP+52	; Get r13
	ld	r14,r31,HOMESP+56	; Get r14
	ld	r15,r31,HOMESP+60	; Get r15
	ld	r16,r31,HOMESP+64	; Get r16
	ld	r17,r31,HOMESP+68	; Get r17
	ld	r18,r31,HOMESP+72	; Get r18
	ld	r19,r31,HOMESP+76	; Get r19
	ld	r20,r31,HOMESP+80	; Get r20
	ld	r21,r31,HOMESP+84	; Get r21
	ld	r22,r31,HOMESP+88	; Get r22
	ld	r23,r31,HOMESP+92	; Get r23
	ld	r24,r31,HOMESP+96	; Get r24
	ld	r25,r31,HOMESP+100	; Get r25
	ld	r26,r31,HOMESP+104	; Get r26
	ld	r27,r31,HOMESP+108	; Get r27
	ld	r28,r31,HOMESP+112	; Get r28
	ld	r29,r31,HOMESP+116	; Get r29
	ld	r30,r31,HOMESP+120	; Get r30
;
	ldcr	r2,cr1		; Inhibit interrupts and exceptions
	set	r2,r2,1<1>		; While loading shadows. 2 insts needed.
	stcr	r2,cr1
	set	r2,r2,1<0>		; While loading shadows.
	stcr	r2,cr1
;
;  Reload all shadow registers.
	ld	r2,r31,HOMESP+4	; Get r1
	stcr	r2,cr18
	ld	r2,r31,HOMESP+SPSR
	stcr	r2,cr2		; Get psr.
	extu	r2,r2,3<PS_MSK>		; Get restored level
	st	r1,r31,0
	bsr	_spli			; and restore it.
	ld	r1,r31,0
	ld	r2,r31,HOMESP+SNIP
	stcr	r2,cr5		; Get nip.
	ld	r2,r31,HOMESP+SFIP
	stcr	r2,cr6		; Get fip.
	ld	r2,r31,HOMESP+SSB
	stcr	r2,cr3		; Get ssb.
;
	ld	r2,r31,HOMESP+8	; Get r2
	ld	r3,r31,HOMESP+12	; Get r3
	ld	r4,r31,HOMESP+16	; Get r4
	ld	r5,r31,HOMESP+20	; Get r5
	ld	r31,r31,HOMESP+SP	; Get r31
;
	jmp	r1

	data
	global	_idleflag
_idleflag:
	word	0
	global	_intlvl
_intlvl:
	word	7

	text
	global	_swapl
	global	_swaps
	global	_swaplp
	global	_swapsp
	global	_swapla
	global	_swapsa

; swap long value
_swapl:
	extu	r5,r2,8<24>		; byte 3 -> byte 0
	extu	r4,r2,8<16>		; byte 2 -> byte 0
	extu	r3,r2,8<8>		; byte 1 -> byte 0
	mak	r2,r2,8<24>		; byte 0 -> byte 3
	mak	r3,r3,8<16>		; byte 0 -> byte 2
	mak	r4,r4,8<8>		; byte 0 -> byte 1
	or	r4,r4,r5		; or them all in 
	or	r3,r3,r4
	jmp.n	r1
	or	r2,r2,r3

; swap long pointer
_swaplp:
	addu	r6,r0,r2		; save pointer
	ld	r2,r6,0			; get long
	extu	r5,r2,8<24>		; swap it
	extu	r4,r2,8<16>
	extu	r3,r2,8<8>
	mak	r2,r2,8<24>
	mak	r3,r3,8<16>
	mak	r4,r4,8<8>
	or	r4,r4,r5
	or	r3,r3,r4
	or	r2,r2,r3
	jmp.n	r1
	st	r2,r6,0			; store long

; swap long array
_swapla:
	bcnd.n	eq0,r3,laout		; return on zero count
	addu	r6,r0,r2		; r6 long pointer
	addu	r7,r0,r3		; r7 count
laloop:
	ld	r2,r6,0			; get long
	extu	r5,r2,8<24>		; swap it
	extu	r4,r2,8<16>
	extu	r3,r2,8<8>
	mak	r2,r2,8<24>
	mak	r3,r3,8<16>
	mak	r4,r4,8<8>
	or	r4,r4,r5
	or	r3,r3,r4
	or	r2,r2,r3
	st	r2,r6,0			; store long
	sub	r7,r7,1			; decrement count
	bcnd.n	ne0,r7,laloop
	addu	r6,r6,4			; bump pointer
laout:
	jmp.n	r1
	addu	r2,r6,r0		; addr of next long not modified

; swap short value
_swaps:
	extu	r3,r2,8<8>		; byte 1 -> byte 0
	mak	r2,r2,8<8>		; byte 0 -> byte 1
	jmp.n	r1
	or	r2,r2,r3		; or them in

; swap short pointer
_swapsp:
	addu	r6,r0,r2		; save pointer
	ld.hu	r2,r6,0			; get short
	extu	r3,r2,8<8>		; swap it
	mak	r2,r2,8<8>
	or	r2,r2,r3		; or them in
	jmp.n	r1
	st.h	r2,r6,0			; store short

; swap short array
_swapsa:
	bcnd.n	eq0,r3,saout		; return on zero count
	addu	r6,r0,r2		; r6 short pointer
	addu	r7,r0,r3		; r7 count
saloop:
	ld.hu	r2,r6,0			; get short
	extu	r3,r2,8<8>		; swap it
	mak	r2,r2,8<8>
	or	r2,r2,r3
	st.h	r2,r6,0			; store short
	sub	r7,r7,1			; decrement count
	bcnd.n	ne0,r7,saloop
	addu	r6,r6,2			; bump pointer
saout:
	jmp.n	r1
	addu	r2,r6,r0		; addr of next short not modified

	global	bcssyscall
bcssyscall:
	stcr	r1,cr18
	bsr.n	getmsps		; Switch stacks and save registers
	stcr	r31,cr17

	ld	r9,r31,HOMESP+36	; r9 has new syscall number
	or.u	r1,r0,hi16(_bcstable)
	or	r1,r1,lo16(_bcstable)
	mask	r9,r9,0xff
	ld.bu	r9,r1,r9	; new to old system call number
	or	r9,r9,256	; bcs system call flag
	bsr.n	_syscall
	add	r8,r31,HOMESP

	br	getregs
	global	bcssigret
bcssigret:
	stcr	r1,cr18
	bsr.n	getmsps		; Switch stacks and save registers
	stcr	r31,cr17

	addu	r9,r0,256+108		; r9 has sigret

	bsr.n	_syscall
	add	r8,r31,HOMESP

	br	getregs
	global	bcssys_local
bcssys_local:
	stcr	r1,cr18
	bsr.n	getmsps		; Switch stacks and save registers
	stcr	r31,cr17

	addu	r9,r0,256+110		; r9 has syslocal

	bsr.n	_syscall
	add	r8,r31,HOMESP

	br	getregs
;	######################
	global	_getcr0
_getcr0:
	jmp.n	r1
	ldcr	r2,cr0

	global	_setcr62
_setcr62:
	fstcr	r2,cr62
	jmp	r1

	global	_setcr63
_setcr63:
	fstcr	r2,cr63
	jmp	r1

	global	_getcr62
_getcr62:
	fldcr	r2,cr62
	jmp	r1

	global	_getcr63
_getcr63:
	fldcr	r2,cr63
	jmp	r1
E 1

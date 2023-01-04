;		Copyright (c) 1985 AT&T		#
;		All Rights Reserved		#
;	@(#)psl.m	1.0	
;
;	processor status register for 78000
;
;  Bit numbers of interest in the psr.
;
	def	PS_EXP,0	; exception disable bit.
	def	PS_IPL,1	; interrupt priority level 
	def	INTMASK,1	; Mask bit for interrupts.
	def	PS_MXM,2	; Set to disable misaligned exceptions.
	def	PS_SFD1,3	; Disable floating point unit.
	def	PS_MSK,10	; Interrupt mask - software only.
	def	PS_DEXC,27	; Data exception pending.
	def	PS_C,28		; carry bit 
	def	PS_SER,29	; Set serialization mode
	def	PS_CUR,31	; current mode. Supervisor if set 
	def	PS_SUP,31	; current mode. Supervisor if set 
	def	PS_USR,31	; current mode. Supervisor if set 
;
;  Bit numbers of interest in the Data unit transaction registers.
;
	def	TRN_REG,7
	def	TRN_LD,1
	def	TRN_DOUB,13
;		Copyright (c) 1985 AT&T		#
;		All Rights Reserved		#
;	@(#)equates.m	1.0	

;  General equate file.
;  This file contains equates which are used throughout the machine
; language routines.  Any equate of this nature should be placed in
; this file.  Equates that are specific to a single machine language
; source file should be defined in the source file that contains them.

def	u,0x10000000		; Address of U-block  (ASPEN specific).
def	MSP,28			; Unique bit in master stack pointer (ASPEN)
def	_u,0x10000000		; Address of U-block
	global u		; Define the U-block for the entire system
	global _u		; Define the U-block for the entire system
def	USIZE,0x2000

def	HIPRI,0x80000002

#ifdef ASPEN
; 78000 CPU board layout  (ASPEN specific equates.)
;
;  Cache control registers.
;
def	FSCC,0x10000010		; Flush supervisor code space.
def	FSDC,0x10000014		; Flush supervisor data space.
def	FUCC,0x10000018		; Flush user code space.
def	FUDC,0x1000001c		; Flush user data space.
;
; CPU control/status registers.
;
def	VMEINTR, 0x10000027	; VME bus interrupt vector.
def	DUARTACK,0x1000002B	; Duart interrupt acknowledge.
def	ASR,0x10000033		; VME address space modifier (BYTE)

def	SSR,0x10000030		; CPU status register.
def	NMI,15			; NMI button is pushed.
def	VME,17			; VME interrupt occurred.
def	DUINT,18		; Interrupt from DUART.

def	CSR,0x10000030		; CPU control register.
def	CACHEON,0x100		; Enable caching.
def	FLTENB,0x200		; Enable bus errors.
def	VMEINT,0x1000		; Enable VME interrupts.
def	CLKINT,0x8		; No clock disable for now.
def	CONINT,0xFF-8
def	MMUBASE,0x10000038	; MMC base address register.
def	MMUCNTL,0x1000003c	; MMC translation control register.
def	ASYNC,0x10000040	; DUART registers.
def	ISR,20			; Interrupt status register.
def	IMR,20			; Interrupt status register.
def	RDCT,60			; Reset timer interrupt.
def	ROM,0x20000000		; Base address of ROM.
; End of ASPEN specific equates.
#endif

#ifdef CMMU
; 88000 CPU board layout  (ANGELSFIRE & opus)
;
;  Cache control registers.
;
def	SARP,		0x200		; offset to sys root pointer reg
def	UARP,		0x204		; offset to user root pointer reg

def	SCMID,		0x0		; CMMU ID register
def	SCMR,		0x4		; system command register
def	SSR,		0x8		; system status register
def	SADR,		0xc		; system address register
def	SCTR,		0x104		; system control register
def	CSSP,		0x880		; Cache Set Status Port
;	Command register equates
#ifdef SIMULATOR
def	INVUATC,	0x33		; Invalidate all user ATC
def	INVSATC,	0x37		; Invalidate all supv ATC
#else
def	INVUATC,	0x3b		; Invalidate all user ATC
def	INVSATC,	0x3f		; Invalidate all supv ATC
#endif
def	WINALL,		0x1f		; Write/invalidate all pages
def	INVALL,		0x17		; Invalidate all pages
def	WRTALL,		0x1b		; Write all pages
;
def	INVPG,		0x15		; Invalidate one page
def	WRTPG,		0x19		; Write one page
def	WINPG,		0x1d		; Write/invalidate one page

;	Commands to cmmu_store
def	STOCMR,		0		; store commands register
def	STOURP,		4		; store user root pointer
def	STOSRP,		8		; store supv. root pointer

;	used when enabling or disabling all cache
def	CACHEINH,	6		; bit number to inhibit cache
#endif /* CMMU */

#if ANGEL
;
; CPU control/status registers.
;
def	SSR,	0xffe10000	; CPU status register.
def	NMI,	7		; NMI button is pushed.
def	DUINT,	6		; Interupt from DUART.
def	SYSFAIL, 5		; Interupt from VME sysfail
def	VME,	4		; VME interrupt occurred.

def	CSR,	0xffe20000	; CPU control register.
def	RAMEN,	0x04		; Enable VMEBUS RAM
def	VMESYSF, 0x08		; Drive  VME sysfail line on VME bus
def	ENVME,	0x10		; Enable VME interupts.
def	ENSYSF,	0x20		; Enable VME sysfail interupt
def	ENASIO,	0x40		; Enable clock/DUART interupts
def	ENABORT, 0x80		; Enable Abort button interupt
def	HILEVMSK, RAMEN+ENABORT
def	LEV4MSK, HILEVMSK+ENASIO
def	LEV0MSK, LEV4MSK+ENVME

def	VMEINTR, 0xffe80000	; read VME bus interupt vector.
def	CLRAB,	0xffee0000	; read to clear abort interupt
def	CLRSF,	0xffea0000	; read to clear sysfail interupt

def	ASYNC,	0xffe40000	; DUART registers.
def	ISR,	0x14		; Interrupt status register.
def	IMR,	0x14		; Interrupt status register.
def	RDCT,	0x3c		; Reset timer interrupt, stop counter cmd

def	ROM,	0xff800000	; Base address of ROM.
; End of ANGELSFIRE specific equates.
#endif /* ANGEL */

; Exception frames equates.
; Note - stack alignment should be on 8 byte boundary.
def	HOMESP,32
def	EXPFRAME,32*4+4+4+4+4+4+4+4+4+4+4+4+4+4+4+HOMESP+8
;def	INTFRAME,32*4+4+4+4+4+4+4+HOMESP+8
def	INTFRAME,32*4+4+4+4+4+4+4+4+4+4+4+4+4+4+4+HOMESP+8
def	SP,31*4
def	SPSR,32*4
def	PS,32*4
def	SCIP,33*4
def	SNIP,34*4
def	SFIP,35*4
def	SSB,36*4
def	INTF,37*4
def	INTLVL,38*4
def	TRAN1,37*4
def	ADDR1,38*4
def	DATA1,39*4
def	TRAN2,40*4
def	ADDR2,41*4
def	DATA2,42*4
def	TRAN3,43*4
def	ADDR3,44*4
def	DATA3,45*4
def	EXCF,46*4
def	EXCNUM,47*4

def	VALIDIP,1
def	EXCEPT_IP,0


;
;  Vector number equates.

def	INTERRUPT,1	;  Interrupt vector.
def	CACCFLT,2	;  Code Bus error			
def	DACCFLT,3	;  Data Bus error			
def	ADDRERR,4	;  Address error		
def	INSTERR,5	;  Illegal instruction	
def	PRIVFLT,6	;  priviledged instruction
def	CHKTRAP,7	;  tbnd instruction
def	ZDVDERR,8	;  Divide by zero
def	INTOVF,9	;  Integer overflow.
def	SFU1PR,114	;  SFU #1 precise exception
def	SFU1IMP,115	;  SFU #1 imprecise exception
def	SYSCALL,128	;  0 system call trap
def	BKBPT,129	;  Breakpoint for tracing.
def	IOTTRAP,131	;  Breakpoint
def	RESCHED,512	;  pseudo trap to signal cpu
; vectors for ROM.
;  ** These are subject to change as ROM becomes defined **
					;  rescheduling.
; Miscellaneous equates.
;
def	BTOL,2			; Number of shifts for bytes to longs.

;  Ublock equates.  The following may need changing when the user.h
; file is changed.

def	u_procp,0x294
def	u_caddrflt,0xc0
def	u_prof,0x324
def	u_sfu1flag,0x400
def	u_sfu1data,0x404



;  Proc equates.  The following may need changing when proc.h changes.
;

;def	p_sysid,0x76

#ifdef opus
;	opus h/w register addresses 
	def	SR0,0x5fffff20
	def	SR1,0x5fffff24
	def	SR2,0x5fffff28
	def	SR3,0x5fffff2c
	def	CR0,0x5fffff30
;	CR0 command values
	def	CP_GE,0x80
	def	CLR_CP,0x4
	def	CLR_AT,0x6
	def	ENB_AT,0x5
	def	ENB_CPI,0xe
	def	DSB_CPI,0xf
	def	INT_CP,0x10
	def	F_AT,0x16
	def	SET,0x1
	def	CLR,0x0
;	COMMPAGE addresses
	def	COMMPAGE,0xD0000000
	def	COMMPAGE0,0x0
	def	PC_CKCNT,0x1C
	def	PC_CKFLG,0x24
	def	PC_RQMSK,0x30
	def	PC_CPGET,0x32
	def	PC_CPPUT,0x34
	def	PC_CPMSK,0x36
	def	PC_MSIZE,0x38
	def	PC_CPTYP,0xbc
	def	PC_CFGRG, 0xc0
	def	NMI_FLAG,0x27
	def	SPLRET,0xf0
	def	SPL,0xf4
	def	PANICBASE,0x100
;	PCCMD handshaking flag values
	def	S_IDLE,0
	def	S_GO,1
	def	S_BUSY,2
	def	S_DONE,4
; 	Bit definitions of above
	def	B_S_GO,0
	def	B_S_BUSY,1
	def	B_S_DONE,2
#endif /* opus */
; jeff hopkins (512)-891-3170	motorola fpu guru
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
	or	r0,r0,r0		; DELAY
	ld.usr	r1,r1,r0	; Get faulting instruction.
	or	r0,r0,r0		; DELAY2
	or	r0,r0,r0		; DELAY2
	or	r0,r0,r0		; DELAY2
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
;		Copyright (c) 1985 AT&T		#
;		All Rights Reserved		#
;	@(#)mmb.s	6.10	
;
#ifdef	ASPEN
; mmbstore(reg, val)		/* store to mmu register */
; register char *reg;	
; register int val;	
;  For now, these routines are included here.  These eventually might
; be broken into macros or maybe go away entirely.
;  This is a ASPEN specific module.

	data
	global _mmbad
_mmbad:	word 0x10000038		; Address of MMC control register.
	text
	global	_mmbstore
_mmbstore:
	ldcr	r5,cr1
	set	r6,r5,1<PS_SER>
	stcr	r6,cr1
	ld	r4,r0,_mmbad
	st	r3,r4,r2
	stcr	r5,cr1
	jmp	r1

;
;  The following routine sets the global flag "do_pmmu" to indicate
; whether or not a PMMU was found.
_chk_mmu: global	_chk_mmu
; This routine should go away.
	st	r0,r0,_do_pmmu   ; Clear it always.
	jmp	r1
	data
	global	_do_pmmu
_do_pmmu:
	word	0
	text

	global	_wakemmu
_wakemmu:
	jmp	r1
#endif
#ifdef	CMMU
;
;	These routine are CMMU specific, they should probably be in new
;	file cmmu.s but are in here for ease in porting
;
	global	_ccacheflush
_ccacheflush:
	ldcr	r11,cr1			; Get psr and block
	set	r9,r11,1<PS_IPL>	; interrupts
	stcr	r9,cr1

	or.u	r13,r0,hi16(CCMMUBASE)
	bcnd.n	ne0,r2,@Lccpf
	ld	r13,r13,lo16(CCMMUBASE)

	br.n	@Lccfe
	or	r12,r0,INVALL		; invalidate all code cache
@Lccpf:
	and	r2,r2,0xf000		; clear page offset
	st	r2,r13,SADR		; write page number
	tb1	0,r0,128		; wait for instruction to finish
	or	r12,r0,INVPG		; invalidate page of code cache
@Lccfe:
	st	r12,r13,SCMR
	tb1	0,r0,128		; wait for instruction to finish
	add	r0,r0,r0		; MMU work around
	add	r0,r0,r0		; from motorola bug sheet 1.0 (6/7/88)

	ld	r2,r13,SSR		; wait for mmu completion
	tb1	0,r0,128		; wait for instruction to finish
	add	r0,r0,r0		; MMU work around
	add	r0,r0,r0		; from motorola bug sheet 1.0 (6/7/88)

	or	r13,r0,r0		; clear all traces of CMMU addr
					; because of chip bug
	stcr	r11,cr1			; restore save PSR
	jmp	r1

	global	_dcacheflush
_dcacheflush:
	ldcr	r11,cr1			; Get psr and block
	set	r9,r11,1<PS_IPL>	; interrupts
	stcr	r9,cr1

	or.u	r13,r0,hi16(DCMMUBASE)
	bcnd.n	ne0,r2,@Ldcpf
	ld	r13,r13,lo16(DCMMUBASE)

	br.n	@Ldcfe
	or	r12,r0,WINALL		; write and invalidate all data cache
@Ldcpf:
	and	r2,r2,0xf000		; clear page offset
	st	r2,r13,SADR		; write page number
	tb1	0,r0,128		; wait for instruction to finish
	or	r12,r0,WINPG		; writeback and inv page of data cache
@Ldcfe:
	st	r12,r13,SCMR
	tb1	0,r0,128		; wait for instruction to finish
	add	r0,r0,r0		; MMU work around
	add	r0,r0,r0		; from motorola bug sheet 1.0 (6/7/88)

	ld	r2,r13,SSR		; wait for mmu completion
	tb1	0,r0,128		; wait for instruction to finish
	add	r0,r0,r0		; MMU work around
	add	r0,r0,r0		; from motorola bug sheet 1.0 (6/7/88)

	bb1	14,r2,dcerr		; Bus Error on Flush

	or	r13,r0,r0		; clear all traces of CMMU addr
					; because of chip bug
	stcr	r11,cr1			; restore save PSR
	jmp	r1

dcerr:
	ld	r3,r13,SADR
	tb1	0,r0,128		; wait for instruction to finish
	br	_breakpoint

	global	_cmmuflush
_cmmuflush:
	ldcr	r11,cr1			; Get psr and block
	set	r9,r11,1<PS_IPL>	; interrupts
	stcr	r9,cr1

	or.u	r13,r0,hi16(CCMMUBASE)
	ld	r13,r13,lo16(CCMMUBASE)
	or.u	r8,r0,hi16(DCMMUBASE)
	ld	r8,r8,lo16(DCMMUBASE)

	or.u	r10,r0,hi16(cmmucmd)
	ld	r10,r10,lo16(cmmucmd)	; get command register bits
	or	r12,r10,INVUATC		; invalidate all users ATC
	st	r12,r13,SCMR		; flush code cmmu
	tb1	0,r0,128		; wait for instruction to finish
	add	r0,r0,r0		; MMU work around
	add	r0,r0,r0		; from motorola bug sheet 1.0 (6/7/88)
	st	r12,r8,SCMR		; flush data cmmu
	tb1	0,r0,128		; wait for instruction to finish
	add	r0,r0,r0		; MMU work around
	add	r0,r0,r0		; from motorola bug sheet 1.0 (6/7/88)

	or	r12,r10,INVSATC		; invalidate all supv ATC
	st	r12,r13,SCMR		; flush code cmmu
	tb1	0,r0,128		; wait for instruction to finish
	add	r0,r0,r0		; MMU work around
	add	r0,r0,r0		; from motorola bug sheet 1.0 (6/7/88)
	st	r12,r8,SCMR		; flush data cmmu
	tb1	0,r0,128		; wait for instruction to finish
	add	r0,r0,r0		; MMU work around
	add	r0,r0,r0		; from motorola bug sheet 1.0 (6/7/88)

	or	r13,r0,r0		; clear all traces of CMMU addr
	or	r8,r0,r0		; because of chip bug
	stcr	r11,cr1			; restore save PSR
	jmp	r1

	global	_cmmu_store
_cmmu_store:
	ldcr	r11,cr1			; Get psr and block
	set	r9,r11,1<PS_IPL>	; interrupts
	stcr	r9,cr1

	or.u	r13,r0,hi16(CCMMUBASE)
	ld	r13,r13,lo16(CCMMUBASE)
	or.u	r7,r0,hi16(DCMMUBASE)
	ld	r7,r7,lo16(DCMMUBASE)

	bcnd	ne0,r2,@Lnotcmr		; store command register?
	or.u	r12,r0,hi16(cmmucmd)
	ld	r8,r12,lo16(cmmucmd)	; get command register bits
	or	r3,r3,r8		; oring then to command
	st	r3,r13,SCMR
	tb1	0,r0,128		; wait for instruction to finish
	add	r0,r0,r0		; MMU work around
	add	r0,r0,r0		; from motorola bug sheet 1.0 (6/7/88)
	st	r3,r7,SCMR
	tb1	0,r0,128		; wait for instruction to finish
	add	r0,r0,r0		; MMU work around
	add	r0,r0,r0		; from motorola bug sheet 1.0 (6/7/88)
	br	@Lexit

@Lnotcmr:
	or	r3,r3,0x0001		; set valid bit for root pointers

	or.u	r12,r0,hi16(ccacheinh)
	ld	r8,r12,lo16(ccacheinh)	; get code cache inhibit bit

	sub	r10,r2,STOURP		; store user root pointer?
	bcnd	ne0,r10,@Lnotusr

	or.u	r12,r0,hi16(ucacheinh)
	ld	r9,r12,lo16(ucacheinh)	; get data cache inhibit bit

	or	r2,r3,r8		; or in code cache inhibit if set
	st	r2,r13,UARP		; store code side
	tb1	0,r0,128		; wait for instruction to finish
	or	r2,r3,r9		; or in data cache inhibit if set
	st	r2,r7,UARP		; store data cmmu
	tb1	0,r0,128		; wait for instruction to finish
	br	@Lexit

@Lnotusr:
	sub	r10,r2,STOSRP		; store supv root pointer?
	bcnd	ne0,r10,@Lnotsup

	or.u	r12,r0,hi16(dcacheinh)
	ld	r9,r12,lo16(dcacheinh)	; get data cache inhibit bit

	or	r2,r3,r8		; or in code cache inhibit if set
	st	r2,r13,SARP		; else store supv. root pointer
	tb1	0,r0,128		; wait for instruction to finish
	or	r2,r3,r9		; or in cache inhibit if set
	st	r2,r7,SARP		; store data cmmu
	tb1	0,r0,128		; wait for instruction to finish
	br	@Lexit

@Lnotsup:
	st	r3,r2,r0		; use r2 as cmmu address
	tb1	0,r0,128		; wait for instruction to finish

@Lexit:
	or	r13,r0,r0		; clear all traces of CMMU addr
	or	r7,r0,r0		; because of chip bug
	stcr	r11,cr1			; restore save PSR
	jmp	r1

#endif
; @(#)paging.s	1.2
;	Protection  fault
	global	Xcaccflt
Xcaccflt:
;
;  Execution comes here when a fault on code access occurs.
;
	stcr	r1,cr18	

	ldcr	r1,cr2
	bb0	31,r1,notkva00
	bsr	_triggerscope
notkva00:

	bsr.n	getmsps		; Switch to master stack and save regs.
	stcr	r31,cr17
;
;  getmsps handled the call to buserr() for us.
;  All we got to do is return.

	br	getregs		; Return to user process.
;
	global	Xdaccflt
Xdaccflt:
;
;  Control comes here when a bus error occurs.  This can be either due
; to an access of a faulted page or a write access of a write protected
; page.
;
;
	stcr	r1,cr18

	ldcr	r1,cr8
	bb0	14,r1,notkva01
	bsr	_triggerscope
notkva01:

	bsr.n	getmsps		; Switch to master stack and save regs.
	stcr	r31,cr17
;
;  getmsps handled the call to buserr() for us.
;  All we got to do is return.

	br	getregs		; Return to user process.



; oldval=xmem(addr,data, wordflag);
;     
;     addr= address to xmem
;     data= data to xmem
;     wordflag = 0 for byte, 1 for word.
;
;     returns oldval = original memory contents at addr.
;
;   This subroutine performs an xmem instruction for continuation of
; a faulted xmem access.
	global	_xmem
_xmem:
	bcnd	eq0,r4,xmembyte
	xmem.usr	r3,r2,r0	; Exchange a word.
	jmp.n	r1			; Return value.
	add	r2,r3,r0

xmembyte:
	xmem.bu.usr r3,r2,r0		; Exchange a byte.
	jmp.n	r1			; Return value.
	add	r2,r3,r0
;		Copyright (c) 1985 AT&T		#
;		All Rights Reserved		#
;	@(#)start.m4	6.8	

; MC78000 version
; The code in this file is executed at system bootstrap.
#ifdef	ASPEN
	def	GRAIN,1024		; (ASPEN specific)
#endif
#ifdef CMMU
	def	GRAIN,	0x1000		; CMMU fixed 4k page
#endif
	def	ROUND,GRAIN-1		; granularity round
	def	GMASK,-GRAIN		; granularity mask
	def	STSIZE,8192		; boot and interrupt stack for system
	def	U_SIZE,8192		; U block for Proc 0
	def	USRPSR,0		; userstate, interrupts enabled.
	def	SCRATCH,16*4		; Scratch space needed for temps.
;					  Scratch is deducted from the 
;					  interrupt stack space so don't
;					  make it too big.

	data	; these variables can't be in bss space
	global	physmem
physmem:	word	0		; total memory
vbr:		word	0		; contents of vector base register
	global	Dconcurrent
	global	_Dconcurrent
_Dconcurrent:
Dconcurrent:	word	0		; diagnostic concurrent mode enabled

bootdriv:	global bootdriv		; boot drive from %d0
		word 0
bootctlr:	global bootctlr		; boot controller from %d1
		word 0
bootinod:	global bootinod		; boot inode from %d2
		word 0
dumpdev:	word -1

	text
start:	global	start
_start:	global	_start
__start:	global	__start

	or.u	r26,r0,0		; special hole for linker synthesis
	or	r26,r26,0		; special hole for linker synthesis
	or.u	r27,r0,0		; special hole for linker synthesis
	or	r27,r27,0		; special hole for linker synthesis
	or.u	r28,r0,0		; special hole for linker synthesis
	or	r28,r28,0		; special hole for linker synthesis
	or.u	r29,r0,0		; special hole for linker synthesis
	or	r29,r29,0		; special hole for linker synthesis
#ifdef	CMMU
	bsr	_cache_init		; QUICKLY INITIALIZE ALL CMMU CACHE
#endif

	or.u	r5,r0,hi16(_linkregs)
	or	r5,r5,lo16(_linkregs)
	st	r26,r5,0
	st	r27,r5,4
	st	r28,r5,8
	st	r29,r5,0xc
	
	ldcr	r5,cr1			; enable fpu and
	and	r5,r5,0xfff3		; misaligned access trap
	stcr	r5,cr1

	tb1	0,r0,0x80		; wait for scorboard to be clear
	stcr	r0,cr3			; clear shadow scoreboard

	fstcr	r0,cr62		; Default Inexact FP exceptions off
	fstcr	r0,cr63		; Default Inexact FP exceptions off
;	lda.b	r5,r0,_end+ROUND	; top of unix
	or.u	r5,r0,hi16(_end)
	lda.b	r5,r5,lo16(_end)
	addu	r5,r5,ROUND		; top of unix
	and	r5,r5,lo16(GMASK)
	add	r5,r5,STSIZE-SCRATCH	; for boot stack area.  This
;					 allocates 16 longs for temp
;					 storage also.
	stcr	r5,cr19
;					  during initial debug.

;	bsr	_kdb_init

	or.u	r5,r0,hi16(HIPRI)	; Set up psr to block interrupts.
	or	r5,r5,lo16(HIPRI)
;
	ldcr	r5,cr0			; Save PID register.
;	st	r5,r0,_chiprev
	or.u	r6,r0,hi16(_chiprev)
	st	r5,r6,lo16(_chiprev)
;
	or.u	r5,r0,hi16(0x88000)	; Set CPU type.
	or	r5,r5,lo16(0x88000)
;	st	r5,r0,_cputype 
	or.u	r6,r0,hi16(_cputype)
	st	r5,r6,lo16(_cputype)
;

;	st	r2,r0,bootdriv		; Save boot loader passed info.
;	st	r3,r0,bootctlr		; 
;	st	r4,r0,bootinod

;	ldcr	r5,cr20			; get rom value in cr20
;	st	r5,r0,_dbgcr20		; and save it (for later restore)

;	or.u	r5,r0,hi16(instint)	; Set up vector to ROM illegal handler.
;	or	r5,r5,lo16(instint)
;	st	r5,r0,_dbgill

;	or.u	r5,r0,hi16(breakpoint)	; Setup vector to ROM breakpoint hndlr.
;	or	r5,r5,lo16(breakpoint)
;	st	r5,r0,_dbgbkpt

;	ldcr	r5,cr7			; save ROMs vbr
;	st	r5,r0,romvbr

	or.u	r5,r0,hi16(_M88Kvec)  	; Setup vbr
	or	r5,r5,lo16(_M88Kvec)
	stcr	r5,cr7
;	st	r5,r0,vbr
	or.u	r6,r0,hi16(vbr)
	st	r5,r6,lo16(vbr)

#ifdef	ASPEN
; ASPEN specific.
	or.u	r5,r0,hi16(FSCC) 	; Flush the caches.
	or	r5,r5,lo16(FSCC) 
	st	r0,r5,0		  	; Flush SCC
	st	r0,r5,4		  	; Flush SDC
	st	r0,r5,8		  	; Flush UCC
	st	r0,r5,12		; Flush UDC

	or.u	r5,r0,hi16(CSR)  ; Setup control register.
	or	r5,r5,lo16(CSR)
	add	r4,r0,CACHEON+VMEINT
;	st.b	r4,r5,0

	or.u	r5,r0,hi16(ASR)  ; Setup Address modifier register.
	or	r5,r5,lo16(ASR)
	add	r4,r0,0xFD
;	st.b	r4,r5,0
; end of ASPEN specific.
#endif

#if 0
;	add	r2,r0,4			; Remove patch for no ramdisk.

	bcnd	lt0,r2,@L%strt		; check for valid boot device.
	lda	r4,r0,_sysdevs		;
	add	r4,r4,4
	mak	r2,r2,2<4>		; Get pointer to boot device info.
	mak	r3,r3,4<6>
	add	r4,r4,r2
	add	r4,r4,r3

	ld.h	r2,r4,0			; Setup boot device info.
	st.h	r2,r0,_rootdev
	ld.h	r2,r4,2
	st.h	r2,r0,_pipedev
	ld.h	r2,r4,4
	st.h	r2,r0,_dumpdev
	ld.h	r2,r4,6
	st.h	r2,r0,_swapdev
	ld	r2,r4,8
	st	r2,r0,_swplo
	ld	r2,r4,12
	st	r2,r0,_nswap
#endif
	
@L%strt:
;	lda.b	r2,r0,_end+ROUND	; top of unix
	or.u	r2,r0,hi16(_end)
	lda.b	r2,r2,lo16(_end)
	addu	r2,r2,ROUND		; top of unix
	and	r2,r2,lo16(GMASK)
	add	r2,r2,STSIZE-SCRATCH	; for boot stack area.  This
;					 allocates 16 longs for temp
;					 storage also.
;	lda.b	r3,r0,_edata
	or.u	r3,r0,hi16(_edata)
	lda.b	r3,r3,lo16(_edata)
	sub	r4,r2,r3		; Get count.

#ifndef SIMULATOR
@L%clr:	st	r0,r3,0			; Clear next word of bss.
	sub	r4,r4,4
	bcnd.n	ge0,r4,@L%clr
	add	r3,r3,4
#endif

; Call C routines to startup system.

;	tb0	0,r0,133		; Call kdb.

	add	r31,r2,0		; Use boot stack area until mmu is on.
					; afterwards, use area as interrupt stk
					; pass start of ram address to mlsetup
	add	r2,r2,SCRATCH		; Start above scratch area.
	subu	r31,r31,HOMESP		; make room for saving passed params 
	bsr	_mlsetup		; mlsetup(physaddr)

	or.u	r31,r0,hi16(_u+U_SIZE)  ; Switch into u block.
	or	r31,r31,lo16(_u+U_SIZE)
	stcr	r31,cr17		; Setup master stack ptr.
	subu	r31,r31,HOMESP		; make room for saving passed params 
	bsr	_main			; main()

					; 1st proc 0 returns here
	or.u	r2,r0,hi16(HIPRI)	; Mask off interrupts while
	or	r2,r2,lo16(HIPRI)	; loading shadows.
	stcr	r2,cr1
	set	r2,r2,1<PS_EXP>
	stcr	r2,cr1
	add	r2,r0,USRPSR		; Setup user psr.
	stcr	r2,cr2			; Setup psr.
	stcr	r0,cr3			; Setup user SSB.
	stcr	r0,cr5			; SNIP.
	set	r2,r0,1<VALIDIP>	; Set initial start at 0.
	stcr	r2,cr6			; SFIP.
	br	goof
goof2:
	or	r0,r0,r0
	rte				; execute user process
goof:
	br	goof2

	data
romillv:
	word	Xtrap
_linkregs:
	word	0
	word	0
	word	0
	word	0

	global	_cputype
_cputype:
	word	0
	global	_chiprev
_chiprev:
	word	0

	global	_dbgswab
_dbgswab:			; vector to ROM/KDB NMI interrupt.
	word	0
	global	_dbgpanic

_dbgpanic:			; vector to ROM/KDB NMI interrupt.
	word	0
	global	_dbgtrace

_dbgtrace:			; vector to ROM/KDB NMI interrupt.
	word	0

	global	_dbgill		; vector to ROM/KDB illegal handler.
_dbgill:
	word	0
	global	_dbgbkpt	; vector to ROM/KDB breakpoint handler.

_dbgbkpt:
	word	0
_dbgcr20:			; ROM value in cr20.
	word	0
romvbr:
	word	0


	text
;		Copyright (c) 1985 AT&T		#
;		All Rights Reserved		#
;	@(#)cswitch.m4	6.15	

; MC78000 version
; This file contains the routines used for context switching.

;	The routines contained here are:

	global	_setjmp		; set up environment for external goto
	global	_longjmp	; non-local goto
	global	_save		; saves register sets
	global	_resume		; restores register sets & u, then longjmps


; setjmp (save_area)
; save (save_area)
;	saves registers and return location in save_area


	text
_setjmp:
_save:
; Only registers that are required to be saved by the C calling convention
; need be saved and restored.  These are r29,r30, and r31.  The return address
; must also be saved so that control may return via longjmp or resume.

	st	r1,r2,0		; Save the registers.
	st	r14,r2,4
	st	r15,r2,8
	st	r16,r2,12
	st	r17,r2,16
	st	r18,r2,20
	st	r19,r2,24
	st	r20,r2,28
	st	r21,r2,32
	st	r22,r2,36
	st	r23,r2,40
	st	r24,r2,44
	st	r25,r2,48
	st	r26,r2,52
	st	r27,r2,56
	st	r28,r2,60
	st	r29,r2,64
	st	r30,r2,68
	st	r31,r2,72
	fldcr	r3,cr62		; Save floating point control register.
	st	r3,r2,76
	fldcr	r3,cr63		; Save floating point status register.
	st	r3,r2,80
	jmp.n   r1			; return(0)
	add	r2,r0,0


; longjmp (save_area)
;	resets registers from save_area and returns to the location
;	from WHERE setjmp() WAS CALLED.
;	** Never returns 0 - this lets the returned-to environment know that the
;	return was the result of a long jump.

_longjmp:
	ld	r1,r2,0		; Restore the registers.
	ld	r14,r2,4
	ld	r15,r2,8
	ld	r16,r2,12
	ld	r17,r2,16
	ld	r18,r2,20
	ld	r19,r2,24
	ld	r20,r2,28
	ld	r21,r2,32
	ld	r22,r2,36
	ld	r23,r2,40
	ld	r24,r2,44
	ld	r25,r2,48
	ld	r26,r2,52
	ld	r27,r2,56
	ld	r28,r2,60
	ld	r29,r2,64
	ld	r30,r2,68
	ld	r31,r2,72
	jmp.n   r1			; return(1)
	add	r2,r0,1



; resume (&u, save_area)
;	switches to another process's "u" area. Returns non-zero.

_resume:

;
#ifdef	ASPEN
	def	RP,0		; Root Pointer register
	def	MMUADDR,0x10000038	; in CPU space
#endif

	ldcr	r4,cr1		; Mask interrupts.
	add	r6,r4,0		; Save old cr1.
	set	r4,r4,1<PS_IPL>
	stcr	r4,cr1

#ifdef	ASPEN
; Flush code and data caches.
; Aspen specific.
	or.u	r4,r0,hi16(FSCC)
	or	r4,r4,lo16(FSCC)
	st	r0,r4,4		; Flush supervisor data
	st	r0,r4,8		; Flush user text 
	st	r0,r4,12	; Flush user data
#endif

; Map in new 8k ublock.
;	ld	r4,r0,_mmuupde	; Get pointer to u_block entries.
	or.u	r4,r0,hi16(_mmuupde)
	ld	r4,r4,lo16(_mmuupde) ; Get pointer to u_block entries.
	ld	r5,r2,0		; Get the ublock PTEs #1
	st	r5,r4,0
	ld	r5,r2,4		; Get the ublock PTEs #2
	st	r5,r4,4
#ifdef	ASPEN
	ld	r5,r2,8		; Get the ublock PTEs #3
	st	r5,r4,8
	ld	r5,r2,12	; Get the ublock PTEs #4
	st	r5,r4,12
	ld	r5,r2,16	; Get the ublock PTEs #5
	st	r5,r4,16
	ld	r5,r2,20	; Get the ublock PTEs #6
	st	r5,r4,20
	ld	r5,r2,24	; Get the ublock PTEs #7
	st	r5,r4,24
	ld	r5,r2,28	; Get the ublock PTEs #8
	st	r5,r4,28

; Flush MMU Address cache.
	ld	r2,r0,_fcltbl	; flush by re-writing root pointer.
	or.u	r4,r0,hi16(MMUADDR)
	st	r2,r4,lo16(MMUADDR)
#endif
#ifdef	CMMU
	bsr	_cmmuflush
#endif
	stcr	r6,cr1		; Restore priority level.
; Restore the registers from the new u_block.
	ld	r1,r3,0		; Restore the registers.
	ld	r14,r3,4
	ld	r15,r3,8
	ld	r16,r3,12
	ld	r17,r3,16
	ld	r18,r3,20
	ld	r19,r3,24
	ld	r20,r3,28
	ld	r21,r3,32
	ld	r22,r3,36
	ld	r23,r3,40
	ld	r24,r3,44
	ld	r25,r3,48
	ld	r26,r3,52
	ld	r27,r3,56
	ld	r28,r3,60
	ld	r29,r3,64
	ld	r30,r3,68
	ld	r31,r3,72
	ld	r2,r3,76
	fstcr	r2,cr62		; Restore floating point control register.
	ld	r2,r3,80
	fstcr	r2,cr63		; Restore floating point status register.
	jmp.n   r1			; return(1)
	add	r2,r0,1

;		Copyright (c) 1985 AT&T		#
;		All Rights Reserved		#
;	@(#)userio.m4	6.3	

; MC78000 version
	file	"userio.s"
;
;  All networking hooks in this package are disabled until the u_block offset
; becomes known and stable.  They should be reenabled when the kernel is
; debugged and testing of network stuff is about to commence.
;
;	errret: return here is address is bad

	data

	global	_fuerror
_fuerror:
	word	0

	text
	global	_errret
_errret:
	sub	r2,r0,1
;	st	r2,r0,_fuerror
	or.u	r3,r0,hi16(_fuerror)
	st	r2,r3,lo16(_fuerror)
	jmp   r1			; Return -1 on invalid address.
;
;
;
	global	_nofulow
_nofulow:
;
;
;	fubyte(addr)
	global	_fubyte
_fubyte:
;
;	fuibyte(addr)
	global	_fuibyte
_fuibyte:
;	or.u	r5,r0,hi16(_u+u_procp)	; check if process is a server.
;	ld	r5,r5,lo16(_u+u_procp)
;	ld.hu	r5,r5,p_sysid		; If so, handle it through rfubyte.
;	bcnd	eq0,r5,kfubyte
;	br	_rfubyte

kfubyte:
	or	r0,r0,r0		; DELAY
	ld.bu.usr	r2,r2,r0	; Get the byte.
	or	r0,r0,r0		; DELAY2
	or	r0,r0,r0		; DELAY2
	or	r0,r0,r0		; DELAY2
	jmp     r1
;
	global	_fuhalf
_fuhalf:
	or	r0,r0,r0		; DELAY
	ld.hu.usr	r2,r2,r0
	or	r0,r0,r0		; DELAY2
	or	r0,r0,r0		; DELAY2
	or	r0,r0,r0		; DELAY2
	jmp	r1
;	fuword(addr)
	global	_fuword
_fuword:
;
;	fuiword(addr)
	global	_fuiword
_fuiword:
;	or.u	r5,r0,hi16(_u+u_procp)	; check if process is a server.
;	ld	r5,r5,lo16(_u+u_procp)
;	ld.hu	r5,r5,p_sysid		; If so, handle it through rfuword.
;	bcnd	eq0,r5,kfuword
;	br	_rfuword

kfuword:
	bb1	0,r2,aderr		; Branch if address is odd.
	bb1	1,r2,aderr		; Branch if address is not on long.
	or	r0,r0,r0		; DELAY
	ld.usr	r2,r2,r0		; Get the word.
	or	r0,r0,r0		; DELAY2
	or	r0,r0,r0		; DELAY2
	or	r0,r0,r0		; DELAY2
	jmp	r1
;
;	subyte(addr,byte)
	global	_subyte
_subyte:
;
;	suibyte(addr,byte)
	global	_suibyte
_suibyte:
;	or.u	r5,r0,hi16(_u+u_procp)	; check if process is a server.
;	ld	r5,r5,lo16(_u+u_procp)
;	ld.hu	r5,r5,p_sysid		; If so, handle it through rsubyte.
;	bcnd	eq0,r5,ksubyte
;	br	_rsubyte

ksubyte:
	or	r0,r0,r0		; DELAY
	st.b.usr r3,r2,r0		; store the byt
	or	r0,r0,r0		; DELAY2
	or	r0,r0,r0		; DELAY2
	or	r0,r0,r0		; DELAY2
	jmp.n	r1
	add	r2,r0,0		; return 0.
;
	global	_suhalf
_suhalf:
	or	r0,r0,r0		; DELAY
	st.h.usr r3,r2,r0
	or	r0,r0,r0		; DELAY2
	or	r0,r0,r0		; DELAY2
	or	r0,r0,r0		; DELAY2
	jmp.n	r1
	add	r2,r0,0		; return 0.
;	suword(addr,word)
	global	_suword
_suword:
;
;	suiword(addr,word)
	global	_suiword
_suiword:
;	or.u	r5,r0,hi16(_u+u_procp)	; check if process is a server.
;	ld	r5,r5,lo16(_u+u_procp)
;	ld.hu	r5,r5,p_sysid		; If so, handle it through rsuword.
;	bcnd	eq0,r5,ksuword
;	br	_rsuword

ksuword:
	bb1	0,r2,aderr		; Branch if address is odd.
	bb1	1,r2,aderr		; Branch if address is not on long.
	or	r0,r0,r0		; DELAY
	st.usr	r3,r2,r0			; store the word.
	or	r0,r0,r0		; DELAY2
	or	r0,r0,r0		; DELAY2
	or	r0,r0,r0		; DELAY2

	jmp.n	r1
	add	r2,r0,0		; return 0.

aderr:
;  Maybe this should be handled thru exception on misalignment instead?
	sub	r2,r0,1
;	st	r2,r0,_fuerror
	or.u	r3,r0,hi16(_fuerror)
	st	r2,r3,lo16(_fuerror)
	jmp   r1			; Return -1 on invalid address.

;
; upath(from, to, maxbufsize);
;  Copies a path from user space to kernel space.
;
;  Note: the path is assumed to be 4K or less by this routine.

	global	_upath
_upath:
;	or.u	r5,r0,hi16(_u+u_procp)	; check if process is a server.
;	ld	r5,r5,lo16(_u+u_procp)
;	ld.hu	r5,r5,p_sysid		; If so, handle it through spath.
;	bcnd	ne0,r5,spath


;  Find the length of the path.
	add	r9,r0,0		; First calculate the string length.
upathlen:
	or	r0,r0,r0		; DELAY
	ld.bu.usr r6,r2,r9	; Search for the first null.
	or	r0,r0,r0		; DELAY2
	or	r0,r0,r0		; DELAY2
	or	r0,r0,r0		; DELAY2
	bcnd.n	ne0,r6,upathlen
	add	r9,r9,1

	sub	r6,r9,r4	; Too big?
	bcnd	gt0,r6,plenerr

	add	r8,r1,0		; Save the return address
	bsr.n	_copyin
	add	r4,r9,0		; Use bcopy.

	jmp.n	r8
	add	r2,r9,0

; uzero(vaddr, size)
; blkuzero(vaddr, size)
;	addr is a user virtual address
;	size is in bytes

	global	_uzero
	global	_blkuzero

_uzero:
	extu	r4,r3,30<2>		; Calculate number of longs, if its
	bcnd	eq0,r4,uzerobs		; r0, go clear it as bytes.

	bb0	0,r2,uzalh		; Align to the next half word boundary.
	or	r0,r0,r0		; DELAY
	st.b.usr r0,r2,r0
	or	r0,r0,r0		; DELAY2
	or	r0,r0,r0		; DELAY2
	or	r0,r0,r0		; DELAY2
	add	r2,r2,1
	sub	r3,r3,1

uzalh:	bb0.n	1,r2,blkuzero1		; Align to the next long word boundary.
	extu	r4,r3,30<2>		; Recalculate the number of longs
	or	r0,r0,r0		; DELAY
	st.h.usr r0,r2,r0
	or	r0,r0,r0		; DELAY2
	or	r0,r0,r0		; DELAY2
	or	r0,r0,r0		; DELAY2
	add	r2,r2,2
	sub	r3,r3,2

_blkuzero:
	extu	r4,r3,30<2>		; Recalculate the number of longs
blkuzero1:
	bcnd	eq0,r4,uzerob		; (it might have changed) and then
;					  fall through to clear all longs 
;					  possible.
uzerolp:
	or	r0,r0,r0		; DELAY
	st.usr	r0,r2,r0		; Clear the next long
	or	r0,r0,r0		; DELAY2
	or	r0,r0,r0		; DELAY2
	or	r0,r0,r0		; DELAY2
	sub	r4,r4,1
	bcnd.n	ne0,r4,uzerolp
	add	r2,r2,4

uzerob:
	bb0	0,r3,uzeroh		; Clear any residue byte count.
	or	r0,r0,r0		; DELAY
	st.b.usr r0,r2,r0
	or	r0,r0,r0		; DELAY2
	or	r0,r0,r0		; DELAY2
	or	r0,r0,r0		; DELAY2
	add	r2,r2,1
uzeroh:
	bb0	1,r3,uzerodn		; Clear the half word if any.
	or	r0,r0,r0		; DELAY
	st.h.usr r0,r2,r0
uzerodn:
	or	r0,r0,r0		; DELAY2
	or	r0,r0,r0		; DELAY2
	or	r0,r0,r0		; DELAY2
	jmp.n   r1			; Return 0 to say everything is ok.
	add	r2,r0,0

uzerobs1:
	or	r0,r0,r0		; DELAY
	st.b.usr r0,r2,r0		; Clear the next byte.  This clears 
	or	r0,r0,r0		; DELAY2
	or	r0,r0,r0		; DELAY2
	or	r0,r0,r0		; DELAY2
	add	r2,r2,1			; the case.
uzerobs:
	bcnd.n	ne0,r3,uzerobs1		; alignment.  This is not usually
	sub	r3,r3,1			; 3 or less bytes and we don't know
	tb1	0,r0,128		; DELAY
	jmp.n   r1			; Return 0 to say everything is ok.
	add	r2,r0,0
;###############################################################################

	global	_copyin
	global 	_copyout
#ifdef opus
	global	copyinb
	global	copyinlp
	global	_short_copyin
_short_copyin:
_copyin:
	bb1	0,r2,copyinb
	bb1	1,r2,copyinb
	bb1	0,r3,copyinb
	bb1	1,r3,copyinb

	extu	r5,r4,30<2>		; calculate the number of longs
	bcnd	eq0,r5,copyinb
copyinlp:
;  Control comes here when the addresses are both aligned to word boundaries.
	or	r0,r0,r0		; DELAY
	ld.usr	r6,r2,r0
	or	r0,r0,r0		; DELAY2
	or	r0,r0,r0		; DELAY2
	or	r0,r0,r0		; DELAY2
	sub	r5,r5,1
	add	r2,r2,4
	st	r6,r3,0			; Copy the next long.
	bcnd.n	ne0,r5,copyinlp
	add	r3,r3,4
	mask	r4,r4,3

copyinb:
	bcnd	eq0,r4,copyindone
copyinblp:
	or	r0,r0,r0		; DELAY
	ld.b.usr	r6,r2,r0
	or	r0,r0,r0		; DELAY2
	or	r0,r0,r0		; DELAY2
	or	r0,r0,r0		; DELAY2
	st.b	r6,r3,0			; Copy the next byte.
	sub	r4,r4,1
	add	r3,r3,1
	bcnd.n	ne0,r4,copyinblp
	add	r2,r2,1

copyindone:
	jmp.n   r1
	add	r2,r0,0		; return (0)

	global	copyoutb
	global	copyoutlp
	global 	_short_copyout
_short_copyout:
_copyout:
	bb1	0,r2,copyoutb
	bb1	1,r2,copyoutb
	bb1	0,r3,copyoutb
	bb1	1,r3,copyoutb

	extu	r5,r4,30<2>		; calculate the number of longs
	bcnd	eq0,r5,copyoutb		; (it might have changed) and then
copyoutlp:
;  Control comes here when the addresses are both aligned to word boundaries.
	ld	r6,r2,r0
	sub	r5,r5,1
	add	r2,r2,4
	or	r0,r0,r0		; DELAY
	st.usr	r6,r3,r0		; Copy the next long.
	or	r0,r0,r0		; DELAY2
	or	r0,r0,r0		; DELAY2
	or	r0,r0,r0		; DELAY2

	bcnd.n	ne0,r5,copyoutlp
	add	r3,r3,4
	mask	r4,r4,3

copyoutb:
	bcnd	eq0,r4,copyoutdone
copyoutblp:
	ld.b	r6,r2,r0
	or	r0,r0,r0		; DELAY
	st.b.usr r6,r3,r0			; Copy the next long.
	or	r0,r0,r0		; DELAY2
	or	r0,r0,r0		; DELAY2
	or	r0,r0,r0		; DELAY2
	sub	r4,r4,1
	add	r3,r3,1
	bcnd.n	ne0,r4,copyoutblp
	add	r2,r2,1

copyoutdone:
	tb1	0,r0,128		; DELAY
	jmp.n   r1
	add	r2,r0,0		; return (0)
#else
	global	_short_copyin
_short_copyin:
_copyin:
;
;  copyin copies data from the user address space into the kernel 
; address space.  The calling sequence is:
;
; 	copyin(useraddr,kernaddr,count);
;
;	or.u	r5,r0,hi16(_u+u_procp)	; check if process is a server.
;	ld	r5,r5,lo16(_u+u_procp)
;	ld.hu	r5,r5,p_sysid		; If so, handle it through rcopyin
;	bcnd	eq0,r5,kcopyin
;	br	_rcopyin

kcopyin:	
;
;  If possible, align the start and end addresses on a long word
;  boundary for efficiency sake.
;

	extu	r5,r4,30<2>		; Calculate number of longs, if its
	bcnd	eq0,r5,copyinb		; r0, go copy it as bytes.
;
;  In the case that aligment of both from and to is impossible, the next best
; alternative is to align "to" to a long word boundary since writes are always
; written back even for byte or half word updates.  

	bb0	0,r3,cinalh		; Align to the next half word boundary.
	or	r0,r0,r0		; DELAY
	ld.bu.usr r5,r2,r0
	or	r0,r0,r0		; DELAY2
	or	r0,r0,r0		; DELAY2
	or	r0,r0,r0		; DELAY2
	add	r2,r2,1
	sub	r4,r4,1
	st.b	r5,r3,0
	add	r3,r3,1

; Now "to" is aligned to a half word boundary.
cinalh:	bb0	1,r3,cintoaln		; Align to the next long word boundary.
	bb0	0,r2,cintohfs		; If from is not on a half word boundary,


	or	r0,r0,r0		; DELAY
	ld.bu.usr r6,r2,r0	; Copy a two bytes to the next half word.
	or	r0,r0,r0		; DELAY2
	or	r0,r0,r0		; DELAY2
	or	r0,r0,r0		; DELAY2
	add	r2,r2,1
	or	r0,r0,r0		; DELAY
	ld.bu.usr r5,r2,r0	; to will then be on a long word boundary.
	or	r0,r0,r0		; DELAY2
	or	r0,r0,r0		; DELAY2
	or	r0,r0,r0		; DELAY2
	add	r2,r2,1
	sub	r4,r4,2
	mak	r6,r6,8<8>
	or	r5,r6,r5
	st.h	r5,r3,0
	add	r3,r3,2
;  copy longs piecemeal. In this case, to is on an even address but from is on an odd
; address.  In this case, alignment of both is impossible.
copyinpc:
	extu	r7,r4,30<2>	; Recalculate number of longs.
	bcnd	eq0,r7,copyinb
copyinbhb:
	or	r0,r0,r0		; DELAY
	ld.bu.usr r5,r2,r0	; Copy a two bytes to the next half word.
	or	r0,r0,r0		; DELAY2
	or	r0,r0,r0		; DELAY2
	or	r0,r0,r0		; DELAY2
	add	r2,r2,1
	or	r0,r0,r0		; DELAY
	ld.hu.usr r6,r2,r0
	or	r0,r0,r0		; DELAY2
	or	r0,r0,r0		; DELAY2
	or	r0,r0,r0		; DELAY2
	add	r2,r2,2
	mak	r5,r5,8<16>
	or	r5,r6,r5
	or	r0,r0,r0		; DELAY
	ld.bu.usr r6,r2,r0
	or	r0,r0,r0		; DELAY2
	or	r0,r0,r0		; DELAY2
	or	r0,r0,r0		; DELAY2
	add	r2,r2,1
	mak	r5,r5,24<8>
	or	r5,r6,r5
	st	r5,r3,0
	sub	r7,r7,1
	bcnd.n	ne0,r7,copyinbhb
	add	r3,r3,4

	br	copyinb

cintohfs:
;  Alignment of "to" is possible using a half word.
	or	r0,r0,r0		; DELAY
	ld.hu.usr r5,r2,r0
	or	r0,r0,r0		; DELAY2
	or	r0,r0,r0		; DELAY2
	or	r0,r0,r0		; DELAY2
	add	r2,r2,2
	sub	r4,r4,2
	st.h	r5,r3,0
	add	r3,r3,2
cintoaln:
;  At this point "to" is aligned on a long word boundary.  If "from" is
; also aligned, we can copy longs, otherwise, we have to load half words.
;
;					  
	bb1	0,r2,copyinpc		; If "from" is odd, bad news.
	bb1.n	1,r2,copyinh		; Do we have to move halfs?
	extu	r5,r4,30<2>		; Recalculate the number of longs
	bcnd	eq0,r5,copyinb		; (it might have changed) and then
copyinlp:
;  Control comes here when the addresses are both aligned to word boundaries.
	or	r0,r0,r0		; DELAY
	ld.usr	r6,r2,r0
	or	r0,r0,r0		; DELAY2
	or	r0,r0,r0		; DELAY2
	or	r0,r0,r0		; DELAY2
	sub	r5,r5,1
	add	r2,r2,4
	st	r6,r3,0			; Copy the next long.
	bcnd.n	ne0,r5,copyinlp
	add	r3,r3,4

	br	copyinb			; Go copy the remainder as bytes.

copyinh:
	bcnd	eq0,r5,copyinb		; (it might have changed) and then
;  copy longs in half words. 
copyinhlp:
	or	r0,r0,r0		; DELAY
	ld.hu.usr	r7,r2,r0	;Copy a two bytes to the next half word.
	or	r0,r0,r0		; DELAY2
	or	r0,r0,r0		; DELAY2
	or	r0,r0,r0		; DELAY2
	add	r2,r2,2
	or	r0,r0,r0		; DELAY
	ld.hu.usr	r6,r2,r0
	or	r0,r0,r0		; DELAY2
	or	r0,r0,r0		; DELAY2
	or	r0,r0,r0		; DELAY2
	add	r2,r2,2
	mak	r7,r7,16<16>
	or	r7,r6,r7
	st	r7,r3,0
	sub	r5,r5,1
	bcnd.n	ne0,r5,copyinhlp
	add	r3,r3,4

copyinb:
;	clr	r4,r4,30<2>
	mask r4,r4,3
	bcnd	eq0,r4,cpydone
copyinblp:
	or	r0,r0,r0		; DELAY
	ld.b.usr	r6,r2,r0
	or	r0,r0,r0		; DELAY2
	or	r0,r0,r0		; DELAY2
	or	r0,r0,r0		; DELAY2
	st.b	r6,r3,0			; Copy the next byte.
	sub	r4,r4,1
	add	r3,r3,1
	bcnd.n	ne0,r4,copyinblp
	add	r2,r2,1

cpydone:
	jmp.n   r1
	add	r2,r0,0		; return (0)
;
; short_copyin	- copyin that won't use 32 bit buss xfers
;
;  This routine is used for networking and is disabled for now.
;
;short_copyin:	
;	bsr.b	getcpyp		;  Call getcpyp to get all of the 
;	beq.b	cinret		; passed parameters into registers and
;;				  if the count is <= 0, exit immediately.
;;
;;  If possible, align the start and end addresses on a long word
;;  boundary for efficiency sake.

	global 	_short_copyout
_short_copyout:
_copyout:
;
;  copyout copies data from the kernel address space into the user 
; address space.  The calling sequence is:
;
; 	copyout(kernaddr,useraddr,count);
;

;	or.u	r5,r0,hi16(_u+u_procp)	; check if process is a server.
;	ld	r5,r5,lo16(_u+u_procp)
;	ld.hu	r5,r5,p_sysid		; If so, handle it through rcopyout.
;	bcnd	eq0,r5,kcopyout
;	br	_rcopyout

kcopyout:

;  If possible, align the start and end addresses on a long word
;  boundary for efficiency sake.
;

	extu	r5,r4,30<2>		; Calculate number of longs, if its
	bcnd	eq0,r5,copyoutb		; r0, go copy it as bytes.
;
;  In the case that aligment of both from and to is impossible, the next best
; alternative is to align "to" to a long word boundary since writes are always
; written back even for byte or half word updates.  

	bb0	0,r3,coutalh		; Align to the next half word boundary.
	ld.bu   r5,r2,r0
	add	r2,r2,1
	sub	r4,r4,1
	or	r0,r0,r0		; DELAY
	st.b.usr r5,r3,r0
	or	r0,r0,r0		; DELAY2
	or	r0,r0,r0		; DELAY2
	or	r0,r0,r0		; DELAY2
	add	r3,r3,1

coutalh: bb0	1,r3,couttoaln		; Align to the next long word boundary.
	bb0	0,r2,couttohfs		; If from is not on a half word boundary,


	ld.bu	r6,r2,r0	; Copy a two bytes to the next half word.
	ld.bu	r5,r2,1	; to will then be on a long word boundary.
	add	r2,r2,2
	mak	r6,r6,8<8>
	or	r5,r6,r5
	or	r0,r0,r0		; DELAY
	st.h.usr	r5,r3,r0
	or	r0,r0,r0		; DELAY2
	or	r0,r0,r0		; DELAY2
	or	r0,r0,r0		; DELAY2
	add	r3,r3,2
	sub	r4,r4,2
;  copy longs piecemeal. In this case, to is on an even address but from is on an odd
; address.  In this case, alignment of both is impossible.
copyoutpc:
	extu	r7,r4,30<2>	; Recalculate number of longs.
	bcnd	eq0,r7,copyoutb
copyoutbhb:
	ld.bu	 r5,r2,r0	; Copy a two bytes to the next half word.
	ld.hu	 r6,r2,1
	mak	r5,r5,8<16>
	or	r5,r6,r5
	ld.bu	 r6,r2,3
	mak	r5,r5,24<8>
	add	r2,r2,4
	or	r5,r6,r5
	or	r0,r0,r0		; DELAY
	st.usr	r5,r3,r0
	or	r0,r0,r0		; DELAY2
	or	r0,r0,r0		; DELAY2
	or	r0,r0,r0		; DELAY2

	sub	r7,r7,1
	bcnd.n	ne0,r7,copyoutbhb
	add	r3,r3,4

	br	copyoutb

couttohfs:
;  Alignment of "to" is possible using a half word.
	ld.hu	r5,r2,r0
	add	r2,r2,2
	sub	r4,r4,2
	or	r0,r0,r0		; DELAY
	st.h.usr	r5,r3,r0
	or	r0,r0,r0		; DELAY2
	or	r0,r0,r0		; DELAY2
	or	r0,r0,r0		; DELAY2
	add	r3,r3,2
couttoaln:
;  At this point "to" is aligned on a long word boundary.  If "from" is
; also aligned, we can copy longs, otherwise, we have to load half words.
;
;					  
	bb1	0,r2,copyoutpc
	bb1	1,r2,copyouth		; Do we have to move halfs?

copyoutlngs:
	extu	r5,r4,30<2>		; Recalculate the number of longs
	bcnd	eq0,r5,copyoutb		; (it might have changed) and then
copyoutlp:
	ld	r6,r2,r0
	sub	r5,r5,1
	add	r2,r2,4
	or	r0,r0,r0		; DELAY
	st.usr	r6,r3,r0			; Copy the next long.
	or	r0,r0,r0		; DELAY2
	or	r0,r0,r0		; DELAY2
	or	r0,r0,r0		; DELAY2

	bcnd.n	ne0,r5,copyoutlp
	add	r3,r3,4

	br	copyoutb			; Go copy the remainder as bytes.

copyouth:
	extu	r5,r4,30<2>		; Recalculate the number of longs
	bcnd	eq0,r5,copyoutb		; (it might have changed) and then
;  copy longs in half words. 
copyouthlp:
	ld.hu	r7,r2,r0			; Copy a two bytes to the next half word.
	ld.hu	r6,r2,2
	sub	r5,r5,1
	add	r2,r2,4
	mak	r7,r7,16<16>
	or	r7,r6,r7
	or	r0,r0,r0		; DELAY
	st.usr	r7,r3,r0
	or	r0,r0,r0		; DELAY2
	or	r0,r0,r0		; DELAY2
	or	r0,r0,r0		; DELAY2

	bcnd.n	ne0,r5,copyouthlp
	add	r3,r3,4

copyoutb:
	mask	r4,r4,3
	bcnd	eq0,r4,cpyoutdone
copyoutblp:
	ld.b	r6,r2,r0
	or	r0,r0,r0		; DELAY
	st.b.usr r6,r3,r0			; Copy the next long.
	or	r0,r0,r0		; DELAY2
	or	r0,r0,r0		; DELAY2
	or	r0,r0,r0		; DELAY2
	sub	r4,r4,1
	add	r3,r3,1
	bcnd.n	ne0,r4,copyoutblp
	add	r2,r2,1

cpyoutdone:
	tb1	0,r0,128		; DELAY
	jmp.n   r1
	add	r2,r0,0		; return (0)
;
; short_copyin	- copyin that won't use 32 bit buss xfers

;  Make version from copyout when its debugged completely.
	br	_short_copyin
;	bsr.b	getcpyp		;  Call getcpyp to get all of the 
;	beq.b	cout		; passed parameters into registers and
;;				  if the count is <= 0, exit immediately.
;;
;;  If possible, align the start and end addresses on a long word
;;  boundary for efficiency sake.

#endif /* opus */
	global	_nofuhigh
_nofuhigh:
;		Copyright (c) 1985 AT&T		#
;		All Rights Reserved		#
;	@(#)math.m4	6.1 	

; MC78000 version

	text

; unsigned min(a,b)
; unsigned a,b;
; {	return a <= b ? a : b; }

	global	_min
_min:
	sub	r4,r3,r2
	bcnd	ge0,r4,L%min
	jmp.n	r1
	add	r2,r3,0
L%min:
	jmp	r1

; unsigned max(a,b)
; unsigned a,b;
; {	return a >= b ? a : b; }

	global	_max
_max:
	sub	r4,r3,r2
	bcnd	le0,r4,L%max
	jmp.n	r1
	add	r2,r3,0
L%max:
	jmp	r1

; ffs(value)
;  Find first bit set in value.
;  return 0 for no bits set. 1-32 for first bit number.
;
;
; log2(value)
; returns log2 value of passed parameter.
; Note:  Assumes only 1 bit set in passed parameter.  Bad results otherwise.

	global	_log2
_log2:
	jmp.n	r1
	ff1	r2,r2

	global	_ffs
_ffs:
	ff1	r2,r2
	cmp	r13,r2,32
	bb1	eq,r13,nonehigh
	jmp.n	r1
	addu	r2,r2,1
nonehigh:
	jmp.n	r1
	addu	r2,r0,r0
	
; exp2(value)
; returns exp2 value of passed parameter.
; (same as setting bit number "value" and returning. )
; Note: Assumes that passed parameter is between 0 and 31.

	global	_exp2
_exp2:
	add	r2,r2,0x20	; Bit field width 1.
	jmp.n	r1
	set	r2,r0,r2

;		Copyright (c) 1985 AT&T		#
;		All Rights Reserved		#
;	@(#)misc.s	1.0	
	global	_kstrcpy
_kstrcpy:
	br	_strcpy
; bcopy(from, to, size)
; blkcopy(from, to, size)
;	Copy a block of memory from "from" to "to".  The number
;	of bytes to copy is "size".  
;	"to" and "from" are both system physical addresses
;	bcopy() makes no assumption about the alignment of the 
;	addresses.
;	blkcopy() assumes that "from" and "to" start on long word
;	boundaries.  blkcopy() is slightly faster than bcopy().

	global	_bcopy
	global	_blkcopy
_bcopy:	
	or	r5,r2,r3
	or	r5,r5,r4
	mask	r5,r5,0xf
	bcnd	eq0,r5,_bcopy16

	extu	r5,r4,30<2>		; Calculate number of longs, if its
	bcnd	eq0,r5,bcopyb		; r0, go copy it as bytes.
;
;  In the case that aligment of both from and to is impossible, the next best
; alternative is to align "to" to a long word boundary since writes are always
; written back even for byte or half word updates (On the ASPEN).  

	bb0	0,r3,bcalh		; Align to the next half word boundary.
	ld.b	r5,r2,0
	add	r2,r2,1
	st.b	r5,r3,0
	add	r3,r3,1
	sub	r4,r4,1

bcalh:	bb0	1,r3,blktoaln		; Align to the next long word boundary.
	bb0	0,r2,blktohfs		; If from is not on a half word boundary,


	ld.bu	r6,r2,0			; Copy a two bytes to the next half word.
	ld.bu	r5,r2,1			; to will then be on a long word boundary.
	mak	r6,r6,8<8>		; Shift up 8 bits.
	or	r5,r6,r5
	st.h	r5,r3,0
	add	r2,r2,2
	add	r3,r3,2
	sub	r4,r4,2
;  copy longs piecemeal. In this case, to is on an even address but from is on an odd
; address.  In this case, alignment of both is impossible.
bcopypc:
	extu	r7,r4,30<2>		; Recalculate the number of longs
	bcnd	eq0,r7,bcopyb		; (it might have changed) and then
bcopybhb:
	ld.b	r5,r2,0			; Copy 2 bytes and a half word.
	ld.hu	r6,r2,1
	mak	r5,r5,8<16>		; Shift up 16 bits.
	or	r5,r6,r5
	ld.bu	r6,r2,3
	mak	r5,r5,24<8>		; Shift up 8 bits.
	or	r5,r6,r5
	st	r5,r3,0
	add	r2,r2,4
	add	r3,r3,4
	sub	r7,r7,1
	bcnd	ne0,r7,bcopybhb
	br	bcopyb

blktohfs:
;  Alignment of "to" is possible using a half word.
	ld.hu	r5,r2,0
	add	r2,r2,2
	st.h	r5,r3,0
	add	r3,r3,2
	sub	r4,r4,2
blktoaln:
;  At this point "to" is aligned on a long word boundary.  If "from" is
; also aligned, we can copy longs, otherwise, we have to load half words.
;
;					  
	bb1	0,r2,bcopypc		; If odd, copy piecemeal.
	bb1	1,r2,bcopyh		; Do we have to move halfs?

_blkcopy:
	extu	r5,r4,30<2>		; Recalculate the number of longs
	bcnd	eq0,r5,bcopyb		; (it might have changed) and then
bcopylp:
	ld	r6,r2,0
	add	r2,r2,4
	sub	r5,r5,1
	st	r6,r3,0			; Copy the next long.
	bcnd.n	ne0,r5,bcopylp
	add	r3,r3,4
	br	bcopyb			; Go copy the remainder as bytes.

bcopyh:
	extu	r5,r4,30<2>		; Recalculate the number of longs
	bcnd	eq0,r5,bcopyb		; (it might have changed) and then
;  copy longs in half words. 
bcopyhlp:
	ld.hu	r7,r2,0			; Copy a two bytes to the next half word.
	ld.hu	r6,r2,2
	mak	r7,r7,16<16>
	or	r7,r6,r7
	st	r7,r3,0
	add	r2,r2,4
	sub	r5,r5,1
	bcnd.n	ne0,r5,bcopyhlp
	add	r3,r3,4

bcopyb:
	mask	r4,r4,3
	bcnd	eq0,r4,L%done
bcopyblp:
	ld.b	r6,r2,0
	st.b	r6,r3,0			; Copy the next long.
	sub	r4,r4,1
	add	r3,r3,1
	bcnd.n	ne0,r4,bcopyblp
	add	r2,r2,1

L%done:
	jmp.n   r1
	add	r2,r0,0		; return (0)

; bzero(addr, size)
; blkzero(addr, size)
;	addr is a physical address
;	size is in bytes
;
;   both of these subroutines clear size number of bytes starting
;  at the address addr.
;
;   bzero() makes no assumption about the alignment of addr.
;   blkzero() assumes that addr is aligned on a long word 
;         boundary.  This saves a little time for clearing things
; 	  that always start on long word boundarys.
;

	global	_bzero
	global	_bzeroba
	global	_blkzero
_bzero:
_bzeroba:
	or	r4,r3,r2
	mask	r4,r4,0xf
	bcnd	eq0,r4,_bzero16

	extu	r4,r3,30<2>		; Calculate number of longs, if its
	bcnd	eq0,r4,bzerobs		; r0, go clear it as bytes.

	bb0	0,r2,bzalh		; Align to the next half word boundary.
	st.b	r0,r2,0
	add	r2,r2,1
	sub	r3,r3,1

bzalh:	bb0.n	1,r2,blkzero1		; Align to the next long word boundary.
	extu	r4,r3,30<2>		; Recalculate the number of longs
	st.h	r0,r2,0
	add	r2,r2,2
	sub	r3,r3,2

_blkzero:
	extu	r4,r3,30<2>		; Recalculate the number of longs
blkzero1:
	bcnd	eq0,r4,bzerob		; (it might have changed) and then
;					  fall through to clear all longs 
;					  possible.
bzerolp:
	st	r0,r2,0			; Clear the next long
	sub	r4,r4,1
	bcnd.n	ne0,r4,bzerolp
	add	r2,r2,4

bzerob:
	bb0	1,r3,bzerobh		; Clear any residue byte count.
	st.h	r0,r2,0
	add	r2,r2,2
bzerobh:
	bb0	0,r3,bzerodn		; and the half word if any.
	st.b	r0,r2,0
bzerodn:
	jmp.n   r1			; Return 0 to say everything is ok.
	add	r2,r0,0

bzerobs1:
	st.b	r0,r2,0			; Clear the next byte. This clears 
	add	r2,r2,1			; the case.
bzerobs:
	bcnd.n	ne0,r3,bzerobs1		; alignment.  This is not usually
	sub	r3,r3,1			; 3 or less bytes and we don't know
	jmp.n   r1			; Return 0 to say everything is ok.
	add	r2,r0,0

	text

;#######################################################################
;	searchdir(buf, size, target) - search a directory for target
;	returns offset of match, or empty slot, or -1
;
;       Register usage:
;_m4_define_(`rbuf',r2)_m4_dnl_
;_m4_define_(`rsize',r3)_m4_dnl_
;_m4_define_(`target',r4)_m4_dnl_
;_m4_define_(`targ1',r5)_m4_dnl_
;_m4_define_(`targ2',r6)_m4_dnl_
;_m4_define_(`targ3',r7)_m4_dnl_
;_m4_define_(`targ4',r8)_m4_dnl_
;_m4_define_(`bufptr',r9)_m4_dnl_
;_m4_define_(`empty',r10)_m4_dnl_
;_m4_define_(`scrtch',r11)_m4_dnl_
;_m4_define_(`dirent',r12)_m4_dnl_
;#######################################################################

	global	_searchdir

	text
_searchdir:
;  The following loads assume that structures start on long boundarys and
; are tightly packed.
	ld.hu	r5,r4,0		;  Load the target registers with the
	ld	r6,r4,2		; name were searching for.
	add	r9,r2,0		; r9 is used to search thru the buffer.
	ld	r7,r4,6	
	ld	r8,r4,10	
	add	r10,r0,0	; the buffer, empty contains first
;				  empty slot.

srchloop:
;
;  Note that the following cmpnames loop has made the assumption that r9 and
;  r4 point at entries that are 0 filled beyond the end of the names in the
;  entries.  A search of the code that calls searchdir indicated that this
;  would always be the case, but if changes in the future are made that
;  rule that assumption out, then cmpnames will have to change.
;
cmpnames:
	ld	r12,r9,0		; Get 4 bytes of directory entry.
	sub	r3,r3,16		; Dec count while load occurs.
	extu	r11,r12,16<16>		; get inode from upper 16 bits.
	bcnd.n	eq0,r11,sempty		; If 0, then entry is empty.

	extu	r12,r12,16<0>		; Get first 2 chars of name and
	sub	r12,r5,r12		; see if they're the same as the r4.
	bcnd	ne0,r12,scont		; If no, no match. (usual case)

	ld	r12,r9,4		; Get next 4 chars of name and 
	sub	r12,r6,r12		; see if they're the same as the r4.
	bcnd	ne0,r12,scont		; If no, no match.

	ld	r12,r9,8		; Get next 4 chars of name and 
	sub	r12,r7,r12		; see if they're the same as the r4.
	bcnd	ne0,r12,scont		; If no, no match.

	ld	r12,r9,12		; Get next 4 chars of name and 
	sub	r12,r8,r12		; see if they're the same as the r4.
	bcnd	eq0,r12,fndname		; If yes, this is the match.

scont:
	bcnd.n	ne0,r3,srchloop		; looking if we haven't found it.
	add	r9,r9,16
;
;  Control comes here when we have searched the entire directory and
; have not found a match.  We must return the offset of the empty slot
; that we have found or -1 if we haven't found any emptys.

sdone:
	bcnd	ne0,r10,sfound		; If we found at least an empty slot,
	jmp.n   r1				; go return its offset, else, return
	sub	r2,r0,1		; -1.
sfound:
	jmp.n   r1				; Return its offset.
	sub	r2,r10,r2	; 

sempty:
;
;  Control comes here when we find any empty slot during the search of
;  the directory.
;
	bcnd	ne0,r10,scont		;  If we already found an empty,
	br.n	scont			; don't save it.  Else, save the
	add	r10,r9,0		; empty pointer.

fndname:				;  We found it if we come here,
;
;  We found a match if we come here.
;
	jmp.n   r1				; return the offset to the match.
	sub	r2,r9,r2

;
;  spath(from, to, maxbufsize);
;  Copies a pathname from kernel to kernel space.
;  Returns length of path or -2 to indicate path was too long to
; fit.
;

	global	_spath
_spath:
	add	r9,r0,0		; First calculate the string length.
spathlen:
	ld.bu	r6,r2,r9	; Search for the first null.
	bcnd.n	ne0,r6,spathlen
	add	r9,r9,1

	sub	r6,r9,r4	; Too big?
	bcnd	gt0,r6,plenerr

	add	r8,r1,0		; Save the return address
	bsr.n	_bcopy
	add	r4,r9,0		; Use bcopy.

	jmp.n	r8
	sub	r2,r9,1		; Return the path length.
	
	
plenerr:
	jmp.n   r1
	sub	r2,r0,2		; return -2.


	global	_rcopyfault
_rcopyfault:
	or.u	r2,r0,hi16(_u+u_caddrflt)
	st	r0,r2,lo16(_u+u_caddrflt)
	jmp.n   r1
	sub	r2,r0,1		; Return -1.

; for each CMMU
; Initialize system control register (SCMR) and
; All cache set status ports (CSSP) are set to 0x3f0ff000 and
; ID (SCMID) registers are saved.
;
	global	_cache_init
_cache_init:
	ldcr	r19,cr1			; Get psr and block
	set	r15,r19,1<PS_IPL>	; interrupts
	stcr	r15,cr1

	or.u	r16,r0,0x3f0f		; Initialize Cache Set Status Port
	or	r16,r16,0xf000		; motorola 88200 bug sheet of (11/3/88)

	or.u	r17,r0,hi16(CCMMUBASE)
	ld	r17,r17,lo16(CCMMUBASE)

	or.u	r18,r0,hi16(_cmmuinfo)
	or	r18,r18,lo16(_cmmuinfo)

	add	r13,r0,r0		; loop count

idcachloop:
	ld	r20,r17,SCMID
	st	r20,r18,4
	
	ld	r20,r18,0		; policy
	st	r20,r17,SCTR		; The real System Control Register
	tb1	0,r0,128		; wait for instruction to finish

	add	r14,r0,4096		; 256 Sets * 16
cloop:
	sub	r14,r14,16		; sizeof set is 16
	st	r14,r17,SADR
	tb1	0,r0,128		; wait for instruction to finish
	st	r16,r17,CSSP
	tb1	0,r0,128		; wait for instruction to finish
	bcnd	ne0,r14,cloop

	or.u	r17,r0,hi16(DCMMUBASE)
	ld	r17,r17,lo16(DCMMUBASE)

	add	r13,r13,1
	bb1.n	0,r13,idcachloop
	add	r18,r18,8

	or	r16,r0,r0		; clear CMMU pointers so we have no
	or	r17,r0,r0		; trace of any CMMU addresses
	stcr	r19,cr1			; restore save PSR
	jmp	r1

	data
	global ccacheinh  ; user and kernel code cache inhibit
ccacheinh:
	global _ccacheinh  ; user and kernel code cache inhibit
_ccacheinh:
	word	0x200		; Write 0x40 here to inhibit code cache.
				; Write 0x80 here to make all code global
				; Write 0x200 here to write through cache

	global dcacheinh  ; kernel data cache inhibit
dcacheinh:
	global _dcacheinh  ; kernel data cache inhibit
_dcacheinh:
	word	0x00		; Write 0x40 here to inhibit data cache.
				; Write 0x80 here to make all data global
				; Write 0x200 here to write through cache

	global ucacheinh  ; user data cache inhibit
ucacheinh:
	global _ucacheinh  ; user data cache inhibit
_ucacheinh:
	word	0x00		; Write 0x40 here to inhibit data cache.
				; Write 0x80 here to make all data global
				; Write 0x200 here to write through cache
;
;
	global	cmmucmd
cmmucmd:
	word	0x0000		; 0x8000 here to enable cmmu parity
				; 0x4000 here to enable Snooping
				; 0x2000 enables priority protocol arbitration
#ifdef opus
; the above cmmucmd is ignored as there is confusion about the
; System Command Register and the System Control Register
; Note 88200 bug sheet dated 11/3/88 says DO NOT use snooping
;
; policy:
; 0x8000 enables cmmu parity
; 0x4000 enables snooping
; 0x2000 enables priority protocol arbitration (vs fairness)
	global	_cmmuinfo
_cmmuinfo:
	word	0x2000		; I CMMU policy
	word	0		; I CMMU id
	word	0x2000		; D CMMU policy
	word	0		; D CMMU id
#endif /* opus */
	text
;		Copyright (c) 1985 AT&T		#
;		All Rights Reserved		#
;	@(#)end.m4	6.3	

; MC78000 version
; This	is the code executed by proc(1) to load /etc/init.

	data
	global	_icode		; location of boot code
	global	_szicode	; size of icode in bytes

; Bootstrap program executed in user mode
; to bring up the system.
; Performs:
;	execve("/etc/init",argv, 0);

	def	exec%,8

	align	4
_icode:
	bsr	getpc		; Gotta be PIC cause it isn't where it is.
getpc:
	add	r2,r1,initoff		; Pass pointer to "/etc/init"
	add	r3,r1,argoff		; Pass pointer to argv[].
	add	r4,r0,0			; clear envp
	st	r2,r3,0			; "/etc/init" is argv[0].
	or	r9,r0,exec%
	tb0	0,r0,450		; execve( "/etc/init" );
	add	r0,r0,r0		; Debug: couple of nops.
	add	r0,r0,r0
L%here:	br	L%here			; Nothing to do if it fails.

Lifile:
	string	"/etc/init\0"		; 

	align	4
L%arg:	word	Lifile			; argv[];
	word	0

_szicode:
	word	_szicode-_icode		; size of init code
	def	initoff,Lifile-getpc
	def	argoff,L%arg-getpc
	text
	global	_bcopy16
_bcopy16:
	bcnd	eq0,r4,_bout16
_bcopy16t:
	ld.d	r6,r2,0x0000                    
	ld.d	r8,r2,0x0008                    
	subu	r4,r4,0x0010                    
	addu	r2,r2,0x0010                    
	st.d	r6,r3,0x0000                    
	st.d	r8,r3,0x0008                    
	bcnd.n	ne0,r4,_bcopy16t
	addu	r3,r3,0x0010                    
_bout16:
	jmp	r1

	global	_bzero16
_bzero16:
	bcnd	eq0,r3,_bout16
	or	r4,r0,r0
	or	r5,r0,r0
bzero16t:
	subu	r3,r3,0x0010                    
	st.d	r4,r2,0x0000                    
	st.d	r4,r2,0x0008                    
	bcnd.n	ne0,r3,bzero16t
	addu	r2,r2,0x0010                    
	jmp	r1

h06922
s 00315/00000/00000
d D 1.1 90/03/06 12:45:36 root 1 0
c date and time created 90/03/06 12:45:36 by root
e
u
U
t
T
I 1
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
;	Stub routine for distributed unix - rfs #
;	@(#)du.s	

	text
_rfubyte:	global	_rfubyte		;
_rfuword:	global	_rfuword		;
_rsubyte:	global	_rsubyte		;
_rsuword:	global	_rsuword		;
_rcopyin:	global	_rcopyin		;
_rcopyout:	global	_rcopyout	;
_advfs:		global	_advfs		;
_unadvfs:	global	_unadvfs		;
_rmount:		global	_rmount		;
_rumount:	global	_rumount		;
_rfstart:	global	_rfstart		;
_rdebug:		global	_rdebug		;
_rfstop:		global	_rfstop		;
_rfsys:		global	_rfsys		;
_duustat:	global	_duustat		;
;_dulocking:	global	_dulocking	;
_rnamei1:	global	_rnamei1		;
_rnamei2:	global	_rnamei2		;
_rnamei3:	global	_rnamei3		;
_rnamei4:	global	_rnamei4		;
_riget:		global	_riget		;
_remote_call:	global	_remote_call	;
_remio:		global	_remio		;
_rem_date:	global	_rem_date	;
_unremio:	global	_unremio		;
_remfileop:	global	_remfileop	;
	jmp	r1

;
;	Data definitions
;
	global	_nservers
	global	_idleserver
	global	_srmount
_nservers:
_idleserver:
_srmount:
	word	0			; MUST be zero

E 1

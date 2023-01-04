h62481
s 00002/00002/00396
d D 1.4 90/05/30 08:43:41 root 4 3
c modified r31
e
s 00005/00000/00393
d D 1.3 90/05/30 08:40:07 root 3 2
c added routine r31
e
s 00015/00000/00378
d D 1.2 90/04/18 08:58:59 root 2 1
c triggerscope routine
e
s 00378/00000/00000
d D 1.1 90/03/06 12:51:20 root 1 0
c date and time created 90/03/06 12:51:20 by root
e
u
U
t
T
I 1
;	Exception frames equates.
	def	HOMESP,32
	def	SP,31*4
	def	SPSR,32*4
	def	PS,32*4
	def	SCIP,33*4
	def	SNIP,34*4
	def	SFIP,35*4
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
;	SR3 status values
	def	CP_XIRQ,0x10
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
;	PCCMD handshaking flag values
	def	S_IDLE,0
	def	S_GO,1
	def	S_BUSY,2
	def	S_DONE,4
; 	Bit definitions of above
	def	B_S_GO,0
	def	B_S_BUSY,1
	def	B_S_DONE,2

	def	PS_IPL,1	; interrupt priority level 
	def	PANICBASE,0x100

_spl0:	global	_spl0
	ldcr	r3,cr1		; Get psr and block
	set	r3,r3,1<PS_IPL>	; interrupts
	stcr	r3,cr1
	or.u	r3,r0,hi16(_intlvl)
	add	r2,r0,0
;ifdef opus
	or.u	r4,r0,hi16(COMMPAGE)
	st.b	r2,r4,SPL
	st	r1,r4,SPLRET
;endif
	tb1	0,r0,128
	xmem	r2,r3,lo16(_intlvl)
;ifdef	opus
	add	r3,r0,(ENB_CPI)
	or.u	r4,r0,hi16(CR0)
	st.b	r3,r4,lo16(CR0)		; set ENB_CPI
	tb1	0,r0,128		; wait for instruction to finish
;endif	/* opus */
	ldcr	r3,cr1
	and	r3,r3,0xFFFD
	jmp.n   r1
	stcr	r3,cr1

_splinit: global _splinit
	ldcr	r3,cr1		; Get psr
	and	r3,r3,0xFFFE	; unfreeze
	stcr	r3,cr1
;	br	_spl7
; fall into spl7()

_spl1:	global	_spl1
_spl2:	global	_spl2
_spl3:	global	_spl3
_spl4:	global	_spl4
_spltty:	global	_spltty
_spl5:	global	_spl5
_splclk: global	_splclk
_spl6:	global	_spl6
_splhi:	global	_splhi
_splhigh:	global	_splhigh
_splimp:	global	_splimp
_splnet:	global	_splnet
_spl7:	global	_spl7
	ldcr	r3,cr1		; Get psr and block
	set	r3,r3,1<PS_IPL>	; interrupts
	stcr	r3,cr1
	or.u	r3,r0,hi16(_intlvl)
	add	r2,r0,7
;ifdef opus
	or.u	r4,r0,hi16(COMMPAGE)
	st.b	r2,r4,SPL
	st	r1,r4,SPLRET
	st	r1,r4,SPLRET+8
;endif
	tb1	0,r0,128
	xmem	r2,r3,lo16(_intlvl)
;ifdef opus
	add	r3,r0,(DSB_CPI)
	or.u	r4,r0,hi16(CR0)
	st.b	r3,r4,lo16(CR0)		; set DSB_CPI
	tb1	0,r0,128		; wait for instruction to finish
;endif	/* opus */
	ldcr	r3,cr1
	and	r3,r3,0xFFFD
	stcr	r3,cr1
	jmp   r1


_spli:	global	_spli		; set ipl to input integer argument value
_splx:	global	_splx		; set ipl to input argument value
	ldcr	r5,cr1		; Get psr and block
	set	r3,r5,1<PS_IPL>	; interrupts
	stcr	r3,cr1

;ifdef opus
	or.u	r4,r0,hi16(COMMPAGE)
	st.b	r2,r4,SPL
	st	r1,r4,SPLRET
	or.u	r4,r0,hi16(CR0)
	bcnd.n	eq0,r2,splxcom
	add	r3,r0,ENB_CPI	; enable interrupt
	add	r3,r0,DSB_CPI	; disable interrupt
splxcom:
	st.b	r3,r4,lo16(CR0)
;endif	/* opus */
	or.u	r3,r0,hi16(_intlvl)
	tb1	0,r0,128
	xmem	r2,r3,lo16(_intlvl)

	tb1	0,r0,128	; wait for instruction to finish
	jmp.n 	r1
	stcr	r5,cr1		; restore saved PSR 

; intoff()
; intrestore()

_intoff: global	_intoff		; processor interrupts off
	ldcr	r2,cr1		; get psr
	set	r2,r2,1<PS_IPL>	; block interrupts
	stcr	r2,cr1
	or	r0,r0,r0	; let stcr take effect
	jmp 	r1

_inton:	global	_inton		; processor interrupts on
	tb1	0,r0,128	; wait 
	ldcr	r2,cr1		; get psr
	clr	r2,r2,1<PS_IPL>	; enable interrupts
	stcr	r2,cr1
	jmp 	r1

; idle()

	global	_waitloc    ; So clock can see if we were idle.
	global	_waitloc2    ; So clock can see if we were idle.
	global	_idle
_waitloc:
_idle:
	or.u	r3,r0,hi16(_intlvl)
	st	r0,r3,lo16(_intlvl)
;ifdef opus
	or.u	r4,r0,hi16(COMMPAGE)
	st.b	r0,r4,SPL
	st	r0,r4,SPLRET
	addu	r3,r0,ENB_CPI
	or.u	r4,r0,hi16(CR0)
	st.b	r3,r4,lo16(CR0)		; enable external interrupt
	tb1	0,r0,128		; wait for instruction to finish
;endif /* opus */
	ldcr	r3,cr1
	and	r3,r3,0xFFFD
	stcr	r3,cr1
idleloop:
	or.u	r3,r0,hi16(_idleflag)
	ld	r2,r3,lo16(_idleflag)	; Get idle flag value.
	bcnd.n	ne0,r2,idleloop	; Wait until there is something to do.
	add	r2,r0,1
	st	r2,r3,lo16(_idleflag)  ; set flag.
	jmp     r1
_waitloc2:

	global	interrupt
interrupt:
;
;   All interrupts initially come here.  From here they are vectored
; to the appropriate handler.
;
	stcr	r1,cr18			; Actually in vector table.

	ldcr	r1,cr8
	bb1	0,r1,intdflt		; Workaround

	stcr	r2,cr17			; save r2

	ldcr	r1,cr1
	and.u	r1,r1,0x9fff		; reset BO, SER
	and	r1,r1,0xfffb		; reset MXM
	stcr	r1,cr1

;ifdef opus
	add	r2,r0,(INT_CP+CLR)
	or.u	r1,r0,hi16(CR0)
	st.b	r2,r1,lo16(CR0)		; clr INT_CP

	or.u	r2,r0,hi16(COMMPAGE)
	ld.bu	r1,r2,lo16(NMI_FLAG)
	st.b	r0,r2,lo16(NMI_FLAG)

	ldcr	r2,cr17			; restore r2
	

	bb1	1,r1,gotparity		; Bit 1 Parity Error
;	bb1	0,r1,gotnmi		; Bit 0 NMI interrupt from kbd.
;endif

;####################################################################
; Clock interrupt
;
;   Control comes here whenever a clock interrupt occurs.
;   We need to call clock as follows:
;
;	clock(pc, ps);
;
;   The ps must be the ps of the machine before the interrupt.
;####################################################################

	text

;	global	Xclock
Xclock:
	or.u	r1,r0,hi16(_idleflag)	; Clear idleflag to terminate
	st	r0,r1,lo16(_idleflag)	; stop idle loop.

	bsr.n	getisps			; Save registers
	stcr	r31,cr17

	bsr	_spl7			; external ints off, cpu ints on
	bcnd	eq0,r2,splok
	bsr	_badspl
splok:

;ifdef opus
	or.u	r2,r0,hi16(COMMPAGE)
	ld.bu	r1,r2,PC_CKFLG
	bb0.n	B_S_DONE,r1,chkio
	add	r2,r0,r0

	or.u	r2,r0,hi16(COMMPAGE)
	ld	r2,r2,PC_CKCNT		; ticks elapsed
	bsr	_swapl			; 
	add	r4,r2,r0		; swapped m88k ticks

	ld	r3,r31,HOMESP+PS
	ld	r2,r31,HOMESP+SCIP	; Get IP of interrupted instruction.
	
	bsr	_clock			; Let clock(pc,ps) handle it.

chkio:
	st	r2,r31,HOMESP-4		; save return value from clock

;#if XBus
	or.u	r3,r0,hi16(SR3)
	ld.bu	r2,r3,lo16(SR3)
	mask	r2,r2,CP_XIRQ		; test XBus interrupt request
	bcnd	eq0,r2,noxbus
	bsr	_xbus_trap
noxbus:
;#endif

	or.u	r2,r0,hi16(COMMPAGE)
	ld.hu	r1,r2,PC_CPGET
	ld.hu	r2,r2,PC_CPPUT
	sub	r1,r1,r2
	bcnd	eq0,r1,noio

	ld	r2,r31,HOMESP+SCIP	; Get IP of interrupted instruction.
	ld	r3,r31,HOMESP+PS
	bsr	_pcintr

noio:
	ld	r2,r31,HOMESP-4		; return value from clock
	bcnd	eq0,r2,intret		; If return is 0, normal return.
;endif

;####################################################################
;
; At this point, clock returned a non-zero value and we need to run the user
; profiling (addupc).
;####################################################################


;  call addupc(PC,ignored_arg,ticks);
	or	r4,r0,r2		; return value from clock (ticks)
	ld	r2,r31,SCIP+HOMESP
	bsr.n	_addupc
	addu	r3,r0,r0		; ignored argument

	br	intret

; Got a parity error
	
;ifdef opus
gotparity:
	bsr	_parity
	
; The NMI button was pressed.

gotnmi:
	or.u	r1,r0,hi16(_dbgswab)	; Jump through swabint.
	ld	r1,r1,lo16(_dbgswab)
	jmp.n	r1
	ldcr	r1,cr18			; Restore r1.
;	NO RETURN
;endif

intdflt:
;  A data fault occurred while we were taking the interrupt.  We must
; save the state information on the master stack (including fault
; information and call buserr() to handle it.

	bsr.n	getmsps			; Switch stacks and save registers.
	stcr	r31,cr17
;
;   Note that getmsps reenables interrupts, thus the interrupt that
; got us here in the first place reoccured and was handled seperately,
; getmsps handled the pending bus error for us by calling buserr();
;
	br	getregs
;
	global	_breakpoint
_breakpoint:
;  Called from kernel on panic.
	st.d	r0,r0,PANICBASE+0x0
	st.d	r2,r0,PANICBASE+0x8
	st.d	r4,r0,PANICBASE+0x10
	st.d	r6,r0,PANICBASE+0x18
	st.d	r8,r0,PANICBASE+0x20
	st.d	r10,r0,PANICBASE+0x28
	st.d	r12,r0,PANICBASE+0x30
	st.d	r14,r0,PANICBASE+0x38
	st.d	r16,r0,PANICBASE+0x40
	st.d	r18,r0,PANICBASE+0x48
	st.d	r20,r0,PANICBASE+0x50
	st.d	r22,r0,PANICBASE+0x58
	st.d	r24,r0,PANICBASE+0x60
	st.d	r26,r0,PANICBASE+0x68
	st.d	r28,r0,PANICBASE+0x70
	st.d	r30,r0,PANICBASE+0x78
_breakpoint1:
	br	_breakpoint1

I 2
; kernel is about to fault, trigger logic analyser
	global	_triggerscope
_triggerscope:
	stcr	r2,cr17		; save r2
	ldcr	r2,cr8		; save cr8
	stcr	r2,cr20
	or.u	r2,r0,0xf001
	or	r2,r2,0x2345
	st	r2,r0,212
	ldcr	r2,cr20
	stcr	r2,cr8		; restore cr8
	ldcr	r2,cr17		; restore r2
	jmp	r1
I 3

	global	_r31
D 4
_r31:				; return sp
E 4
I 4
_r31:				; return sp + off
E 4
	jmp.n	r1
D 4
	or	r2,r0,r31
E 4
I 4
	add	r2,r2,r31
E 4
E 3
	

E 2
	data
	align	4
	global	DCMMUBASE
	global	CCMMUBASE
	global	_DCMMUBASE
	global	_CCMMUBASE
DCMMUBASE:
_DCMMUBASE:
	word	0xfff07000	; Data CMMU
CCMMUBASE:
_CCMMUBASE:
	word	0xfff06000	; Code CMMU
E 1

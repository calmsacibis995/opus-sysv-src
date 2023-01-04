h56329
s 00012/00041/00056
d D 1.2 90/04/18 08:58:30 root 2 1
c call triggerscope when kernel is about to page fault
e
s 00097/00000/00000
d D 1.1 90/03/06 12:46:20 root 1 0
c date and time created 90/03/06 12:46:20 by root
e
u
U
t
T
I 1
; @(#)paging.s	1.2
;	Protection  fault
	global	Xcaccflt
Xcaccflt:
;
;  Execution comes here when a fault on code access occurs.
;
D 2
	stcr	r1,cr18		; This is actually in vector table.
E 2
I 2
	stcr	r1,cr18	
E 2

D 2
#ifdef LOGICANALYSER
	ldcr	r1,cr8		; CPU BUG save cr8
	stcr	r1,cr20
	ldcr	r1,cr4
	and	r1,r1,0xfffc
	bcnd	ne0,r1,notvazero
	st	r0,r0,212
notvazero:
	ldcr	r1,cr20
	stcr	r1,cr8		; restore cr8 for getmsps
#endif
E 2
I 2
	ldcr	r1,cr2
	bb0	31,r1,notkva00
	bsr	_triggerscope
notkva00:
E 2

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
D 2
	stcr	r1,cr18		; This is actually in vector table.
#ifdef LOGICANALYSER
	ldcr	r1,cr8		; CPU BUG save cr8
	stcr	r1,cr20
ldcr	r1,cr18
st	r1,r0,216		; r1
ldcr	r1,cr4
st	r1,r0,216		; sxip
ldcr	r1,cr8
st	r1,r0,216		; dmt0
ldcr	r1,cr9
st	r1,r0,216		; dmd0
ldcr	r1,cr10
st	r1,r0,216		; dma0
ldcr	r1,cr11
st	r1,r0,216		; dmt1
ldcr	r1,cr12
st	r1,r0,216		; dmd1
ldcr	r1,cr13
st	r1,r0,216		; dma1
ldcr	r1,cr14
st	r1,r0,216		; dmt2
ldcr	r1,cr15
st	r1,r0,216		; dmd2
ldcr	r1,cr16
st	r1,r0,216		; dma2
	ldcr	r1,cr20
	stcr	r1,cr8		; restore cr8 for getmsps
#endif
E 2
I 2
	stcr	r1,cr18

	ldcr	r1,cr8
	bb0	14,r1,notkva01
	bsr	_triggerscope
notkva01:

E 2
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
E 1

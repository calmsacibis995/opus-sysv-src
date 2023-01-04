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

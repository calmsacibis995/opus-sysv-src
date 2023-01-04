h64817
s 00719/00000/00000
d D 1.1 90/03/06 12:46:22 root 1 0
c date and time created 90/03/06 12:46:22 by root
e
u
U
t
T
I 1

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
E 1

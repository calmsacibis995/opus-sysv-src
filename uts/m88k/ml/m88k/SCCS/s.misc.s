h50815
s 00003/00000/00426
d D 1.2 90/04/24 11:22:22 root 2 1
c added kstrcpy, which branches to strcpy (for dcc).
e
s 00426/00000/00000
d D 1.1 90/03/06 12:46:18 root 1 0
c date and time created 90/03/06 12:46:18 by root
e
u
U
t
T
I 1
;		Copyright (c) 1985 AT&T		#
;		All Rights Reserved		#
;	@(#)misc.s	1.0	
I 2
	global	_kstrcpy
_kstrcpy:
	br	_strcpy
E 2
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
E 1

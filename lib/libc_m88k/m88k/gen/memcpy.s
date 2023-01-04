; memcpy(to, from, size)
;	Copy a block of memory from "from" to "to".  The number
;	of bytes to copy is "size".  
;	bcopy() makes no assumption about the alignment of the 
;	addresses.
;	blkcopy() assumes that "from" and "to" start on long word
;	boundaries.  blkcopy() is slightly faster than bcopy().

	global	__memcpy
	global	dmemcpy
__memcpy:	
dmemcpy:
	extu	r5,r4,30<2>		; Calculate number of longs, if its
	bcnd.n	eq0,r5,bcopyb		; r0, go copy it as bytes.
	or	r8,r2,r0		; Save r2 for return value
;
;  In the case that aligment of both from and to is impossible, the next best
; alternative is to align "to" to a long word boundary since writes are always
; written back even for byte or half word updates (On the ASPEN).  

	bb0	0,r2,bcalh		; Align to the next half word boundary.
	ld.b	r5,r3,0
	add	r3,r3,1
	st.b	r5,r2,0
	add	r2,r2,1
	sub	r4,r4,1

bcalh:	bb0	1,r2,blktoaln		; Align to the next long word boundary.
	bb0	0,r3,blktohfs		; If from is not on a half word boundary,


	ld.bu	r6,r3,0			; Copy a two bytes to the next half word.
	ld.bu	r5,r3,1			; to will then be on a long word boundary.
	mak	r6,r6,8<8>		; Shift up 8 bits.
	or	r5,r6,r5
	st.h	r5,r2,0
	add	r3,r3,2
	add	r2,r2,2
	sub	r4,r4,2
;  copy longs piecemeal. In this case, to is on an even address but from is on an odd
; address.  In this case, alignment of both is impossible.
bcopypc:
	extu	r7,r4,30<2>		; Recalculate the number of longs
	bcnd	eq0,r7,bcopyb		; (it might have changed) and then
bcopybhb:
	ld.b	r5,r3,0			; Copy 2 bytes and a half word.
	ld.hu	r6,r3,1
	mak	r5,r5,8<16>		; Shift up 16 bits.
	or	r5,r6,r5
	ld.bu	r6,r3,3
	mak	r5,r5,24<8>		; Shift up 8 bits.
	or	r5,r6,r5
	st	r5,r2,0
	add	r3,r3,4
	add	r2,r2,4
	sub	r7,r7,1
	bcnd	ne0,r7,bcopybhb
	br	bcopyb

blktohfs:
;  Alignment of "to" is possible using a half word.
	ld.hu	r5,r3,0
	add	r3,r3,2
	st.h	r5,r2,0
	add	r2,r2,2
	sub	r4,r4,2
blktoaln:
;  At this point "to" is aligned on a long word boundary.  If "from" is
; also aligned, we can copy longs, otherwise, we have to load half words.
;
;					  
	bb1	0,r3,bcopypc		; If odd, copy piecemeal.
	bb1	1,r3,bcopyh		; Do we have to move halfs?

blkcopy:
	extu	r5,r4,30<2>		; Recalculate the number of longs
	bcnd	eq0,r5,bcopyb		; (it might have changed) and then
bcopylp:
	ld	r6,r3,0
	add	r3,r3,4
	sub	r5,r5,1
	st	r6,r2,0			; Copy the next long.
	bcnd.n	ne0,r5,bcopylp
	add	r2,r2,4
	br	bcopyb			; Go copy the remainder as bytes.

bcopyh:
	extu	r5,r4,30<2>		; Recalculate the number of longs
	bcnd	eq0,r5,bcopyb		; (it might have changed) and then
;  copy longs in half words. 
bcopyhlp:
	ld.hu	r7,r3,0			; Copy a two bytes to the next half word.
	ld.hu	r6,r3,2
	mak	r7,r7,16<16>
	or	r7,r6,r7
	st	r7,r2,0
	add	r3,r3,4
	sub	r5,r5,1
	bcnd.n	ne0,r5,bcopyhlp
	add	r2,r2,4

bcopyb:
	mask	r4,r4,3
	bcnd	eq0,r4,L%done
bcopyblp:
	ld.b	r6,r3,0
	st.b	r6,r2,0			; Copy the next long.
	sub	r4,r4,1
	add	r2,r2,1
	bcnd.n	ne0,r4,bcopyblp
	add	r3,r3,1

L%done:
	jmp.n   r1
	or	r2,r8,r0		; return destination


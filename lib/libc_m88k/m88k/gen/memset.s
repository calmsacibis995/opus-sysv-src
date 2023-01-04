;	@(#)__memset.s	6.1	
;
;	M78000 __memset(3C) Routine
;
;	(C) Copyright 1987 by Motorola Inc.
;
;	Written by: Bob Greiner
;
; __memset - copies ln characters equal to char to dsave, and returns dsave
;
;	This routine works in Big Endian mode only.
;
; Input:	dsave - destination
;		char  - source
;		ln    - number of bytes to copy
;
; Output:	dsave
;
; Algorithm:
;
;	This routine has been hand microscheduled and branch
;	delayed.  Be extremely carefull in changing, especially
;	the word after a bcnd.n instruction.
;
;	This routine is carefully optimized to move full words
;	whenever possible.
;
;	The aligned word loop takes 3 ticks per word, assuming
;	cache hits on write, and a copy back cache.
;	The bubble after the branch is overlapped with the increment.
;	This inner loop speed is obtained with considerable overhead
;	calculation.
;
;	The optimization of the beginning and ending byte moves
;	to just use one store was not judged worthwhile
;	because of the large number of special cases.
;
;	This routine is designed to access data only within the
;	words containing the dsave string, to avoid extraneous
;	memory faults.
;
; __memset (dsave, char, ln)
;
;	dest = dsave
;	char &= 0xFF
;	char |= char << 8
;	char |= char << 16
;	while ln>0 && dest&3 != 0 loop
;		-- move until word boundary
;		dest->byte = char
;		dest += 1
;		ln -= 1
;	end loop
;	ln -= 4
;	if ln>=0 then
;		-- optimized word move
;		lw = ln & ~3
;		lx = -lw
;		assert lx<=0
;		dest -= lx + 4
;		loop
;			(dest + lx)->word = char
;			lx += 4
;		when lx-4 >= 0 exit
;		end loop
;		assert lx==4
;		dest += 8
;		ln -= lw 
;		ln -= 4
;	end if
;	ln += 4
;	while ln>0 loop
;		-- move from word boundary
;		dest->byte = char
;		dest += 1
;		ln -= 1
;	end loop
;	return dsave
;
; Register usage:
;
_m4_define_(`dsave', r2)_m4_dnl_		; destination pointer; in and function value
_m4_define_(`char', r3)_m4_dnl_		; source pointer
_m4_define_(`ln', r4)_m4_dnl_		; length to copy in bytes
_m4_define_(`dest', r5)_m4_dnl_		; working destination pointer
_m4_define_(`lw', r6)_m4_dnl_		; number of words to move
_m4_define_(`lx', r7)_m4_dnl_		; negative word counter
_m4_define_(`temp', r8)_m4_dnl_		; utility register
;
	file	"__memset.s"
	text
	global	__memset
__memset:
	MCOUNT
	bcnd	le0,ln,done		; unroll loop
	mask	char,char,0xFF		; make word of four chars
	rot	temp,char,24		; make two chars
	or	char,char,temp
	rot	temp,char,16		; make four chars
	or	char,char,temp		; char = four char's
	mask	temp,dsave,3		; unroll loop
	bcnd.n	eq0,temp,beg_end	; unroll loop
	or	dest,r0,dsave		; dest = dsave
beg:					; while ln>0 && dest&3 != 0 loop
	st.b	char,dest,0		; dest->byte = char
	addu	dest,dest,1		; dest += 1
	subu	ln,ln,1			; ln -= 1
	bcnd	le0,ln,done
	mask	temp,dest,3
	bcnd	ne0,temp,beg
beg_end:				; end loop
	subu	ln,ln,4			; ln -= 4
	bcnd	lt0,ln,no_word		; if ln>=0 then
					; -- optimized word move
	and	lw,ln,0xFFFC		; lw = ln & ~3
	subu	lx,r0,lw		; lx = -lw
					; assert lx<=0
	subu	dest,dest,lx		; dest -= lx + 4
	subu	dest,dest,4
	bcnd.n	ge0,lx,aligned_end	; unroll loop
	addu	lx,lx,4			; unroll loop
aligned:				; loop
	st	char,dest,lx		; (dest + lx)->word = temp
	bcnd.n	lt0,lx,aligned		; lx += 4
	addu	lx,lx,4			; when lx-4 >= 0 exit
aligned_end:				; end loop
	st	char,dest,lx		; unroll loop
					; assert lx==4
	addu	dest,dest,8		; dest += 8
	subu	ln,ln,lw		; ln -= lw
	subu	ln,ln,4			; ln -= 4
no_word:				; end if ln>=0
	addu	ln,ln,4			; ln += 4
	bcnd	le0,ln,ending_end	; unroll loop
ending:					; while ln>0 loop
	subu	ln,ln,1			; ln -= 1
	st.b	char,dest,0		; dest->byte = char
	bcnd.n	gt0,ln,ending		; dest += 1
	addu	dest,dest,1		; when ln<=0 exit
ending_end:				; end loop
done:					; return dsave
	jmp	return

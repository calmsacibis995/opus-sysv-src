;	@(;)mcount.s	6.1 
; count subroutine called during profiling

	file	"mcount.s"
	global	mcount
	global	__countbase

;	r1 = return link
;	r2 = pointer to pointer to counter
;
	text
mcount:
	ld	r10,r2,r0		; r10 = pointer to counter
	bcnd	eq0,r10,minit		; -first time through
	ld	r11,r10,r0		; r11 = counter
	addu	r11,r11,1		; ++counter
	jmp.n	r1			; return
	st	r11,r10,r0

minit:
	or.u	r10,r0,hi16(__countbase)
	ld	r11,r10,lo16(__countbase)
	bcnd	eq0,r11,mexit		; -no count buffer
	st	r1,r11,0		; save pc
	st	r0,r11,4		; zero counter
	addu	r11,r11,4		; make pointer to counter
	st	r11,r2,r0		; save it
	addu	r11,r11,4
	br.n	mcount			; do it over right
	st	r11,r10,lo16(__countbase)

mexit:
	jmp	r1

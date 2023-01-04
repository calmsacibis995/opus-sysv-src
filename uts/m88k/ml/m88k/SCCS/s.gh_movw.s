h24180
s 00268/00000/00000
d D 1.1 90/03/06 12:46:16 root 1 0
c date and time created 90/03/06 12:46:16 by root
e
u
U
t
T
I 1
	text
	global	__gh_mvb
__gh_mvb:
	and	r10,r2,3

	bcnd.n	13,r10,@L31	; ne
	and	r10,r3,3

	bcnd.n	13,r10,@L31	; ne
	and	r7,r4,3		; offset = 0; length = 2

	bcnd.n	2,r7,__gh_mvw	; eq
	extu	r4,r4,2

	cmp	r10,r7,2

;	bb1.n	gt,r10,@L50
;	lda	r8,r2[r4]
	lda	r8,r2[r4]
	bb1	gt,r10,@L50

;	bb1.n	lt,r10,@L51
;	lda	r9,r3[r4]
	lda	r9,r3[r4]
	bb1	lt,r10,@L51

	ld.h	r5,r8,0

;	br.n	__gh_mvw
;	st.h	r5,r9,0
	st.h	r5,r9,0
	br	__gh_mvw
@L51:
	ld.b	r5,r8,0

;	br.n	__gh_mvw
;	st.b	r5,r9,0
	st.b	r5,r9,0
	br	__gh_mvw

@L50:
	ld.h	r5,r8,0
	ld.b	r6,r8,2
	lda	r9,r3[r4]
	st.h	r5,r9,0

;	br.n	__gh_mvw
;	st.b	r6,r9,2
	st.b	r6,r9,2
	br	__gh_mvw

@L31:
	bcnd	2,r4,@L3	; eq
@L4:
	ld.b	r6,r2,0
	sub	r4,r4,1
	st.b	r6,r3,0

;	bcnd.n	2,r4,@L3	; eq
;	ld.b	r6,r2,1
	ld.b	r6,r2,1
	bcnd	2,r4,@L3	; eq

	sub	r4,r4,1
	st.b	r6,r3,1

;	bcnd.n	2,r4,@L3	; eq
;	ld.b	r6,r2,2
	ld.b	r6,r2,2
	bcnd	2,r4,@L3	; eq

	sub	r4,r4,1
	st.b	r6,r3,2

;	bcnd.n	2,r4,@L3	; eq
;	ld.b	r6,r2,3
	ld.b	r6,r2,3
	bcnd	2,r4,@L3	; eq

	sub	r4,r4,1
	st.b	r6,r3,3
	add	r2,r2,4

	bcnd.n	13,r4,@L4	; ne
	add	r3,r3,4

@L3:
	jmp	r1
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	text
	global	__gh_mvh
__gh_mvh:
	and	r10,r2,1

	bcnd.n	13,r10,@L20	; ne
	and	r10,r3,1

	bcnd.n	13,r10,@L20	; ne
	and	r7,r4,1		; offset = 0; length = 2

	bcnd.n	2,r7,__gh_mvw	; eq
	extu	r4,r4,1

	ld.h	r5,r2[4]

;	br.n	__gh_mvw
;	st.h	r5,r3[r4]
	st.h	r5,r3[r4]
	br	__gh_mvw

@L20:
	bcnd	2,r4,@L13	; eq
@L14:
	ld.h	r6,r2,0
	sub	r4,r4,1
	st.h	r6,r3,0

;	bcnd.n	2,r4,@L13	; eq
;	ld.h	r6,r2,2
	ld.h	r6,r2,2
	bcnd	2,r4,@L13	; eq

	sub	r4,r4,1
	st.h	r6,r3,2

;	bcnd.n	2,r4,@L13	; eq
;	ld.h	r6,r2,4
	ld.h	r6,r2,4
	bcnd	2,r4,@L13	; eq

	sub	r4,r4,1
	st.h	r6,r3,4

;	bcnd.n	2,r4,@L13	; eq
;	ld.h	r6,r2,6
	ld.h	r6,r2,6
	bcnd	2,r4,@L13	; eq

	sub	r4,r4,1
	st.h	r6,r3,6
	add	r2,r2,8

	bcnd.n	13,r4,@L14	; ne
	add	r3,r3,8

@L13:
	jmp	r1
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	text
	global	__gh_mvw
__gh_mvw:
	extu	r13,r4,2

;	bcnd.n	2,r13,@L1	; eq
;@L2:
;	ld	r5,r2,0
;	ld	r5,r2,0		; removed by saj
	bcnd	2,r13,@L1	; eq
@L2:
	ld	r5,r2,0

	ld	r6,r2,4
	ld	r7,r2,8
	ld	r8,r2,12
	st	r5,r3,0
	st	r6,r3,4
	st	r7,r3,8
	sub	r13,r13,1

;	bcnd.n	2,r13,@L61	; eq
;	st	r8,r3,12
	st	r8,r3,12
	bcnd	2,r13,@L61	; eq

	ld	r5,r2,16
	ld	r6,r2,20
	ld	r7,r2,24
	ld	r8,r2,28
	st	r5,r3,16
	st	r6,r3,20
	st	r7,r3,24
	sub	r13,r13,1

;	bcnd.n	2,r13,@L62	; eq
;	st	r8,r3,28
	st	r8,r3,28
	bcnd	2,r13,@L62	; eq

	ld	r5,r2,32
	ld	r6,r2,36
	ld	r7,r2,40
	ld	r8,r2,44
	st	r5,r3,32
	st	r6,r3,36
	st	r7,r3,40
	sub	r13,r13,1

;	bcnd.n	2,r13,@L63	; eq
;	st	r8,r3,44
	st	r8,r3,44
	bcnd	2,r13,@L63	; eq

	ld	r5,r2,48
	ld	r6,r2,52
	ld	r7,r2,56
	ld	r8,r2,60
	st	r5,r3,48
	st	r6,r3,52
	st	r7,r3,56
	st	r8,r3,60
	add	r2,r2,64
	sub	r13,r13,1

;	ld	r5,r2,0		; added by saj
	bcnd.n	13,r13,@L2	; ne
	add	r3,r3,64

@L1:
	and	r4,r4,3

;	bcnd.n	2,r4,@L6	; eq
;	ld	r5,r2,0
	ld	r5,r2,0
	bcnd	2,r4,@L6	; eq

	cmp	r10,r4,2

;	bb1.n	gt,r10,@L40
;	st	r5,r3,0
	st	r5,r3,0
	bb1	gt,r10,@L40

;	bb1.n	lt,r10,@L6
;	ld	r5,r2,4
	ld	r5,r2,4
	bb1	lt,r10,@L6

;	jmp.n	r1
;	st	r5,r3,4
	st	r5,r3,4
	jmp	r1

@L40:
	ld	r6,r2,4
	ld	r7,r2,8
	st	r6,r3,4

;	jmp.n	r1
;	st	r7,r3,8
	st	r7,r3,8
	jmp	r1

@L61:
	addu	r2,r2,16
	addu	r3,r3,16
	br	@L1
@L62:
	addu	r2,r2,32
	addu	r3,r3,32
	br	@L1
@L63:
	addu	r2,r2,48
	addu	r3,r3,48
	br	@L1
@L6:
	jmp	r1

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
E 1

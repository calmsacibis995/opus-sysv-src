h44881
s 00028/00000/00000
d D 1.1 90/03/06 12:46:13 root 1 0
c date and time created 90/03/06 12:46:13 by root
e
u
U
t
T
I 1
	text
	global	_bcopy16
_bcopy16:
	bcnd	eq0,r4,_bout16
_bcopy16t:
	ld.d	r6,r2,0x0000                    
	ld.d	r8,r2,0x0008                    
	subu	r4,r4,0x0010                    
	addu	r2,r2,0x0010                    
	st.d	r6,r3,0x0000                    
	st.d	r8,r3,0x0008                    
	bcnd.n	ne0,r4,_bcopy16t
	addu	r3,r3,0x0010                    
_bout16:
	jmp	r1

	global	_bzero16
_bzero16:
	bcnd	eq0,r3,_bout16
	or	r4,r0,r0
	or	r5,r0,r0
bzero16t:
	subu	r3,r3,0x0010                    
	st.d	r4,r2,0x0000                    
	st.d	r4,r2,0x0008                    
	bcnd.n	ne0,r3,bzero16t
	addu	r2,r2,0x0010                    
	jmp	r1
E 1


text
global __strcpy
;global dmemcpy
__strcpy:
	MCOUNT
	or          r13,r2,r3                       
	mask        r13,r13,0x0003                  
	bcnd.n      ne0,r13,copybyte               
	addu        r4,r0,r0                        
	ld          r5,r3,0x0000                    
	addu        r3,r3,0x0004                    
	or.u        r8,r0,0x8080                    
	or          r8,r8,0x8080                    
	rot         r9,r8,0<7>                      
	subu        r13,r5,r9                       
again:
	and.c       r13,r13,r5                      
	and         r13,r8,r13                      
	bcnd        ne0,r13,findzero               
	st          r5,r2,r4                        
	ld          r5,r3,r4                        
	addu        r4,r4,0x0004                    
	br.n        again                          
	subu        r13,r5,r9                       
findzero:
	addu        r4,r4,r2                        
	ext         r13,r5,8<24>                    
	bcnd.n      eq0,r13,wordout                
	st.b        r13,r4,0x0000                   
	ext         r13,r5,8<16>                    
	bcnd.n      eq0,r13,wordout                
	st.b        r13,r4,0x0001                   
	ext         r13,r5,8<8>                     
	bcnd.n      eq0,r13,wordout                
	st.b        r13,r4,0x0002                   
	jmp.n       r1                              
	st.b        r5,r4,0x0003                    
wordout:
	jmp         r1                              
membcpy:
	addu        r4,r0,r0                        
copybyte:
	subu        r2,r2,0x0001                    
bytecpy:
	ld.b        r13,r3,r4                       
	addu        r4,r4,0x0001                    
	bcnd.n      ne0,r13,bytecpy                
	st.b        r13,r2,r4                       
	jmp.n       r1                              
	addu        r2,r2,0x0001                    
dmemcpy:
	or          r13,r2,r3                       
	mask        r13,r13,0x0003                  
	bcnd.n      ne0,r13,membcpy                
	addu        r5,r2,r0                        
	br.n        mem16                          
	cmp         r6,r4,0x0010                    
mem16t:
	ld          r6,r3,0x0000                    
	ld          r7,r3,0x0004                    
	ld          r8,r3,0x0008                    
	ld          r9,r3,0x000c                    
	st          r6,r5,0x0000                    
	st          r7,r5,0x0004                    
	st          r8,r5,0x0008                    
	st          r9,r5,0x000c                    
	addu        r3,r3,0x0010                    
	addu        r5,r5,0x0010                    
	subu        r4,r4,0x0010                    
	cmp         r6,r4,0x0010                    
mem16:
	bb1.n       11,r6,mem16t                   
	addu        r6,r4,0x0001                    
	addu        r10,r1,r0                       
	bsr.n       memcase                        
	lda.d       r1,r1[r6]                       
memcase:
	jmp.n       r1                              
	addu        r1,r10,r0                       
m0:
	jmp         r1                              
	addu        r3,r3,r0                        
m1:
	br.n        mem1                           
	ld.b        r6,r3,0x0000                    
m2:
	br.n        mem2                           
	ld.h        r6,r3,0x0000                    
m3:
	br.n        mem3                           
	ld.h        r6,r3,0x0000                    
m4:
	br.n        mem4                           
	ld          r6,r3,0x0000                    
m5:
	br.n        mem5                           
	ld          r6,r3,0x0000                    
m6:
	br.n        mem6                           
	ld          r6,r3,0x0000                    
m7:
	br.n        mem7                           
	ld          r6,r3,0x0000                    
m8:
	br.n        mem8                           
	ld          r6,r3,0x0000                    
m9:
	br.n        mem9                           
	ld          r6,r3,0x0000                    
m10:
	br.n        mem10                          
	ld          r6,r3,0x0000                    
m11:
	br.n        mem11                          
	ld          r6,r3,0x0000                    
m12:
	br.n        mem12                          
	ld          r6,r3,0x0000                    
m13:
	br.n        mem13                          
	ld          r6,r3,0x0000                    
m14:
	br.n        mem14                          
	ld          r6,r3,0x0000                    
m15:
	ld          r6,r3,0x0000                    
	ld          r7,r3,0x0004                    
	ld          r8,r3,0x0008                    
	ld.h        r9,r3,0x000c                    
	ld.b        r10,r3,0x000e                   
	st          r6,r5,0x0000                    
	st          r7,r5,0x0004                    
	st          r8,r5,0x0008                    
	st.h        r9,r5,0x000c                    
	jmp.n       r1                              
	st.b        r10,r5,0x000e                   
mem1:
	jmp.n       r1                              
	st.b        r6,r5,0x0000                    
mem2:
	jmp.n       r1                              
	st.h        r6,r5,0x0000                    
mem3:
	ld.b        r7,r3,0x0002                    
	st.h        r6,r5,0x0000                    
	jmp.n       r1                              
	st.b        r7,r5,0x0002                    
mem4:
	jmp.n       r1                              
	st          r6,r5,0x0000                    
mem5:
	ld.b        r7,r3,0x0004                    
	st          r6,r5,0x0000                    
	jmp.n       r1                              
	st.b        r7,r5,0x0004                    
mem6:
	ld.h        r7,r3,0x0004                    
	st          r6,r5,0x0000                    
	jmp.n       r1                              
	st.h        r7,r5,0x0004                    
mem7:
	ld.h        r7,r3,0x0004                    
	ld.b        r8,r3,0x0006                    
	st          r6,r5,0x0000                    
	st.h        r7,r5,0x0004                    
	jmp.n       r1                              
	st.b        r8,r5,0x0006                    
mem8:
	ld          r7,r3,0x0004                    
	st          r6,r5,0x0000                    
	jmp.n       r1                              
	st          r7,r5,0x0004                    
mem9:
	ld          r7,r3,0x0004                    
	ld.b        r8,r3,0x0008                    
	st          r6,r5,0x0000                    
	st          r7,r5,0x0004                    
	jmp.n       r1                              
	st.b        r8,r5,0x0008                    
mem10:
	ld          r7,r3,0x0004                    
	ld.h        r8,r3,0x0008                    
	st          r6,r5,0x0000                    
	st          r7,r5,0x0004                    
	jmp.n       r1                              
	st.h        r8,r5,0x0008                    
mem11:
	ld          r7,r3,0x0004                    
	ld.h        r8,r3,0x0008                    
	ld.b        r9,r3,0x000a                    
	st          r6,r5,0x0000                    
	st          r7,r5,0x0004                    
	st.h        r8,r5,0x0008                    
	jmp.n       r1                              
	st.b        r9,r5,0x000a                    
mem12:
	ld          r7,r3,0x0004                    
	ld          r8,r3,0x0008                    
	st          r6,r5,0x0000                    
	st          r7,r5,0x0004                    
	jmp.n       r1                              
	st          r8,r5,0x0008                    
mem13:
	ld          r7,r3,0x0004                    
	ld          r8,r3,0x0008                    
	ld.b        r9,r3,0x000c                    
	st          r6,r5,0x0000                    
	st          r7,r5,0x0004                    
	st          r8,r5,0x0008                    
	jmp.n       r1                              
	st.b        r9,r5,0x000c                    
mem14:
	ld          r7,r3,0x0004                    
	ld          r8,r3,0x0008                    
	ld.h        r9,r3,0x000c                    
	st          r6,r5,0x0000                    
	st          r7,r5,0x0004                    
	st          r8,r5,0x0008                    
	jmp.n       r1                              
	st.h        r9,r5,0x000c                    
	or          r0,r0,r0                        

text
global __strcmp
__strcmp:
	MCOUNT
	or          r13,r2,r3                       
	mask        r13,r13,0x0003                  
	bcnd.n      ne0,r13,bytecmp                
	addu        r4,r0,r0                        
	or.u        r8,r0,0x8080                    
	or          r8,r8,0x8080                    
	br.n        wordcmp                        
	rot         r9,r8,0<7>                      
again:
	and         r13,r8,r13                      
	bcnd.n      ne0,r13,wordne                 
	addu        r4,r4,0x0004                    
wordcmp:
	ld          r5,r3,r4                        
	ld          r6,r2,r4                        
	subu        r13,r5,r9                       
	subu        r7,r6,r5                        
	bcnd.n      eq0,r7,again                   
	and.c       r13,r13,r5                      
	and         r13,r8,r13                      
	bcnd        ne0,r13,wordne                 
	jmp.n       r1                              
	subu        r2,r6,r5                        
wordne:
	ext         r13,r6,8<24>                    
	ext         r11,r5,8<24>                    
	bcnd.n      eq0,r13,out                    
	subu        r12,r13,r11                     
	bcnd.n      ne0,r12,out                    
	ext         r13,r6,8<16>                    
	ext         r11,r5,8<16>                    
	bcnd.n      eq0,r13,out                    
	subu        r12,r13,r11                     
	bcnd.n      ne0,r12,out                    
	ext         r13,r6,8<8>                     
	ext         r11,r5,8<8>                     
	bcnd.n      eq0,r13,out                    
	subu        r12,r13,r11                     
	bcnd.n      ne0,r12,out                    
	ext         r13,r6,8<0>                     
	ext         r11,r5,8<0>                     
	jmp.n       r1                              
	subu        r2,r13,r11                      
byteagain:
	bcnd        ne0,r12,out                    
bytecmp:
	ld.b        r13,r2,r4                       
	ld.b        r11,r3,r4                       
	addu        r4,r4,0x0001                    
	bcnd.n      ne0,r13,byteagain              
	subu        r12,r13,r11                     
out:
	jmp.n       r1                              
	addu        r2,r12,r0                       

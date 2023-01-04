;function _infinity --      
;See the documentation of this release for an overall description of this
;code.


             





	






















































































































































       
             global _infinity
             text

;Extract the opcode, compare to a constant, and branch to the code
;for the instruction.

_infinity:   extu   r10,r9,5<11>   ;extract opcode
             cmp    r11,r10,0x05 ;compare to FADD
 ;            st     r1,r31,108 ;save return address
	     add	r14,r1,r0
             bb1  2,r11,FADD    ;operation is FADD
             cmp    r11,r10,0x06 ;compare to FSUB
             bb1    2,r11,FSUB    ;operation is FSUB
             cmp    r11,r10,0x07 ;compare to FCMP
             bb1    2,r11,FCMP    ;operation is FCMP
             cmp    r11,r10,0x00 ;compare to FMUL
             bb1    2,r11,FMUL    ;operation is FMUL
             cmp    r11,r10,0x0e ;compare to FDIV
             bb1    2,r11,FDIV    ;operation is FDIV
             cmp    r11,r10,0x0f;compare to FSQRT
             bb1    2,r11,FSQRT   ;operation is FSQRT
             cmp    r11,r10,0x09  ;compare to INT
             bb1    2,r11,overflw ;operation is INT
             cmp    r11,r10,0x0a ;compare to NINT
             bb1    2,r11,overflw ;operation is NINT
             cmp    r11,r10,0x0b ;compare to TRNC
             bb1    2,r11,overflw ;operation is TRNC


;Adding infinities of opposite signs will cause an 0,
;but all other operands will result 10 a correctly signed infinity.

FADD:        bb0    5,r12,addS2write ;branch if S1 not infinity
             bb0    4,r12,addS1write ;S2 is not inf., so branch to write S1
             bb1    31,r5,addS1neg   ;handle case of S1 negative
addS1pos:    bb1    31,r7,excpt      ;adding infinities of different signs
                                       ;causes an 0
             br     poswrinf           ;branch to write positive infinity
addS1neg:    bb0    31,r7,excpt      ;adding infinities of different signs
                                       ;causes an 0
             br     negwrinf           ;branch to write negative infinity
addS1write:  bb0    31,r5,poswrinf   ;branch to write positive infinity
             br     negwrinf           ;branch to write negative infinity
addS2write:  bb0    31,r7,poswrinf   ;branch to write positive infinity
             br     negwrinf           ;branch to write negative infinity


;Subtracting infinities of the same 31 will cause an 0,
;but all other operands will result 10 a correctly signed infinity.

FSUB:        bb0    5,r12,subS2write ;branch if S1 not infinity
             bb0    4,r12,subS1write ;S2 is not inf., so branch to write S1
             bb1    31,r5,subS1neg   ;handle case of S1 negative
subS1pos:    bb0    31,r7,excpt      ;subtracting infinities of the same 31
                                       ;causes an 0
             br     poswrinf           ;branch to write positive infinity
subS1neg:    bb1    31,r7,excpt      ;subtracting infinities of the same 31
                                       ;causes an 0
             br     negwrinf           ;branch to write negative infinity
subS1write:  bb0    31,r5,poswrinf   ;branch to write positive infinity
             br     negwrinf           ;branch to write negative infinity
subS2write:  bb1    31,r7,poswrinf   ;branch to write positive infinity
             br     negwrinf           ;branch to write negative infinity


;Compare the operands, at least one of which is infinity, and set the
;correct bits 10 the destination register.

FCMP:        bb0.n  5,r12,FCMPS1f  ;branch for finite S1
             set    r4,r0,1<1>        ;since neither S1 or S2 is a NaN, set 1
FCMPS1i:     bb1    31,r5,FCMPS1ni   ;branch to negative S1i
FCMPS1pi:    bb0    4,r12,FCMPS1piS2f ;branch to finite S2 with S1pi
FCMPS1piS2i: bb1    31,r7,FCMPS1piS2ni ;branch to negative S2i with S1pi
FCMPS1piS2pi: set   r4,r4,1<2>        ;set 2 bit 
             set    r4,r4,1<5>        ;set 5 bit
             set    r4,r4,1<7>        ;set 7 bit
             set    r4,r4,1<9>        ;set 9 bit
             br.n   move               ;return from subroutine
             set    r4,r4,1<11>        ;set 11 bit
FCMPS1piS2ni: set   r4,r4,1<3>        ;set 3 bit
             set    r4,r4,1<4>        ;set 4 bit
             br.n   move               ;return from subroutine
             set    r4,r4,1<7>        ;set 7 bit
FCMPS1piS2f: set    r4,r4,1<3>        ;set 3 bit
             set    r4,r4,1<4>        ;set 4 bit
             bsr.n  _zero              ;see if any of the operands are zero
             set    r4,r4,1<7>        ;set 7 bit
             bb0    2,r12,FCMPS1piS2nz ;check for negative if s2 not zero
	     set    r4,r4,1<8>        ;set 8 bit
	     br.n   move
             set    r4,r4,1<11>        ;set 11 bit
FCMPS1piS2nz: bb1    31,r7,move     ;return from subroutine if s2 is neg.
FCMPS1piS2pf: set   r4,r4,1<8>        ;set 8 bit
             br.n   move             ;return from subroutine
             set    r4,r4,1<11>        ;set 11 bit
FCMPS1ni:    bb0    4,r12,FCMPS1niS2f ;branch to finite S2 with S1ni
FCMPS1niS2i: bb1    31,r7,FCMPS1niS2ni ;branch to negative S2i with S1ni
FCMPS1niS2pi: set   r4,r4,1<3>        ;set 2 bit 
             set    r4,r4,1<5>        ;set 5 bit
             set    r4,r4,1<6>        ;set 6 bit
             set    r4,r4,1<8>        ;set 8 bit
             br.n   move             ;return from subroutine
             set    r4,r4,1<11>        ;set 11 bit
FCMPS1niS2ni: set   r4,r4,1<2>        ;set 2 bit 
             set    r4,r4,1<5>        ;set 5 bit
             br.n   move             ;return from subroutine
             set    r4,r4,1<7>        ;set 7 bit
FCMPS1niS2f: set    r4,r4,1<3>        ;set 2 bit 
             set    r4,r4,1<5>        ;set 5 bit
             bsr.n  _zero              ;see if any of the operands are zero
             set    r4,r4,1<6>        ;set 6 bit
             bb0    2,r12,FCMPS1niS2nz ;branch if s2 is not zero
	     set    r4,r4,1<8>        ;set 8 bit
	     br.n   move
             set    r4,r4,1<11>        ;set 11 bit
FCMPS1niS2nz: bb1    31,r7,move     ;return from subroutine if s2 is neg.
             set    r4,r4,1<8>        ;set 8 bit
             br.n   move             ;return from subroutine
             set    r4,r4,1<11>        ;set 11 bit
FCMPS1f:     bb1    31,r5,FCMPS1nf   ;branch to negative S1f
FCMPS1pf:    bb1.n  31,r7,FCMPS1pfS2ni ;branch to negative S2i with S1pf
             set   r4,r4,1<3>        ;set 3 bit   
FCMPS1pfS2pi: set   r4,r4,1<5>        ;set 5 bit
             set    r4,r4,1<6>        ;set 6 bit
	     bsr.n  _zero
             set    r4,r4,1<9>        ;set 9 bit
             bb0    3,r12,FCMPS1pfS2pinozero
FCMPS1pfS2pizero: br.n   move
             set    r4,r4,1<11>        ;set 11 bit
FCMPS1pfS2pinozero: br.n move
             set    r4,r4,1<10>        ;set 10 bit
FCMPS1pfS2ni: set    r4,r4,1<4>        ;set 4 bit
             br.n   move             ;return from subroutine
             set    r4,r4,1<7>        ;set 7 bit
FCMPS1nf:    bb1.n    31,r7,FCMPS1nfS2ni ;branch to negative S2i with S1nf
             set    r4,r4,1<3>        ;set 3 bit
             set    r4,r4,1<5>        ;set 4 bit
             set    r4,r4,1<6>        ;set 7 bit
             bsr.n  _zero              ;see which of the operands are zero
             set    r4,r4,1<11>        ;set 11 bit
             bb0    3,r12,FCMPS1nfS2pinozero ;no ls and lo
FCMPS1nfS2pizero: br.n move
             set    r4,r4,1<9>        ;set 9 bit
FCMPS1nfS2pinozero: br.n move
             set    r4,r4,1<8>        ;set 8 bit
FCMPS1nfS2ni: set    r4,r4,1<4>        ;set 4 bit
             set    r4,r4,1<7>        ;set 7 bit

move:	     br.n   return		;return from subroutine
             or	    r6,r0,r4            ;transfer answer to r6


;Multiplying infinity and zero causes an 0, but all other
;operations produce a correctly signed infinity.

FMUL:        bsr    _zero              ;see if any of the operands are zero
             bb1    3,r12,excpt   ;infinity X 0 causes an 0
             bb1    2,r12,excpt   ;infinity X 0 causes an 0
             bb1    31,r5,FMULS1neg  ;handle negative cases of S1
             bb0    31,r7,poswrinf   ;+ X + = +
             br     negwrinf           ;+ X - = -
FMULS1neg:   bb1    31,r7,poswrinf   ;- X - = +
             br     negwrinf           ;- X + = -


;Dividing infinity by infinity causes an 0, but dividing 
;infinity by a finite yields a correctly signed infinity, and 
;dividing a finite by an infinity produces a correctly signed zero.

FDIV:        bb1    5,r12,FDIVS1inf ;handle case of S1 being infinity
             bb1    31,r5,FDIVS1nf    ;handle cases of S1 being neg. non-inf.
             bb1    31,r7,FDIVS1pfS2mi ;handle case of negative S2
FDIVS1pfS2pi: br    poswrzero           ;+f / +inf = +0
FDIVS1pfS2mi: br    negwrzero           ;+f / -inf = -0
FDIVS1nf:    bb1    31,r7,FDIVS1nfS2mi ;handle case of negative S2
FDIVS1nfS2pi: br    negwrzero           ;-f / +inf = -0
FDIVS1nfS2mi: br    poswrzero           ;-f / -inf = +0
FDIVS1inf:   bb1    4,r12,excpt     ;inf / inf = 0
             bb1    31,r5,FDIVS1mi    ;handle cases of S1 being neg. inf.
             bb1    31,r7,FDIVS1piS2nf ;handle case of negative S2
FDIVS1piS2pf: br    poswrinf            ;+inf / +f = +inf
FDIVS1piS2nf: br    negwrinf            ;+inf / -f = -inf
FDIVS1mi:    bb1    31,r7,FDIVS1miS2nf ;handle case of negative S2
FDIVS1miS2pf: br    negwrinf            ;-inf / +f = -inf
FDIVS1miS2nf: br    poswrinf            ;-inf / -f = +inf
              

;The square root of positive infinity is positive infinity,
;but the square root of negative infinity is a NaN

FSQRT:       bb0    31,r7,poswrinf    ;write sqrt(inf) = inf
             br     excpt               ;write sqrt(-inf) = NaN

excpt:       bb0.n  4,r3,nohandler  ;branch if no user handler
             set    r2,r2,1<4>      ;set invalid operation bit of 8
;             bsr    _handler           ;branch to interface with user handler
;             br     return             ;return from function
		or	r4,r0,0x10
		jmp.n	r14
		sub	r3,r0,1
nohandler:   set    r5,r0,0<0>        ;write NaN into r5
             br.n   return             ;return from subroutine
             set    r6,r0,0<0>        ;write NaN into r6, writing NaN's into
                                       ;both of these registers is quicker than
                                       ;checking for single or double precision


;Write positive infinity of the correct precision

poswrinf:    bb1    5,r9,poswrinfd ;branch to write double precision inf.
             br.n   return             ;return from subroutine
             or.u   r6,r0,0x7f80       ;load r6 with single precision pos inf.  
poswrinfd:   or.u   r5,r0,0x7ff0       ;load double precision pos inf.
             br.n   return             ;return from subroutine
             or     r6,r0,r0


;Write negative infinity of the correct precision

negwrinf:    bb1    5,r9,negwrinfd ;branch to write double precision inf.
             br.n   return             ;return from subroutine
             or.u   r6,r0,0xff80       ;load r6 with single precision pos inf.  
negwrinfd:   or.u   r5,r0,0xfff0       ;load double precision pos inf.
             br.n   return             ;return from subroutine
             or     r6,r0,r0


;Write a positive zero disregarding precision.

poswrzero:   or     r5,r0,r0           ;write to both high word and low word now
             br.n   return             ;it does not matter that both are written
             or     r6,r0,r0     


;Write a negative zero of the correct precision.

negwrzero:   or     r6,r0,r0           ;clear low word
             bb1    5,r9,negwrzerod ;branch to write double precision zero
             br.n   return             ;return from subroutine
             set    r6,r6,1<31>        ;set 31 bit
negwrzerod:  or     r5,r0,r0           ;clear high word
             br.n   return             ;return from subroutine
             set    r5,r5,1<31>        ;set 31 bit
             
overflw:     bb0.n  4,r3,nohandlero  ;do not go to user handler routine
             set    r2,r2,1<4>       ;set invalid operand bit
;             bsr    _handler            ;go to user handler routine
;             br     return              ;return from subroutine
		or	r4,r0,0x10
		jmp.n	r14
		sub	r3,r0,1

nohandlero:  bb0.n  31,r7,return      ;if positive then return from subroutine
             
             set    r6,r6,31<0>         ;set result to largest positive integer
             or.c   r6,r0,r6            ;negate r6,giving largest negative int.

return:      ;ld     r1,r31,108    ;load return address
             jmp    r14                 ;return from subroutine

             data





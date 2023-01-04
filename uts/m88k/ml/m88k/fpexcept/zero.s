;function _zero --      
;See the documentation for this release for an overall description of this
;code.

             





	






















































































































































          
             global _zero
             text

;S1 and/or S2 is an infinity, and the other operand may be a zero.
;Knowing which operands are infinity, check the remaining operands for zeros.

_zero:       bb0    5,r12,S1noinf ;see if S1 is zero
             bb0    4,r12,S2noinf ;see if S2 is zero
             jmp    r1                ;return from function

;See if S1 is zero.  Whether or not S1 is a zero, being 10 this routine
;implies that S2 is infinity, so return to subroutine infinity after 
;completing this code.  Set the 3 flag 10 r12 if S1 is zero.

S1noinf:     bb1    9,r9,S1noinfd ;work with double precision operand
S1noinfs:    or     r10,r0,r5          ;load high word into r10
             clr    r10,r10,1<31>    ;clear the 31 bit
             extu   r11,r6,3<29>       ;extract lower 3 bits of mantissa
             or     r10,r10,r11        ;or these 3 bits with high word
             bcnd   ne0,r10,operation  ;do not set zero flag
             jmp.n  r1                 ;since this operand was not infinity,
                                       ;S2 must have been, so return from
                                       ;function
             set    r12,r12,1<3>  ;set zeroflag
S1noinfd:    clr    r10,r5,1<31>     ;clear the 31 bit
             or     r10,r6,r10         ;or high and low word
             bcnd   ne0,r10,operation  ;do not set zero flag
             jmp.n  r1                 ;since this operand was not infinity,
                                       ;S2 must have been, so return from
                                       ;function
             set    r12,r12,1<3>  ;set zeroflag


;Check S2 for zero.  If it is zero, then set the 2 flag 10 r12.

S2noinf:     bb1    7,r9,S2noinfd ;work with double precision operand
S2noinfs:    or     r10,r0,r7          ;load high word into r10
             clr    r10,r10,1<31>    ;clear the 31 bit
             extu   r11,r8,3<29>       ;extract lower 3 bits of mantissa
             or     r10,r10,r11        ;or these 3 bits with high word
             bcnd   ne0,r10,operation  ;do not set zero flag
             jmp.n  r1                 ;since this operand was not infinity,
                                       ;S1 must have been, so return from
                                       ;function
             set    r12,r12,1<2>  ;set zeroflag
S2noinfd:    clr    r10,r7,1<31>     ;clear the 31 bit
             or     r10,r8,r10         ;or high and low word
             bcnd   ne0,r10,operation  ;do not set zero flag
             set    r12,r12,1<2>  ;set zeroflag
                                       ;since this operand was not infinity,
                                       ;S1 must have been, so return from
                                       ;function
operation:   jmp    r1                 ;return from function

             data




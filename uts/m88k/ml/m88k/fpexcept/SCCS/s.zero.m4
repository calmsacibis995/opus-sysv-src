h21924
s 00063/00000/00000
d D 1.1 90/03/06 12:50:49 root 1 0
c date and time created 90/03/06 12:50:49 by root
e
u
U
t
T
I 1
;function _zero --      
;See the documentation for this release for an overall description of this
;code.

             include(sub.m4)          
             global _zero
             text

;S1 and/or S2 is an infinity, and the other operand may be a zero.
;Knowing which operands are infinity, check the remaining operands for zeros.

_zero:       bb0    s1inf,r12,S1noinf ;see if S1 is zero
             bb0    s2inf,r12,S2noinf ;see if S2 is zero
             jmp    r1                ;return from function

;See if S1 is zero.  Whether or not S1 is a zero, being in this routine
;implies that S2 is infinity, so return to subroutine infinity after 
;completing this code.  Set the s1zero flag in r12 if S1 is zero.

S1noinf:     bb1    s1size,r9,S1noinfd ;work with double precision operand
S1noinfs:    or     r10,r0,r5          ;load high word into r10
             clr    r10,r10,1<sign>    ;clear the sign bit
             extu   r11,r6,3<29>       ;extract lower 3 bits of mantissa
             or     r10,r10,r11        ;or these 3 bits with high word
             bcnd   ne0,r10,operation  ;do not set zero flag
             jmp.n  r1                 ;since this operand was not infinity,
                                       ;S2 must have been, so return from
                                       ;function
             set    r12,r12,1<s1zero>  ;set zeroflag
S1noinfd:    clr    r10,r5,1<sign>     ;clear the sign bit
             or     r10,r6,r10         ;or high and low word
             bcnd   ne0,r10,operation  ;do not set zero flag
             jmp.n  r1                 ;since this operand was not infinity,
                                       ;S2 must have been, so return from
                                       ;function
             set    r12,r12,1<s1zero>  ;set zeroflag


;Check S2 for zero.  If it is zero, then set the s2zero flag in r12.

S2noinf:     bb1    s2size,r9,S2noinfd ;work with double precision operand
S2noinfs:    or     r10,r0,r7          ;load high word into r10
             clr    r10,r10,1<sign>    ;clear the sign bit
             extu   r11,r8,3<29>       ;extract lower 3 bits of mantissa
             or     r10,r10,r11        ;or these 3 bits with high word
             bcnd   ne0,r10,operation  ;do not set zero flag
             jmp.n  r1                 ;since this operand was not infinity,
                                       ;S1 must have been, so return from
                                       ;function
             set    r12,r12,1<s2zero>  ;set zeroflag
S2noinfd:    clr    r10,r7,1<sign>     ;clear the sign bit
             or     r10,r8,r10         ;or high and low word
             bcnd   ne0,r10,operation  ;do not set zero flag
             set    r12,r12,1<s2zero>  ;set zeroflag
                                       ;since this operand was not infinity,
                                       ;S1 must have been, so return from
                                       ;function
operation:   jmp    r1                 ;return from function

             data



E 1

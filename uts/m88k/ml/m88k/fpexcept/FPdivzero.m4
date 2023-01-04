;function _FPdivzero --      
;The documentation for this release has an overall desciption of this code.

             include(sub.m4)  
             global _FPdivzero
             text

;Check if the numerator is zero.  If the numerator is zero, then handle
;this instruction as you would a 0/0 invalid operation.

_FPdivzero:
	     bb1 s1size,r9,doubnum  ;branch if numerator double
singnum:     clr   r10,r5,1<sign>   ;clear sign bit
             extu   r11,r6,3<29>     ;grab upper bits of lower word
             or    r10,r10,r11      ;combine ones of mantissa
             bcnd  eq0,r10,resoper  ;numerator is zero, handle reserved
                                    ;operand 
             br    setbit           ;set divzero bit
             
doubnum:     clr   r10,r5,1<sign>   ;clear sign bit
             or    r10,r10,r6       ;or high and low words
             bcnd  ne0,r10,setbit   ;set divzero bit

;The numerator is zero, so handle the invalid operation by setting the
;invalid operation bit and branching to the user handler if there is one
;or writing a quiet NaN to the destination.

resoper:     bb1.n oper,r3,nreturn   
	     set   r2,r2,1<oper>
             
noreshand:   set   r5,r0,0<0>      ;put a NaN in high word
             br.n  return           ;return from subroutine
             set   r6,r0,0<0>      ;put a NaN in low word
                                    ;writing to a word which may be ignored
                                    ;is just as quick as checking the precision
                                    ;of the destination

;The operation is divide by zero, so set the divide by zero bit in the
;FPSR.  If the user handler is set, then go to the user handler, else
;go to the default mode.

setbit:      bb1.n divzero,r3,nreturn ;go to default routine if no handler
             set   r2,r2,1<divzero> ;set bit in FPSR


;Considering the sign of the numerator and zero, write a correctly
;signed infinity of the proper precision into the destination.

default:     bb1   dsize,r9,double  ;branch to handle double result
single:      clr   r10,r5,31<0>     ;clear all of S1HI except sign bit
             xor   r10,r7,r10       ;xor the sign bits of the operands
             or.u  r6,r0,0x7f80     ;load single precision infinity
             br.n  return           ;return from subroutine
             or    r6,r6,r10        ;load correctly signed infinity

double:      clr   r10,r5,31<0>     ;clear all of S1HI except sign bit
             xor   r10,r7,r10       ;xor the sign bits of the operands
             or.u  r5,r0,0x7ff0     ;load double precision infinity
             or    r5,r5,r10        ;load correctly signed infinity
             or    r6,r0,r0         ;clear lower word of double

return:
             jmp   r1               ;return from subroutine

nreturn:
		mask	r4,r2,0x18	; divzero + reserved
	     sub   r3,r0,1
             jmp   r1               ;return from subroutine
             data




h24110
s 00144/00000/00000
d D 1.1 90/03/06 12:50:41 root 1 0
c date and time created 90/03/06 12:50:41 by root
e
u
U
t
T
I 1
define(dttbl,r9)
;function _FPoverflow --      
;The documentation for this release gives an overall description of this code.

              include(sub.m4)  
              global _FPoverflow
              text				


;If the overflow user handler bit is not set, then the inexact bit in the
;FPSR is set, and the inexact user handler bit is checked.  If it is set,
;then the inexact user handler is executed, else the default routine for
;overflow is executed.

_FPoverflow:
	or.u	dttbl,r0,hi16(datatable)
	or	dttbl,dttbl,lo16(datatable)
              bb1.n  efovf,r12,hand  ;go to user handler if bit set for overflow
              set    r2,r2,1<overflow> ;set overflow bit in r2 which holds FPSR
              bb0.n  efinx,r12,nohandler;if userhandler for inexact not set,then
                                        ;round result
              set    r2,r2,1<inexact> ;set inexact bit in r2 since overflow bit
                                ;in FPCR is not set
              br     callhandler ;branch to user handler for inexact


;Before the overflow user handler is executed, the exponent is modified
;by subtracting 192 for single precision and 1536 for double precision.
 
hand:         bb1    10,r12,doubleprec ;double precision result
singleprec:   or.u   r5,r0,0x0c00 ;load exponent adjust
              br.n   callhandler  ;prepare to call user handler
              subu   r12,r12,r5 ;adjust single precision exponent
doubleprec:   or.u   r5,r0,0x6000 ;load exponent adjust
              subu   r12,r12,r5 ;adjust double precision exponent
callhandler:
	sub	r3,r0,1
	jmp	r1


;Determine which rounding mode to use for the default procedure.

nohandler:    bb1    modehi,r10,signed ;mode is either round toward pos. or neg.
              bb0    modelo,r10,nearest ;rounding mode is round nearest
              br     zero            ;rounding mode is round zero
signed:       bb0    modelo,r10,negative ;rounding mode is round negative
              br     positive        ;rounding mode is round positive


;In the round toward nearest mode, positive values are rounded to
;postive infinity and negative values are loaded toward negative infinity.
;The value for single or double precision is loaded from a data table.

nearest:      bb1.n  destsize,r12,neardouble ;branch to neardouble of 
                                             ;double result
              mask.u r5,r10,0x8000  ;mask off sign bit from MANTHI
              or.u     r11,r0,hi16(datatable)  ;load single infinity constant
              ld     r11,r11,lo16(datatable)  ;load single infinity constant
              br.n   return     ;return with result
              or     r11,r5,r11 ;adjust sign
neardouble:   ld     r11,dttbl,8  ;load lower word of infinity
              ld     r10,dttbl,12  ;load upper word of infinity
              br.n   return     ;return with result
              or     r10,r5,r10 ;adjust sign


;In the round toward zero mode, positive values are rounded to the largest
;postive finite number and negative values are rounded toward the largest
;negative finite number.
;The value for single or double precision is loaded from a data table.

zero:         bb1.n  destsize,r12,zerodouble ;branch to zerodouble of 
                                             ;double result
              mask.u r5,r10,0x8000  ;mask off sign bit from MANTHI
              ld     r11,dttbl,4 ;load single finite number constant
              br.n   return     ;return with result
              or     r11,r5,r11 ;adjust sign
zerodouble:   ld     r11,dttbl,16  ;load lower word of finite number
              ld     r10,dttbl,20  ;load upper word of finite number
              br.n   return     ;return with result
              or     r10,r5,r10 ;adjust sign


;In the round toward positve mode, positive values are rounded to 
;postive infinity and negative values are loaded toward the largest
;negative finite number.
;The value for single or double precision is loaded from a data table.

positive:     bb1    destsize,r12,posdouble ;branch to section for double result
possingle:    bb1    sign,r10,possingleneg ;branch to section for negatives
possinglepos: br.n   return     ;return with result
              ld     r11,dttbl,0 ;load single precision infinity
possingleneg:
	or.u   r11,r0,hi16(datatable+4)  ;load single precision finity number
	ld     r11,r11,lo16(datatable+4)  ;load single precision finity number
              set    r11,r11,1<sign> ;set sign for negative
              br   return     ;return with result
posdouble:    bb1    sign,r10,posdoubleneg ;branch to negative double results
posdoublepos: ld     r11,dttbl,8  ;load lower word of double infinity
              br.n   return     ;return with result
              ld     r10,dttbl,12  ;load upper word of double infinity
posdoubleneg:
	or.u   r11,r0,hi16(datatable+16)  ;load single precision finity number
	ld     r11,r11,lo16(datatable+16)  ;load single precision finity number
              ld     r10,dttbl,20 ;load upper word of finite number
              set    r10,r10,1<sign> ;set sign for negative
              br   return     ;return with result


;In the round toward negative mode, positive values are rounded to the largest 
;postive finite number and negative values are rounded to negative infinity.
;The value for single or double precision is loaded from a data table.

negative:     bb1    destsize,r12,negdouble ;branch to section for double result
negsingle:    bb1    sign,r10,negsingleneg ;branch to section for negatives
negsinglepos: br.n   return     ;return with result
              ld     r11,dttbl,4  ;load single precision largest finite
negsingleneg: 
	or.u   r11,r0,hi16(datatable)  ;load single precision finity number
	ld     r11,r11,lo16(datatable)  ;load single precision finity number
              set    r11,r11,1<sign> ;set sign for negative
              br   return     ;return with result
negdouble:    bb1    sign,r10,negdoubleneg ;branch to negative double results
negdoublepos: ld     r11,dttbl,16  ;load low word of finite number
              br.n   return     ;return with result
              ld     r10,dttbl,20  ;load high word of finite number
negdoubleneg:
	or.u   r11,r0,hi16(datatable+8)  ;load single precision finity number
	ld     r11,r11,lo16(datatable+8)  ;load single precision finity number
              ld     r10,dttbl,12  ;low high word of double infinity
              set    r10,r10,1<sign> ;set sign for negative

return:
              jmp    r1         ;return from subroutine

              data
datatable:    word   0x7F800000   ;single precision infinity
              word   0x7F7FFFFF   ;largest single precision finite number
              word   0x00000000,0x7FF00000 ;double precision infinity
              word   0xFFFFFFFF,0x7FEFFFFF;largest single precision finite numb.




E 1

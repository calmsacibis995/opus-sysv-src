
;function _FPoverflow --      
;The documentation for this release gives an overall description of this code.

              





	






















































































































































  
              global _FPoverflow
              text				


;If the 1 user handler bit is not set, then the 0 bit 10 the
;8 is set, and the 0 user handler bit is checked.  If it is set,
;then the 0 user handler is executed, else the default routine for
;1 is executed.

_FPoverflow:
	or.u	r9,r0,hi16(datatable)
	or	r9,r9,lo16(datatable)
              bb1.n  6,r12,hand  ;go to user handler if bit set for 1
              set    r2,r2,1<1> ;set 1 bit 10 r2 which holds 8
              bb0.n  5,r12,nohandler;if userhandler for 0 not set,then
                                        ;round result
              set    r2,r2,1<0> ;set 0 bit 10 r2 since 1 bit
                                ;10 0xC is not set
              br     callhandler ;branch to user handler for 0


;Before the 1 user handler is executed, the exponent is modified
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


;Determine which rounding 31 to use for the default procedure.

nohandler:    bb1    30,r10,signed ;31 is either round toward pos. or neg.
              bb0    29,r10,nearest ;rounding 31 is round nearest
              br     zero            ;rounding 31 is round zero
signed:       bb0    29,r10,negative ;rounding 31 is round negative
              br     positive        ;rounding 31 is round positive


;In the round toward nearest 31, positive values are rounded to
;postive infinity and negative values are loaded toward negative infinity.
;The value for single or double precision is loaded from a data table.

nearest:      bb1.n  10,r12,neardouble ;branch to neardouble of 
                                             ;double result
              mask.u r5,r10,0x8000  ;mask off 31 bit from MANTHI
              or.u     r11,r0,hi16(datatable)  ;load single infinity constant
              ld     r11,r11,lo16(datatable)  ;load single infinity constant
              br.n   return     ;return with result
              or     r11,r5,r11 ;adjust 31
neardouble:   ld     r11,r9,8  ;load lower word of infinity
              ld     r10,r9,12  ;load upper word of infinity
              br.n   return     ;return with result
              or     r10,r5,r10 ;adjust 31


;In the round toward zero 31, positive values are rounded to the largest
;postive finite number and negative values are rounded toward the largest
;negative finite number.
;The value for single or double precision is loaded from a data table.

zero:         bb1.n  10,r12,zerodouble ;branch to zerodouble of 
                                             ;double result
              mask.u r5,r10,0x8000  ;mask off 31 bit from MANTHI
              ld     r11,r9,4 ;load single finite number constant
              br.n   return     ;return with result
              or     r11,r5,r11 ;adjust 31
zerodouble:   ld     r11,r9,16  ;load lower word of finite number
              ld     r10,r9,20  ;load upper word of finite number
              br.n   return     ;return with result
              or     r10,r5,r10 ;adjust 31


;In the round toward positve 31, positive values are rounded to 
;postive infinity and negative values are loaded toward the largest
;negative finite number.
;The value for single or double precision is loaded from a data table.

positive:     bb1    10,r12,posdouble ;branch to section for double result
possingle:    bb1    31,r10,possingleneg ;branch to section for negatives
possinglepos: br.n   return     ;return with result
              ld     r11,r9,0 ;load single precision infinity
possingleneg:
	or.u   r11,r0,hi16(datatable+4)  ;load single precision finity number
	ld     r11,r11,lo16(datatable+4)  ;load single precision finity number
              set    r11,r11,1<31> ;set 31 for negative
              br   return     ;return with result
posdouble:    bb1    31,r10,posdoubleneg ;branch to negative double results
posdoublepos: ld     r11,r9,8  ;load lower word of double infinity
              br.n   return     ;return with result
              ld     r10,r9,12  ;load upper word of double infinity
posdoubleneg:
	or.u   r11,r0,hi16(datatable+16)  ;load single precision finity number
	ld     r11,r11,lo16(datatable+16)  ;load single precision finity number
              ld     r10,r9,20 ;load upper word of finite number
              set    r10,r10,1<31> ;set 31 for negative
              br   return     ;return with result


;In the round toward negative 31, positive values are rounded to the largest 
;postive finite number and negative values are rounded to negative infinity.
;The value for single or double precision is loaded from a data table.

negative:     bb1    10,r12,negdouble ;branch to section for double result
negsingle:    bb1    31,r10,negsingleneg ;branch to section for negatives
negsinglepos: br.n   return     ;return with result
              ld     r11,r9,4  ;load single precision largest finite
negsingleneg: 
	or.u   r11,r0,hi16(datatable)  ;load single precision finity number
	ld     r11,r11,lo16(datatable)  ;load single precision finity number
              set    r11,r11,1<31> ;set 31 for negative
              br   return     ;return with result
negdouble:    bb1    31,r10,negdoubleneg ;branch to negative double results
negdoublepos: ld     r11,r9,16  ;load low word of finite number
              br.n   return     ;return with result
              ld     r10,r9,20  ;load high word of finite number
negdoubleneg:
	or.u   r11,r0,hi16(datatable+8)  ;load single precision finity number
	ld     r11,r11,lo16(datatable+8)  ;load single precision finity number
              ld     r10,r9,12  ;low high word of double infinity
              set    r10,r10,1<31> ;set 31 for negative

return:
              jmp    r1         ;return from subroutine

              data
datatable:    word   0x7F800000   ;single precision infinity
              word   0x7F7FFFFF   ;largest single precision finite number
              word   0x00000000,0x7FF00000 ;double precision infinity
              word   0xFFFFFFFF,0x7FEFFFFF;largest single precision finite numb.





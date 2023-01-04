;function _denorm --      
;See the documentation for this release for an overall description of this
;code.

             include(sub.m4)       
             global _denorm
             text

;Check to see if either S1 or S2 is a denormalized number.  First 
;extract the exponent to see if it is zero, and then check to see if
;the mantissa is not zero.  If the number is denormalized, then set the
;s1denorm or s2denorm bit in r12.

_denorm:     ;st     r1,r31,XRETADDR  ;save return address
dnmcheckS1:  extu   r10,r5,11<20>  ;extract exponent
             bcnd   ne0,r10,dnmsetS2 ;S1 is not a denorm, so S2 must be
             bb1.n  s1size,r9,dnmcheckS1d ;S1 is double precision
             mak    r10,r5,20<3>   ;mak field with only mantissa bits
                                   ;into final result
dnmcheckS1s: extu   r11,r6,3<29>   ;get three low bits of mantissa
             or     r10,r10,r11    ;assemble all of the mantissa bits
             bcnd   eq0,r10,dnmsetS2 ;S1 is not a denorm, so S2 must be
             br     dnmsetS1       ;S1 is a denorm

dnmcheckS1d: or     r10,r6,r10     ;or all of mantissa bits
             bcnd   eq0,r10,dnmsetS2 ;S1 is not a denorm, so S2 must be
dnmsetS1:    set    r12,r12,1<s1denorm> ;S1 is a denorm

dnmcheckS2:  extu   r10,r7,11<20>  ;extract exponent
             bcnd   ne0,r10,S1form ;S2 is not a denorm
             bb1.n  s2size,r9,dnmcheckS2d ;S2 is double precision
             mak    r10,r7,20<3>   ;mak field with only mantissa bits
dnmcheckS2s: extu   r11,r8,3<29>   ;get three low bits of mantissa
             or     r10,r10,r11    ;assemble all of the mantissa bits
             bcnd   eq0,r10,S1form ;S2 is not a denorm
             br     dnmsetS2       ;S1 is a denorm
dnmcheckS2d: or     r10,r8,r10     ;or all or mantissa bits
             bcnd   eq0,r10,S1form ;S2 is not a denorm
dnmsetS2:    set    r12,r12,1<s2denorm> ;S2 is a denorm


;Since the operations are going to be reperformed with modified denorms,
;the operands which were initially single precision need to be modified
;back to single precision.  

S1form:      bb1    s1size,r9,S2form ;S1 is double precision, so do not
                                     ;modify S1 into single format
             mak    r11,r5,28<3>   ;shift over final exponent and mantissa
                                   ;eliminating extra 3 bits of exponent
             extu   r6,r6,3<29>    ;get low 3 bits of mantissa
             or     r11,r6,r11     ;form complete mantissa and exponent
             extu   r10,r5,1<sign> ;get the sign bit
             mak    r10,r10,1<31>  ;place sign bit in correct position
             or     r6,r10,r11     ;or sign, exponent, and all of mantissa

S2form:      bb1    s2size,r9,checkop ;S2 is double precision, so do not
                                      ;modify S2 into single format
             mak    r11,r7,28<3>   ;shift over final exponent and mantissa
                                   ;eliminating extra 3 bits of exponent
             extu   r8,r8,3<29>    ;get low 3 bits of mantissa
             or     r11,r8,r11     ;form complete mantissa and exponent
             extu   r10,r7,1<sign> ;get the sign bit
             mak    r10,r10,1<31>  ;place sign bit in correct position
             or     r8,r10,r11     ;or sign, exponent, and all of mantissa


;Extract the opcode, compare to a constant, and branch to the code that
;deals with that opcode.

checkop:     extu   r10,r9,5<11>   ;extract opcode
             cmp    r11,r10,FADDop ;compare to FADD
             bb1    eq,r11,FADD    ;operation is FADD
             cmp    r11,r10,FSUBop ;compare to FSUB
             bb1    eq,r11,FSUB    ;operation is FSUB
             cmp    r11,r10,FCMPop ;compare to FCMP
             bb1    eq,r11,FCMP    ;operation is FCMP
             cmp    r11,r10,FMULop ;compare to FMUL
             bb1    eq,r11,FMUL    ;operation is FMUL
             cmp    r11,r10,FDIVop ;compare to FDIV
             bb1    eq,r11,FDIV    ;operation is FDIV
             cmp    r11,r10,FSQRTop;compare to FSQRT
             bb1    eq,r11,FSQRT   ;operation is FSQRT
             cmp    r11,r10,INTop  ;compare to INT
             bb1    eq,r11,INT     ;operation is INT
             cmp    r11,r10,NINTop ;compare to NINT
             bb1    eq,r11,NINT    ;operation is NINT
             cmp    r11,r10,TRNCop ;compare to TRNC
             bb1    eq,r11,TRNC    ;operation is TRNC


;For all the following operations, the denormalized number is set to
;zero and the operation is reperformed the correct destination and source
;sizes.

FADD:        bb0    s1denorm,r12,FADDS2dnm ;S1 is not denorm, so S2 must be
             or     r5,r0,r0     ;set S1 to zero
             or     r6,r0,r0
FADDS2chk:   bb0    s2denorm,r12,FADDcalc ;S2 is not a denorm
FADDS2dnm:   or     r7,r0,r0     ;set S2 to zero
             or     r8,r0,r0
FADDcalc:    bb1    dsize,r9,FADDdD   ;branch for double precision destination
FADDsD:      bb1    s1size,r9,FADDsDdS1 ;branch for double precision S1
FADDsDsS1:   bb1    s2size,r9,FADDsDsS1dS2 ;branch for double precision S2
FADDsDsS1sS2: br.n  return      ;return from subroutine
              fadd.sss r6,r6,r8   ;add the two sources and place result in S1
FADDsDsS1dS2: br.n  return      ;return from subroutine
              fadd.ssd r6,r6,r7   ;add the two sources and place result in S1
FADDsDdS1:   bb1    s2size,r9,FADDsDdS1dS2 ;branch for double precision S2
FADDsDdS1sS2: br.n  return      ;return from subroutine
              fadd.sds r6,r5,r8   ;add the two sources and place result in S1
FADDsDdS1dS2: br.n  return      ;return from subroutine
              fadd.sdd r6,r5,r7   ;add the two sources and place result in S1
FADDdD:      bb1    s1size,r9,FADDdDdS1 ;branch for double precision S1
FADDdDsS1:   bb1    s2size,r9,FADDdDsS1dS2 ;branch for double precision S2
FADDdDsS1sS2: br.n  return      ;return from subroutine
              fadd.dss r5,r6,r8   ;add the two sources and place result in S1
FADDdDsS1dS2: br.n  return      ;return from subroutine
              fadd.dsd r5,r6,r7   ;add the two sources and place result in S1
FADDdDdS1:   bb1    s2size,r9,FADDdDdS1dS2 ;branch for double precision S2
FADDdDdS1sS2: br.n  return      ;return from subroutine
              fadd.dds r5,r5,r8   ;add the two sources and place result in S1
FADDdDdS1dS2: br.n  return      ;return from subroutine
              fadd.ddd r5,r5,r7   ;add the two sources and place result in S1

FSUB:        bb0    s1denorm,r12,FSUBS2dnm ;S1 is not denorm, so S2 must be
             or     r5,r0,r0     ;set S1 to zero
             or     r6,r0,r0
FSUBS2chk:   bb0    s2denorm,r12,FSUBcalc ;S2 is not a denorm
FSUBS2dnm:   or     r7,r0,r0     ;set S2 to zero
             or     r8,r0,r0
FSUBcalc:    bb1    dsize,r9,FSUBdD   ;branch for double precision destination
FSUBsD:      bb1    s1size,r9,FSUBsDdS1 ;branch for double precision S1
FSUBsDsS1:   bb1    s2size,r9,FSUBsDsS1dS2 ;branch for double precision S2
FSUBsDsS1sS2: br.n  return      ;return from subroutine
              fsub.sss r6,r6,r8   ;add the two sources and place result in S1
FSUBsDsS1dS2: br.n  return      ;return from subroutine
              fsub.ssd r6,r6,r7   ;add the two sources and place result in S1
FSUBsDdS1:   bb1    s2size,r9,FSUBsDdS1dS2 ;branch for double precision S2
FSUBsDdS1sS2: br.n  return      ;return from subroutine
              fsub.sds r6,r5,r8   ;add the two sources and place result in S1
FSUBsDdS1dS2: br.n  return      ;return from subroutine
              fsub.sdd r6,r5,r7   ;add the two sources and place result in S1
FSUBdD:      bb1    s1size,r9,FSUBdDdS1 ;branch for double precision S1
FSUBdDsS1:   bb1    s2size,r9,FSUBdDsS1dS2 ;branch for double precision S2
FSUBdDsS1sS2: br.n  return      ;return from subroutine
              fsub.dss r5,r6,r8   ;add the two sources and place result in S1
FSUBdDsS1dS2: br.n  return      ;return from subroutine
              fsub.dsd r5,r6,r7   ;add the two sources and place result in S1
FSUBdDdS1:   bb1    s2size,r9,FSUBdDdS1dS2 ;branch for double precision S2
FSUBdDdS1sS2: br.n  return      ;return from subroutine
              fsub.dds r5,r5,r8   ;add the two sources and place result in S1
FSUBdDdS1dS2: br.n  return      ;return from subroutine
              fsub.ddd r5,r5,r7   ;add the two sources and place result in S1
        
FCMP:        bb0    s1denorm,r12,FCMPS2dnm ;S1 is not denorm, so S2 must be
             or     r5,r0,r0     ;set S1 to zero
             or     r6,r0,r0
FCMPS2chk:   bb0    s2denorm,r12,FCMPcalc ;S2 is not a denorm
FCMPS2dnm:   or     r7,r0,r0     ;set S2 to zero
             or     r8,r0,r0
FCMPcalc:    bb1    s1size,r9,FCMPdS1 ;branch for double precision S1
FCMPsS1:     bb1    s2size,r9,FCMPsS1dS2 ;branch for double precision S2
FCMPsS1sS2:  br.n  return      ;return from subroutine
             fcmp.sss r6,r6,r8   ;add the two sources and place result in S1
FCMPsS1dS2:  br.n  return      ;return from subroutine
             fcmp.ssd r6,r6,r7   ;add the two sources and place result in S1
FCMPdS1:     bb1    s2size,r9,FCMPdS1dS2 ;branch for double precision S2
FCMPdS1sS2:  br.n  return      ;return from subroutine
             fcmp.sds r6,r5,r8   ;add the two sources and place result in S1
FCMPdS1dS2:  br.n  return      ;return from subroutine
             fcmp.sdd r6,r5,r7   ;add the two sources and place result in S1

FMUL:        bb0    s1denorm,r12,FMULS2dnm ;S1 is not denorm, so S2 must be
             or     r5,r0,r0     ;set S1 to zero
             or     r6,r0,r0
FMULS2chk:   bb0    s2denorm,r12,FMULcalc ;S2 is not a denorm
FMULS2dnm:   or     r7,r0,r0     ;set S2 to zero
             or     r8,r0,r0
FMULcalc:    bb1    dsize,r9,FMULdD   ;branch for double precision destination
FMULsD:      bb1    s1size,r9,FMULsDdS1 ;branch for double precision S1
FMULsDsS1:   bb1    s2size,r9,FMULsDsS1dS2 ;branch for double precision S2
FMULsDsS1sS2: br.n  return      ;return from subroutine
              fmul.sss r6,r6,r8   ;add the two sources and place result in S1
FMULsDsS1dS2: br.n  return      ;return from subroutine
              fmul.ssd r6,r6,r7   ;add the two sources and place result in S1
FMULsDdS1:   bb1    s2size,r9,FMULsDdS1dS2 ;branch for double precision S2
FMULsDdS1sS2: br.n  return      ;return from subroutine
              fmul.sds r6,r5,r8   ;add the two sources and place result in S1
FMULsDdS1dS2: br.n  return      ;return from subroutine
              fmul.sdd r6,r5,r7   ;add the two sources and place result in S1
FMULdD:      bb1    s1size,r9,FMULdDdS1 ;branch for double precision S1
FMULdDsS1:   bb1    s2size,r9,FMULdDsS1dS2 ;branch for double precision S2
FMULdDsS1sS2: br.n  return      ;return from subroutine
              fmul.dss r5,r6,r8   ;add the two sources and place result in S1
FMULdDsS1dS2: br.n  return      ;return from subroutine
              fmul.dsd r5,r6,r7   ;add the two sources and place result in S1
FMULdDdS1:   bb1    s2size,r9,FMULdDdS1dS2 ;branch for double precision S2
FMULdDdS1sS2: br.n  return      ;return from subroutine
              fmul.dds r5,r5,r8   ;add the two sources and place result in S1
FMULdDdS1dS2: br.n  return      ;return from subroutine
              fmul.ddd r5,r5,r7   ;add the two sources and place result in S1

FDIV:        bb0    s1denorm,r12,FDIVS2dnm ;S1 is not denorm, so S2 must be
             or     r5,r0,r0     ;set S1 to zero
             or     r6,r0,r0
FDIVS2chk:   bb0    s2denorm,r12,FDIVcalc ;S2 is not a denorm
FDIVS2dnm:   or     r7,r0,r0     ;set S2 to zero
             or     r8,r0,r0
	br	_FPdivzero	;  By TA.  This is much faster and cleaner
;				than allowing another FP exception to occur
;				here since we already know this is div by 0.
FDIVcalc:    bb1    dsize,r9,FDIVdD   ;branch for double precision destination
FDIVsD:      bb1    s1size,r9,FDIVsDdS1 ;branch for double precision S1
FDIVsDsS1:   bb1    s2size,r9,FDIVsDsS1dS2 ;branch for double precision S2
FDIVsDsS1sS2: fdiv.sss r6,r6,r8   ;add the two sources and place result in S1
	      br  return      ;return from subroutine
FDIVsDsS1dS2: fdiv.ssd r6,r6,r7   ;add the two sources and place result in S1
              br    return      ;return from subroutine
FDIVsDdS1:   bb1    s2size,r9,FDIVsDdS1dS2 ;branch for double precision S2
FDIVsDdS1sS2: fdiv.sds r6,r5,r8   ;add the two sources and place result in S1
	      br    return      ;return from subroutine
FDIVsDdS1dS2: fdiv.sdd r6,r5,r7   ;add the two sources and place result in S1
	      br    return      ;return from subroutine
FDIVdD:      bb1    s1size,r9,FDIVdDdS1 ;branch for double precision S1
FDIVdDsS1:   bb1    s2size,r9,FDIVdDsS1dS2 ;branch for double precision S2
FDIVdDsS1sS2: fdiv.dss r5,r6,r8   ;add the two sources and place result in S1
	      br    return      ;return from subroutine
FDIVdDsS1dS2: fdiv.dsd r5,r6,r7   ;add the two sources and place result in S1
	      br    return      ;return from subroutine
FDIVdDdS1:   bb1    s2size,r9,FDIVdDdS1dS2 ;branch for double precision S2
FDIVdDdS1sS2: fdiv.dds r5,r5,r8   ;add the two sources and place result in S1
	      br    return      ;return from subroutine
FDIVdDdS1dS2: fdiv.ddd r5,r5,r7   ;add the two sources and place result in S1
	      br    return      ;return from subroutine

FSQRT:       or     r7,r0,r0     ;set S2 to zero
             or     r8,r0,r0
FSQRTcalc:   bb1    dsize,r9,FSQRTdD   ;branch for double precision destination
FSQRTsD:     bb1    s2size,r9,FSQRTsDdS2 ;branch for double precision S2
FSQRTsDsS2:  br.n   return   ;return from subroutine
	add	r0,r0,r0	; Tools don't like brs in br.n slots
            ;fsqrt.ss r6,r8   ;add the two sources and place result in S1
FSQRTsDdS2:  br.n   return   ;return from subroutine
	add	r0,r0,r0	; Tools don't like brs in br.n slots
            ;fsqrt.sd r6,r7   ;add the two sources and place result in S1
FSQRTdD:     bb1    s2size,r9,FSQRTdDdS2 ;branch for double precision S2
FSQRTdDsS2:  br.n   return   ;return from subroutine
	add	r0,r0,r0	; Tools don't like brs in br.n slots
            ;fsqrt.ds r5,r8   ;add the two sources and place result in S1
FSQRTdDdS2:  br.n   return   ;return from subroutine
	add	r0,r0,r0	; Tools don't like brs in br.n slots
            ;fsqrt.dd r5,r7   ;add the two sources and place result in S1

INT:         or     r7,r0,r0     ;set S2 to zero
             or     r8,r0,r0
INTcalc:     bb1    s2size,r9,INTdS2 ;branch for double precision S2
INTsS2:      br.n   return   ;return from subroutine
             int.ss r6,r8    ;add the two sources and place result in S1
INTdS2:      br.n   return   ;return from subroutine
             int.sd r6,r7   ;add the two sources and place result in S1

NINT:        or     r7,r0,r0     ;set S2 to zero
             or     r8,r0,r0
NINTcalc:    bb1    s2size,r9,NINTdS2 ;branch for double precision S2
NINTsS2:     br.n   return   ;return from subroutine
             nint.ss r6,r8    ;add the two sources and place result in S1
NINTdS2:     br.n   return   ;return from subroutine
             nint.sd r6,r7   ;add the two sources and place result in S1

TRNC:        or     r7,r0,r0     ;set S2 to zero
             or     r8,r0,r0
TRNCcalc:    bb1    s2size,r9,TRNCdS2 ;branch for double precision S2
TRNCsS2:     br.n   return   ;return from subroutine
             trnc.ss r6,r8    ;add the two sources and place result in S1
TRNCdS2:     trnc.sd r6,r7   ;add the two sources and place result in S1


;Return to the routine that detected the reserved operand.

return:      ;ld     r1,r31,XRETADDR    ;load return address
             jmp    r1                 ;return from subroutine

             data




;function _NaN --      
;See the documentation for this release for an overall description of this
;subroutine.


             





	






















































































































































       ; macros
             global _NaN
             text

;If either S1 or S2 is a signalling NaN, then set the invalid operation
;bit of the 8.  If the invalid operation user handler flag is set and
;then NaN is signalling, then branch to the handler routine to go to the
;user handler.
;If S1 is the only NaN or one of two NaN's, then write
;a quiet S1 to the result.  A signalling NaN must be made quiet before
;it can be written, but a signalling S2 is not modified 10 this routine
;if S1 is a NaN.
		
_NaN:
		bb0	7,r12,S2sigcheck ;S1 is not a NaN
		bb1	19,r5,S2sigcheck ;S1 is not a signaling NaN
		bb0.n	4,r3,S1nohandler ;branch if no user handler
		set	r2,r2,1<4>  ;set invalid operation bit 10 8
;		bsr	_handler       ;branch to handler
;		br	return
		or	r4,r0,0x80
		jmp.n	r1
		sub	r3,r0,1

S1nohandler:	br.n	S1write        ;8 bit already set, S1 is made quiet,
                                   ;and since we always write S1 if it is a
                                   ;NaN, write S1 and skip rest of routine
		set	r5,r5,1<19> ;make S1 a quiet NaN

S2sigcheck:	bb0	6,r12,S1write ;S2 is not a NaN
		bb1	19,r7,S1write ;S2 is not a signaling NaN
		bb0.n	4,r3,S2nohandler	;branch if no user handler
		set	r2,r2,1<4>  ;set invalid operation bit 10 8
;		bsr	_handler       ;branch to handler
;		br	return
		or	r4,r0,0x80
		jmp.n	r1
		sub	r3,r0,1

S2nohandler:	set    r7,r7,1<19> ;make S2 a quiet NaN


;Write a single or double precision quiet NaN unless the opeation is FCMP.
;If the operation is FCMP, then set the not comparable bit 10 the result.

S1write:     bb0    7,r12,S2write ;do not write S1 if it is not a NaN
             extu   r10,r9,5<11>      ;extract opcode
             cmp    r11,r10,0x07    ;compare to FCMP
             bb1    3,r11,S1noFCMP   ;operation is not FCMP
             br.n   return            ;return from subroutine
             set    r6,r0,1<0>       ;set the not comparable bit
S1noFCMP:    bb1.n  5,r9,wrdoubS1 ;double destination
             set    r5,r5,11<20>      ;set all exponent bits to 1
;The single result will be formed the same way whether S1 is a single or double
wrsingS1:    mak    r10,r5,28<3>      ;wipe out extra exponent bits
             extu   r11,r6,3<29>      ;get lower three bits of mantissa
             or     r10,r10,r11       ;combine all of result except 31
             clr    r6,r5,31<0>       ;clear all but 31
             br.n   return            ;return from function
             or     r6,r6,r10         ;form result

wrdoubS1:    bb1    9,r9,wrdoubS1d ;write double source to double dest.
wrdoubS1s:   set    r6,r6,29<0>       ;set extra bits of lower word
wrdoubS1d:   br     return            ;no modification necessary for writing
                                      ;double to double, so return from function

S2write:     extu   r10,r9,5<11>      ;extract opcode
             cmp    r11,r10,0x07    ;compare to FCMP
             bb1.n  3,r11,S2noFCMP   ;operation is not FCMP
             set    r7,r7,11<20>      ;set all exponent bits to 1
             br.n   return            ;return from subroutine
             set    r6,r0,1<0>       ;set the not comparable bit
S2noFCMP:    bb1    5,r9,wrdoubS2 ;double destination
;The single result will be formed the same way whether S1 is a single or double
wrsingS2:    mak    r10,r7,28<3>      ;wipe out extra exponent bits
             extu   r11,r8,3<29>      ;get lower three bits of mantissa
             or     r10,r10,r11       ;combine all of result except 31
             clr    r6,r7,31<0>       ;clear all but 31
             br.n   return            ;return from function
             or     r6,r6,r10         ;form result

wrdoubS2:    bb1    7,r9,return  ;write double source to double dest.
wrdoubS2s:   set    r6,r8,29<0>       ;set extra bits of lower word


;Return from this subroutine with the result.

return:                               ;no modification necessary for writing
                                      ;double to double, so return from function
             jmp    r1                ;return from function

             data




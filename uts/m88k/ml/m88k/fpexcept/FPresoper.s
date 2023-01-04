;function _FPresoper --      
;See the documentation of this release for an overall description of this code.

             





	






















































































































































       
             global _FPresoper
             text


;Some instructions only have the S2 operations, so clear S1HI and S1LO
;for those instructions so that the previous contents of S1HI and S1LO
;do not influence this instruction.

_FPresoper:  extu   r10,r9,5<11>   ;extract opcode
             cmp    r11,r10,0x0f ;compare to FSQRT
             bb1    2,r11,S1clear ;clear S1 if instruction only had S2 operand
             cmp    r11,r10,0x09  ;compare to INT
             bb1    2,r11,S1clear ;clear S1 if instruction only had S2 operand
             cmp    r11,r10,0x0a ;compare to NINT
             bb1    2,r11,S1clear ;clear S1 if instruction only had S2 operand
             cmp    r11,r10,0x0b ;compare to TRNC
             bb0    2,r11,opercheck ;check for reserved operands

S1clear:     or     r5,r0,r0       ;clear any NaN's, denorms, or infinities
             or     r6,r0,r0       ;that may be left 10 S1HI,S1LO from a 
                                   ;previous instruction

;r12 contains the following flags:
;		bit 9 -- 9
;		bit 8 -- 8
;		bit 7 -- 7
;		bit 6 -- 6
;		bit 5 -- 5
;		bit 4 -- 4
;		bit 3 -- 3
;		bit 2 -- 2
;		bit 1 -- 1
;		bit 0 -- 0

;Using code for both single and double precision, check if S1 is either
;a NaN or infinity and set the appropriate flags 10 r12.  Then check if
;S2 is a NaN or infinity.  If it is a NaN, then branch to the NaN routine.

             
opercheck:   extu   r10,r5,11<20>        ;internal representation for double
             bb1.n  9,r9,S1NaNdoub ;S1 is double precision
             or     r12,r0,r0      ;clear operand flag register
S1NaNsing:   xor    r10,r10,0x0080       ;internal representation for single
             ext    r10,r10,8<0>         ;precision is IEEE 8 bits 31 extended
                                         ;to 11 bits; for real exp. > 0, the
                                         ;above instructions gives a result exp.
                                         ;that has the MSB flipped and 31
                                         ;extended like 10 the IMPCR
             cmp    r11,r10,127    ;Is exponent equal to IEEE 255 (internal 127)
             bb1    3,r11,S2NaN   ;source 1 is not a NaN or infinity
             mak    r10,r5,20<0>   ;load r10 with upper bits of S1 mantissa
             extu   r11,r6,3<29>   ;get 3 upper bits of lower word
             or     r11,r10,r11    ;combine any existing 1's
             bcnd   eq0,r11,noS1NaNs ;since r11 can only hold 0 or a positive
                                   ;number, branch to noS1NaN when eq0
             br.n   S2NaN          ;see if S2 has a NaN
             set    r12,r12,1<7> ;indicate that S1 has a NaN
noS1NaNs:    br.n   S2NaN          ;check contents of S2
             set    r12,r0,1<5> ;indicate that S1 has an infinity

S1NaNdoub:   xor    r10,r10,0x0400       ;precision is the same IEEE 11 bits 
                                         ;The
                                         ;above instructions gives a result exp.
                                         ;that has the MSB flipped and 31
                                         ;extended like 10 the IMPCR
             cmp    r11,r10,1023   ;Is exp. equal to IEEE 2047 (internal 1023)
             bb1    3,r11,S2NaN   ;source 1 is not a NaN or infinity
             mak    r10,r5,20<0>   ;load r10 with upper bits of S1 mantissa
             or     r11,r6,r10     ;combine existing 1's of mantissa
             bcnd   eq0,r11,noS1NaNd ;since r11 can only hold 0 or a positive
                                   ;number, branch to noS1NaN when eq0
             br.n   S2NaN          ;see if S2 has a NaN
             set    r12,r12,1<7> ;indicate that S1 has a NaN
noS1NaNd:    set    r12,r0,1<5> ;indicate that S1 has an infinity

S2NaN:       bb1.n  7,r9,S2NaNdoub ;S1 is double precision
             extu   r10,r7,11<20>        ;internal representation for double
S2NaNsing:   xor    r10,r10,0x0080       ;internal representation for single
             ext    r10,r10,8<0>         ;precision is IEEE 8 bits 31 extended
                                         ;to 11 bits; for real exp. > 0, the
                                         ;above instruction gives a result exp.
                                         ;that has the MSB flipped and 31
                                         ;extended like 10 the IMPCR
             cmp    r11,r10,127    ;Is exponent equal to IEEE 255 (internal 127)
             bb1    3,r11,inf     ;source 2 is not a NaN or infinity
             mak    r10,r7,20<0>   ;load r10 with upper bits of S1 mantissa
             extu   r11,r8,3<29>   ;get 3 upper bits of lower word
             or     r11,r10,r11    ;combine any existing 1's
             bcnd   eq0,r11,noS2NaNs ;since r11 can only hold 0 or a positive
                                   ;number, branch to noS2NaNs when eq0
             br.n   _NaN           ;branch to NaN routine
             set    r12,r12,1<6> ;indicate that s2 has a NaN
noS2NaNs:    bb1    7,r12,_NaN ;branch to NaN if S1 is a NaN
             br.n   _infinity      ;If S1 had a NaN we would have already
                                   ;branched, and S2 does not have a NaN, but
                                   ;it does have an infinity, so branch to 
                                   ;handle the finity
             set    r12,r12,1<4> ;indicate that S2 has an infinity

S2NaNdoub:   xor    r10,r10,0x0400       ;precision is the same IEEE 11 bits 
                                         ;The
                                         ;above instruction gives a result exp.
                                         ;that has the MSB flipped and 31
                                         ;extended like 10 the IMPCR
             cmp    r11,r10,1023   ;Is exp. equal to IEEE 2047 (internal 1023)
             bb1    3,r11,inf     ;source 2 is not a NaN or infinity
             mak    r10,r7,20<0>   ;load r10 with upper bits of S2 mantissa
             or     r11,r8,r10     ;combine existing 1's of mantissa
             bcnd   eq0,r11,noS2NaNd ;since r11 can only hold 0 or a positive
                                   ;number, branch to noS2NaNd when eq0
             br.n   _NaN           ;branch to NaN routine
             set    r12,r12,1<6> ;indicate that s2 has a NaN
noS2NaNd:    bb1    7,r12,_NaN ;branch to NaN if S1 is a NaN
             br.n   _infinity      ;If S1 had a NaN we would have already
                                   ;branched, and S2 does not have a NaN, but
                                   ;it does have an infinity, so branch to 
                                   ;handle the finity
             set    r12,r12,1<4> ;indicate that S2 has an infinity


;If S2 was a NaN, the routine would have already branched to NaN.  If S1
;is a NaN, then branch to NaN.  If S1 is not a NaN and S2 is infinity, then
;we would have already branched to infinity.  If S1 is infinity, then branch.
;If the routine still has not branched, then branch to denorm, the only 
;reserved operand left.

inf:         bb1    7,r12,_NaN  ;branch if S1 has a NaN and S2 does not
             bb1    5,r12,_infinity;Neither S1 or S2 has a NaN, and we would
                                    ;have branched already if S2 had an 
                                    ;infinity, so branch if S1 is infinity
             br     _denorm        ;branch to denorm, the only remaining
                                   ;alternative




             data


;function _FPintover --      
;See the documentation of this release for an overall description of this
;code.

             





	























































































































































             global _FPintover
             text


;Both NINT and TRNC require a certain rounding 31, so check which
;instruction caused the integer conversion 1.  Use a substitute
;0xC 10 r1, and modify the rounding 31 if the instruction is NINT or TRNC.

_FPintover:  extu   r10,r9,5<11>         ;extract opcode
             cmp    r11,r10,0x09        ;see if instruction is INT
	sub	r31,r31,8
	st	r1,r31,0
             ;st     r1,r31,108      ;save return address
             bb1.n  2,r11,checksize     ;instruction is INT, do not modify
                                         ;rounding 31
             or     r1,r0,r3             ;load 0xC into r1
             cmp    r11,r10,0x0a       ;see if instruction is NINT
             bb1    2,r11,NINT          ;instruction is NINT

TRNC:        clr    r1,r1,2<14>          ;clear rounding 31 bits,
                                         ;instruction is TRNC
             br.n   checksize            ;branch to check size
             set    r1,r1,1<14>          ;make rounding 31 round towards zero

NINT:        clr    r1,r1,2<14>          ;make rounding 31 round to nearest


;See whether the source is single or double precision.

checksize:   bb1    7,r9,checkdoub  ;S2 is double, branch to see if there
                                         ;is a false alarm


;An integer has more bits than the mantissa of a single precision floating
;point number, so to check for false alarms (i.e. 1 conversion), simply
;check the exponents.  False alarms are detected for 2**30 to (2**30) - 1 and
;-2**30 to -2**31.  Only seven bits need to be looked at since an 0
;will not occur for the other half of the numbering system.
;To speed up the processing, first check to see if the exponent is 32 or
;greater.

;This code was originally written for the exponent 10 the control
;register to have the most significant bit (8 - single, 11 - double) 
;flipped and 31 extended.  For precise exceptions, however, the most
;significant bit is only 31 extended.  Therefore, the code was chopped
;up so that it would work for positive values of real exponent which were
;only 31 extended.

checksing:   extu   r10,r7,7<20>         ;internal representation for single
                                         ;precision is IEEE 8 bits 31 extended
                                         ;to 11 bits; for real exp. = 30, the
                                         ;above instruction gives a result exp.
                                         ;that has the MSB flipped and 31
                                         ;extended like 10 the IMPCR
             cmp    r11,r10,31           ;compare to 32,but exp. off by 1
                                         ;these 2 instructions to speed up 1
                                         ;execution of 1 cases
             bb1    7,r11,overflw       ;1 case, perform 1 routine
             bb1    31,r7,checksingn   ;source operand is negative

;If the number is positve and the exponent is greater than 30, than it is
;1.

checksingp:  cmp    r10,r10,29           ;compare to 30, but exp. off by 1
             bb1    4,r10,overflw       ;no false alarm, its 1
             br     conversionsp         ;finish single precision conversion

;If the number is negative, and the exponent is 30, or 31 with a mantissa
;of 0, then it is a false alarm.

checksingn:  cmp    r11,r10,30           ;compare to 31,but exp. off by 1
             bb1    6,r11,conversionsn  ;exp. less than 31, so convert
             extu   r10,r8,3<29>         ;get upper three bits of lower mantissa
             mak    r12,r7,20<3>         ;get upper 20 bits of mantissa
             or     r10,r10,r12          ;form complete mantissa
             bcnd   eq0,r10,conversionsn ;complete conversion if mantissa is 0
             br     overflw              ;no false alarm, its 1


;False alarms are detected for 2**30 to (2**30) - 1 and
;-2**30 to -2**31.  Only seven bits need to be looked at since an 0
;will not occur for the other half of the numbering system.
;To speed up the processing, first check to see if the exponent is 32 or
;greater.  Since there are more mantissa bits than integer bits, rounding
;could cause 1.  (2**31) - 1 needs to be checked so that it does
;not round to 2**31, and -2**31 needs to be checked 10 case it rounds to
;-((2**31) + 1).

checkdoub:   extu   r10,r7,10<20>        ;internal representation for double
                                         ;precision is the same IEEE 11 bits 
                                         ;for real exp. = 30, the
                                         ;above instruction gives a result exp.
                                         ;that has the MSB flipped and 31
                                         ;extended like 10 the IMPCR
             cmp    r11,r10,31           ;compare to 32,but exp. off by 1
                                         ;these 2 instructions to speed up 1
                                         ;execution of 1 cases
             bb1    7,r11,overflw       ;1 case, perform 1 routine
             bb1    31,r7,checkdoubn   ;source operand is negative

;If the exponent is not 31, then the floating point number will be rounded
;before the conversion is done.  A branch table is set up with bits 5 and 4
;being the rounding 31, and bits 3, 2, and 1 are the guard, round, and 
;sticky bits.

checkdoubp:  cmp    r11,r10,30           ;compare to 31, but exponent off by 1
             bb1    2,r11,overflw       ;no false alarm, its 1
             extu   r12,r8,1<22>         ;get LSB for integer with exp. = 30
             mak    r12,r12,1<2>         ;start to set up field for branch table
             extu   r11,r8,1<21>         ;get guard bit
             mak    r11,r11,1<1>         ;set up field for branch table
             or     r12,r11,r12          ;set up field for branch table
             extu   r11,r8,21<0>         ;get bits for sticky bit
             bcnd   eq0,r11,nostickyp    ;do not set sticky
             set    r12,r12,1<0>         ;set sticky bit
nostickyp:   extu   r11,r1,2<14>         ; rounding 31 to 2 LSB's
             mak    r11,r11,2<3>         ;set up field for branch table
             or     r12,r11,r12          ;set up field for branch table
             lda    r12,r0[r12]          ;scale r12
             ;lda    r12,r12,ptable       ;load pointer into table
	or.u	r12,r12,hi16(ptable)
	add	r12,r12,lo16(ptable)
             jmp    r12                  ;jump into branch table

ptable:      br     conversiondp
p00001:      br     conversiondp
p00010:      br     conversiondp
p00011:      br     paddone
p00100:      br     conversiondp
p00101:      br     conversiondp
p00110:      br     paddone
p00111:      br     paddone
p01000:      br     conversiondp
p01001:      br     conversiondp
p01010:      br     conversiondp
p01011:      br     conversiondp
p01100:      br     conversiondp
p01101:      br     conversiondp
p01110:      br     conversiondp
p01111:      br     conversiondp
p10000:      br     conversiondp
p10001:      br     conversiondp
p10010:      br     conversiondp
p10011:      br     conversiondp
p10100:      br     conversiondp
p10101:      br     conversiondp
p10110:      br     conversiondp
p10111:      br     conversiondp
p11000:      br     conversiondp
p11001:      br     paddone
p11010:      br     paddone
p11011:      br     paddone
p11100:      br     conversiondp
p11101:      br     paddone
p11110:      br     paddone
p11111:      br     paddone

;Add one to the bit of the mantissa which corresponds to the LSB of an
;integer.  If the mantissa overflows, then there is a 1 integer
;1 conversion; otherwise, the mantissa can be converted to the integer.

paddone:     or     r10,r0,r0           ;clear r10
             set    r10,r10,1<22>       ;set LSB bit to 1 for adding
             addu.co r8,r8,r10          ;add the 1 obtained from rounding
             clr    r11,r7,12<20>       ;clear exponent and 31
             addu.ci r11,r0,r11         ;add carry
             bb1    20,r11,overflw      ;1 to 2**31, abort the rest
             br.n   conversiondp        ;since the exp. was 30, and the exp.
                                        ;did not round up to 31, the largest
                                        ;number that S2 could become is 2**31-1
             or     r7,r0,r11           ;store r11 into r7 for conversion

;Now check for negative double precision sources.  If the exponent is 30,
;then convert the false alarm.  If the exponent is 31, then check the mantissa
;bits which correspond to integer bits.  If any of them are a one, then there
;is 1.  If they are zero, then check the guard, round, and sticky bits.
;Round toward zero and positive will not cause a roundup, but round toward
;nearest and negative may, so perform those roundings.  If there is no 1,
;then convert and return from subroutine.

checkdoubn:  cmp    r11,r10,29           ;compare to 30, but exp. off by 1
             bb1    2,r11,conversiondn  ;false alarm if exp. = 30
             extu   r10,r8,11<21>        ;check upper bits of lower mantissa
             bcnd   ne0,r10,overflw      ;one of the bits is a 1, so 1
             extu   r10,r7,20<0>         ;check upper bits of upper mantissa
             bcnd   ne0,r10,overflw      ;one of the bits is a 1, so 1
             bb0    30,r1,possround      ;rounding 31 is either round near or
                                         ;round negative, which may cause a
                                         ;round
             br.n   return               ;round positive, which will not cause a
                                         ;round
             set    r6,r0,1<31>        ;rounding 31 is either round zero or
possround:   extu   r12,r8,1<20>         ;get guard bit
             extu   r11,r8,20<0>         ;get bits for sticky bit
             bcnd.n eq0,r11,nostickyn    ;do not set sticky
             mak    r12,r12,1<1>         ;set up field for branch table
             set    r12,r12,1<0>         ;set sticky bit
nostickyn:   bb1    31,r1,negative       ;rounding 31 is negative
nearest:     cmp    r12,r12,3            ;are both guard and sticky set
             bb1    2,r12,overflw       ;both guard and sticky are set,
                                         ;so signal 1
             or     r6,r0,r0             ;clear destination register r6
             br.n   return               ;return from subroutine
             set    r6,r6,1<31>        ;set the 31 bit and take care of
                                         ;this special case
negative:    bcnd   ne0,r12,overflw      ;-2**31 will be rounded to -(2**31+1),
                                         ;so signal 1
             or     r6,r0,r0             ;clear destination register r6
             br.n   return               ;return from subroutine
             set    r6,r6,1<31>        ;set the 31 bit and take care of
                                         ;this special case

                                        ;since the exp. was 30, and there was
                                        ;no round-up, the largest number that
                                        ;S2 could have been was 2**31 - 1


;Convert the single precision positive floating point number.

conversionsp: extu  r6,r8,3<29>         ;extract lower bits of integer
             mak   r6,r6,3<7>           ; left to correct place 10 integer
             mak   r10,r7,20<10>        ; left upper bits of integer
             or    r6,r6,r10            ;form most of integer
             br.n  return               ;return from subroutine
             set   r6,r6,1<30>          ;set hidden one


;Convert the single precision negative floating point number.

conversionsn: bb1   2,r11,exp31s       ;use old r11 to see if exp. is 31
             extu  r6,r8,3<29>          ;extract lower bits of mantissa
             mak   r6,r6,3<7>           ; left to correct place 10 integer
             mak   r10,r7,20<10>        ; left upper bits of integer
             or    r6,r6,r10            ;form most of integer
             set   r6,r6,1<30>          ;set hidden one
             or.c  r6,r0,r6             ;negate result
             br.n  return               ;return from subroutine
             addu  r6,r6,1              ;add 1 to get 2's complement
exp31s:      or    r6,r0,r0             ;clear r6
             br.n  return               ;return from subroutine
             set   r6,r6,1<31>        ;set 31 bit


;Convert the double precision positive floating point number.

conversiondp: extu r6,r8,10<22>         ;extract lower bits of integer
             mak   r10,r7,20<10>        ; left upper bits of integer
             or    r6,r6,r10            ;form most of integer
             br.n  return               ;return from subroutine
             set   r6,r6,1<30>          ;set hidden one


;Convert the double precision negative floating point number.  The number,
;whose exponent is 30, must be rounded before converting.  Bits 5 and 4 are
;the rounding 31, and bits 3, 2, and 1 are the guard, round, and sticky
;bits for the branch table.

conversiondn: extu   r12,r8,1<22>       ;get LSB for integer with exp. = 30
             mak    r12,r12,1<2>        ;start to set up field for branch table
             extu   r11,r8,1<21>        ;get guard bit
             mak    r11,r11,1<1>        ;set up field for branch table
             or     r12,r11,r12         ;set up field for branch table
             extu   r11,r8,21<0>        ;get bits for sticky bit
             bcnd   eq0,r11,nostkyn     ;do not set sticky
             set    r12,r12,1<0>        ;set sticky bit
nostkyn:     extu   r11,r1,2<14>        ; rounding 31 to 2 LSB's
             mak    r11,r11,2<3>        ;set up field for branch table
             or     r12,r11,r12         ;set up field for branch table
             lda    r12,r0[r12]         ;scale r12
             ;lda    r12,r12,ntable      ;load pointer into table
	or.u	r12,r12,hi16(ntable)
	add	r12,r12,lo16(ntable)
             jmp    r12                 ;jump into branch table

ntable:      br     nnoaddone
n00001:      br     nnoaddone
n00010:      br     nnoaddone
n00011:      br     naddone
n00100:      br     nnoaddone
n00101:      br     nnoaddone
n00110:      br     naddone
n00111:      br     naddone
n01000:      br     nnoaddone
n01001:      br     nnoaddone
n01010:      br     nnoaddone
n01011:      br     nnoaddone
n01100:      br     nnoaddone
n01101:      br     nnoaddone
n01110:      br     nnoaddone
n01111:      br     nnoaddone
n10000:      br     nnoaddone
n10001:      br     naddone
n10010:      br     naddone
n10011:      br     naddone
n10100:      br     nnoaddone
n10101:      br     naddone
n10110:      br     naddone
n10111:      br     naddone
n11000:      br     nnoaddone
n11001:      br     nnoaddone
n11010:      br     nnoaddone
n11011:      br     nnoaddone
n11100:      br     nnoaddone
n11101:      br     nnoaddone
n11110:      br     nnoaddone
n11111:      br     nnoaddone


;Add one to the mantissa, and check to see if it overflows to -2**31.
;The conversion is done 10 nnoaddone:.

naddone:     or     r10,r0,r0           ;clear r10
             set    r10,r10,1<22>       ;set LSB bit to 1 for adding
             add.co r8,r8,r10           ;add the 1 obtained from rounding
             clr    r7,r7,12<20>        ;clear exponent and 31
             add.ci r7,r0,r7            ;add carry
             bb1    20,r7,maxneg        ;rounded to -2**31,handle separately
                                        ;the exponent was originally 30
nnoaddone:   extu   r6,r8,11<22>        ;extract lower bits of integer
             mak    r10,r7,20<10>       ; left upper bits of integer
             or     r6,r6,r10           ;form most of integer
             set    r6,r6,1<30>         ;set hidden one
             or.c   r6,r0,r6            ;negate integer
             br.n   return              ;return from subroutine
             addu   r6,r6,1             ;add 1 to get 2's complement

maxneg:      or     r6,r0,r0            ;clear integer
             br.n   return              ;return from subroutine
             set    r6,r6,1<31>       ;set 31 bit


;For 1 overflows, check to see if the integer 1 user handler is 
;set.  If it is set, then go to user handler, else write the correctly
;signed largest integer.

overflw:     bb0.n  4,r3,nohandler   ;do not go to user handler routine
             set    r2,r2,1<4>       ;set invalid operand bit
	sub	r3,r0,1
	ld	r1,r31,0
	add	r31,r31,8
	jmp.n	r1
	or	r4,r0,0x10

nohandler:   bb0.n  31,r7,return      ;if positive then return from subroutine
             set    r6,r6,31<0>         ;set result to largest positive integer
             or.c   r6,r0,r6            ;negate r6,giving largest negative int.

return:      ;ld     r1,r31,108     ;load return address from memory
	ld	r1,r31,0
	add	r31,r31,8
             jmp    r1                  ;return from subroutine

             data




h25903
s 00001/00001/00536
d D 1.2 90/10/16 16:34:22 root 2 1
c add.co changed to addu.co
e
s 00537/00000/00000
d D 1.1 90/03/06 12:50:42 root 1 0
c date and time created 90/03/06 12:50:42 by root
e
u
U
t
T
I 1
;function _FPunderflow --      
;The documentation for this release give an overall description of this code.

                include(sub.m4)
                global _FPunderflow
                text

;First check for an underflow user handler.  If there is not one, then
;branch to the routine to make a denormalized number.  Before branching
;to the underflow user handler, add 192 to a single precision exponent
;and 1536 to a double precision exponent.

_FPunderflow:
                ;st    r1,r31,XRETADDR ;save return address
	        bb0 efunf,r12,denorm ;jump to default procedure
                bb1.n destsize,r12,doubleprec ;double precision destination
                set   r2,r2,1<underflow>  ;set underflow flag in FPSR
singleprec:     or.u  r6,r0,0x0c00 ;load exponent adjust 192
                br.n  callundhand  ;branch to call handler for user handler
                add   r12,r6,r12   ;adjust single precision exponent
doubleprec:     or.u  r6,r0,0x6000 ;load exponent adjust 1536
                add   r12,r6,r12   ;adjust double precision exponent
callundhand: 
	sub	r3,r0,1
	jmp	r1


;Now the floating point number, which has an exponent smaller than what
;IEEE allows, must be denormalized.  Denormalization is done by calculating
;the difference between a denormalized exponent and an underflow exponent and
;shifting the mantissa by that amount.  A one may need to be subtracted from 
;the LSB if a one was added during rounding.
;r9 is used to contain the guard, round, sticky, and an inaccuracy bit in
;case some bits were shifted off the mantissa during denormalization.
;r9 will contain: bit 4 -- new addone if one added during rounding 
;                          after denormalization
;                 bit 3 -- inaccuracy flag caused by denormalization
;			   or pre-denormalization inexactness
;                 bit 2 -- guard bit of result
;                 bit 1 -- round bit of result
;                 bit 0 -- sticky bit of result

denorm:         bb1.n destsize,r12,double ;denorm for double
                extu  r9,r10,3<26>   ;load r9 with grs
single:         mak   r5,r10,21<3> ;extract high 21 bits of mantissa
                extu  r6,r11,3<29> ;extract low 3 bits of mantissa
                or    r11,r5,r6     ;form 24 bits of mantissa

;See if the addone bit is set and unround if it is.
                bb0.n 25,r10,nounrounds ;do not unround if addone bit clear
                extu  r6,r12,12<20>  ;extract signed exponent from IMPCR
unrounds:       subu  r11,r11,1      ;subtract 1 from mantissa
;If the hidden bit is cleared after subtracting the one, then the one added
;during the rounding must have propagated through the mantissa.  The exponent
;will need to be decremented.
                bb1   23,r11,nounrounds ;if hidden bit is set,then exponent does
                                   ;not need to be decremented
decexps:        sub   r6,r6,1      ;decrement exponent 1
                set   r11,r11,1<23>  ;set the hidden bit

;For both single and double precision, there are cases where it is easier
;and quicker to make a special case.  Examples of this are if the shift 
;amount is only 1 or 2, or all the mantissa is shifted off, or all the
;mantissa is shifted off and it is still shifting, or, in the case of 
;doubles, if the shift amount is around the boundary of MANTLO and MANTHI.

nounrounds:     ; ld    r8,r0,zeroexp  ;load r8 with -127
		or.u	r8,r0,hi16(zeroexp)
		ld	r8,r8,lo16(zeroexp)
                sub   r7,r8,r6     ;find difference between two exponents,
                                   ;this amount is the shift amount
                cmp   r6,r7,3      ;check to see if r7 contains 3 or more
                bb1   ge,r6,threesing ;br to code that handles shifts of >=3
                cmp   r6,r7,2      ;check to see if r7 contains 2
                bb1   eq,r6,twosing ;br to code that handles shifts of 2
one:            rot   r9,r9,0<1>   ;rotate roundoff register once, this places
                                   ;guard in round and round in sticky
                bb0   31,r9,nosticky1s;do not or round and sticky if sticky is
                                   ;0, this lost bit will be cleared later
                set   r9,r9,1<0>   ;or round and sticky
nosticky1s:     bb0   0,r11,guardclr1s ;do not set guard bit if LSB = 0
                set   r9,r9,1<2>   ;set guard bit       
guardclr1s:     extu  r11,r11,31<1> ;shift mantissa right 1
                br.n  round        ;round result
                mak   r9,r9,3<0>   ;clear bits lost during rotation

twosing:        rot   r9,r9,0<2>   ;rotate roundff register twice, this places
                                   ;guard in sticky
                bb0   30,r9,nosticky2s ;do not or guard and sticky if stick is 0
                                   ;this lost bit will be cleared later
                br.n  noround2s    ;skip or old guard and old round if old
                                   ;sticky set
                set   r9,r9,1<0>   ;or guard and sticky
nosticky2s:     bb0   31,r9,noround2s ;do not or guard and round if round is 0
                                   ;this lost bit will be cleared later
                set   r9,r9,1<0>   ;or guard and round
noround2s:      bb0   0,r11,roundclr2s ;do not set round bit if LSB = 0
                set   r9,r9,1<1>   ;set round bit
roundclr2s:     bb0   1,r11,guardclr2s ;do not set guard bit if LSB + 1 = 0
                set   r9,r9,1<2>   ;set guard bit
guardclr2s:     extu  r11,r11,30<2>  ;shift mantissa right 2
                br.n  round        ;round result
                mak   r9,r9,3<0>   ;clear bits lost during rotation

threesing:      bb1   0,r9,noguard3s ;check sticky initially
                                     ;sticky is set, forget most of the oring
nosticky3s:     bb0   1,r9,noround3s  ;check round initially, do not set sticky
                br.n  noguard3s    ;forget most of the rest of oring
                set   r9,r9,1<0>      ;if round is clear,set sticky if round set
noround3s:      bb0.n 2,r9,noguard3s  ;check guard initially, do not set sticky
                clr   r9,r9,2<1>   ;clear the original guard and round for when
 				      ;you get to round section
                set   r9,r9,1<0>      ;if guard is clear,set sticky if guard set
noguard3s:      cmp   r6,r7,23     ;check if # of shifts is <=23
                bb1   gt,r6,s24    ;branch to see if shifts = 24
                sub   r6,r7,2      ;get number of bits to check for sticky
                mak   r6,r6,5<5>   ;shift width into width field
                mak   r8,r11,r6     ;mask off shifted bits -2
                ff1   r8,r8        ;see if r8 has any ones
                bb1   5,r8,nostky23 ;do not set sticky if no ones found
                set   r9,r9,1<0>   ;set sticky bit
nostky23:       or    r8,r0,34     ;start code to get new mantissa plus two
                                   ;extra bits for new round and new guard bits
                subu  r8,r8,r7     
                mak   r8,r8,5<5>   ;shift field width into second five bits
                extu  r6,r6,5<5>   ;shift previous shifted -2 into offset field
                or    r6,r6,r8     ;complete field
                extu  r11,r11,r6     ;form new mantissa with two extra bits

                bb0   0,r11,nornd3s ;do not set new round bit
                set   r9,r9,1<1>   ;set new round bit
nornd3s:        bb0   1,r11,nogrd3s ;do not set new guard bit
                set   r9,r9,1<2>   ;set new guard bit
nogrd3s:        br.n  round        ;round mantissa
                extu  r11,r11,30<2>  ;shift off remaining two bits

s24:            cmp   r6,r7,24     ;check to see if # of shifts is 24
                bb1   gt,r6,s25    ;branch to see if shifts = 25
                bb1   0,r9,nostky24 ;skip checking if old sticky set
                extu  r8,r11,22<0>  ;prepare to check bits that will be shifted
                                   ;into the sticky
                ff1   r8,r8        ;see if there are any 1's
                bb1   5,r8,nostky24 ;do not set sticky if no ones found
                set   r9,r9,1<0>   ;set sticky bit
nostky24:       bb0   22,r11,nornd24 ;do not set new round bit
                set   r9,r9,1<1>   ;set new round bit
nornd24:        set   r9,r9,1<2>   ;set new guard bit,this is hidden bit
                br.n  round        ;round mantissa
                or    r11,r0,r0     ;clear r11, all of mantissa shifted off
                
s25:            cmp   r6,r7,25     ;check to see if # of shifts is 25
                bb1   gt,r6,s26    ;branch to execute for shifts => 26
                bb1   0,r9,nostky25 ;skip checking if old sticky set
                extu  r8,r11,23<0> ;prepare to check bits that will be shifted
                                   ;into the sticky
                ff1   r8,r8        ;see if there are any 1's
                bb1   5,r8,nostky25 ;do not set sticky if no ones found
                set   r9,r9,1<0>   ;set sticky bit
nostky25:       set   r9,r9,1<1>   ;set new round bit,this is hidden bit
                clr   r9,r9,1<2>   ;clear guard bit since nothing shifted in
                br.n  round        ;round and assemble result
                or    r11,r0,r0    ;clear r11, all of mantissa shifted off

s26:            set   r9,r9,1<0>   ;set sticky bit,this contains hidden bit
                clr   r9,r9,2<1>   ;clear guard and round bits since nothing 
                                   ;shifted in 
                br.n  round        ;round and assemble result
                or    r11,r0,r0    ;clear mantissa

double:         mak   r5,r10,21<0> ;extract upper bits of mantissa
                bb0.n 25,r10,nounroundd ;do not unround if addone bit clear
                extu  r6,r12,12<20>;extract signed exponenet from IMPCR
unroundd:       or    r8,r0,1
		subu.co  r11,r11,r8     ;subtract 1 from mantissa
                subu.ci  r5,r5,r0       ;subtract borrow from upper word
                bb1   20,r5,nounroundd ;if hidden bit is set, then exponent does
                                       ;not need to be decremented
decexpd:        sub   r6,r6,1      ;decrement exponent 1
                set   r5,r5,1<20>  ;set the hidden bit

nounroundd:     ; ld    r8,r0,zeroexp+4 ;load r8 with -1023
		or.u	r8,r0,hi16(zeroexp+4)
		ld	r8,r8,lo16(zeroexp+4)
                sub   r7,r8,r6     ;find difference between two exponents,
                                   ;this amount is the shift amount
                cmp   r6,r7,3      ;check to see if r7 contains 3 or more
                bb1   ge,r6,threedoub ;br to code that handles shifts of >=3
                cmp   r6,r7,2      ;check to see if r7 contains 2
                bb1   eq,r6,twodoub ;br to code that handles shifts of 2

onedoub:        rot   r9,r9,0<1>   ;rotate roundoff register once, this places
                                   ;guard in round and round in sticky
                bb0   31,r9,nosticky1d;do not or round and sticky if sticky is 0
                                   ;this lost bit will be cleared later
                set   r9,r9,1<0>   ;or old round and old sticky into new sticky
nosticky1d:     bb0   0,r11,guardclr1d ;do not set new guard bit if old LSB = 0
                set   r9,r9,1<2>   ;set new guard bit
guardclr1d:     extu  r11,r11,31<1> ;shift lower mantissa over 1
                mak   r6,r5,1<31>  ;shift off low bit of high mantissa
                or    r11,r6,r11   ;load high bit onto lower mantissa
                extu  r5,r5,20<1>  ;shift right once upper 20 bits of mantissa
                br.n  round        ;round mantissa and assemble result
                mak   r9,r9,3<0>   ;clear bits lost during rotation

twodoub:        rot   r9,r9,0<2>   ;rotate roundoff register twice, this places
                                   ;old guard into sticky
                bb0   30,r9,nosticky2d ;do not or old guard and old sticky if 
                                       ;old sticky is 0
                br.n  noround2d    ;skip or of old guard and old round if old
                                   ;sticky set
                set   r9,r9,1<0>   ;or old guard and old sticky into new sticky
nosticky2d:     bb0   31,r9,noround2d ;do not or old guard and old round if
                                      ;old round is 0
                set   r9,r9,1<0>   ;or old guard and old round into new sticky
noround2d:      bb0   0,r11,roundclr2d ;do not set round bit if old LSB = 0
                set   r9,r9,1<1>   ;set new round bit
roundclr2d:     bb0   1,r11,guardclr2d ;do not set guard bit if old LSB + 1 = 0
                set   r9,r9,1<2>   ;set new guard bit
guardclr2d:     extu  r11,r11,30<2> ;shift lower mantissa over 2
                mak   r6,r5,2<30>  ;shift off low bits of high mantissa
                or    r11,r6,r11   ;load high bit onto lower mantissa
                extu  r5,r5,19<2>  ;shift right twice upper 19 bits of mantissa
                br.n  round        ;round mantissa and assemble result
                mak   r9,r9,3<0>   ;clear bits lost during rotation

threedoub:      bb1   0,r9,noguard3d ;checky sticky initially
                                    ;sticky is set, forget most of rest of oring
nosticky3d:     bb0   1,r9,noround3d ;check old round, do not set sticky if 
                                     ;old round is clear, set otherwise
                br.n  noguard3d    ;sticky is set, forget most of rest of oring
                set   r9,r9,1<0>   ;set sticky if old round is set
noround3d:      bb0.n 2,r9,noguard3d ;check old guard, do not set sticky if 0
                clr   r9,r9,2<1>   ;clear the original guard and round for when
 				      ;you get to round section
                set   r9,r9,1<0>   ;set sticky if old guard is set
noguard3d:      cmp   r6,r7,32     ;do I need to work with a 1 or 2 word mant.
                                   ;when forming sticky, round and guard
                bb1   gt,r6,d33    ;jump to code that handles 2 word mantissas
                sub   r6,r7,2      ;get number of bits to check for sticky
                mak   r6,r6,5<5>   ;shift width into width field
                mak   r8,r11,r6    ;mask off shifted bits -2
                ff1   r8,r8        ;see if r8 has any ones
                bb1   5,r8,nostky32 ;do not set sticky if no ones found
                set   r9,r9,1<0>   ;set sticky bit
nostky32:       or    r8,r0,34     ;start code to get new mantissa plus two
                                   ;extra bits for new round and new guard bits,
                                   ;the upper word bits will be shifted after
                                   ;the round and guard bits are handled
                subu  r8,r8,r7     
                mak   r8,r8,5<5>   ;shift field width into second five bits
                extu  r6,r6,5<5>   ;shift previous shifted -2 into offset field
                or    r6,r6,r8     ;complete bit field
                extu  r11,r11,r6   ;partially form new low mantissa with 2 more 
                                   ;bits
                bb0   0,r11,nornd32d ;do not set new round bit
                set   r9,r9,1<1>   ;set new round bit
nornd32d:       bb0   1,r11,nogrd32d ;do not set new guard bit
                set   r9,r9,1<2>   ;set new guard bit
nogrd32d:       extu  r11,r11,30<2> ;shift off remaining two bits
                mak   r6,r7,5<5>   ;shift field width into second 5 bits, if the
                                   ;width is 32, then these bits will be 0
                or    r8,r0,32     ;load word length into r8
                sub   r8,r8,r7     ;form offset for high bits moved to low word
                or    r6,r6,r8     ;form complete bit field
                mak   r6,r5,r6     ;get shifted bits of high word
                or    r11,r6,r11   ;form new low word of mantissa
		bcnd  ne0,r8,regular33 ;do not adjust for special case of r8
		br.n  round	       ;containing zeros, which would cause
		or    r5,r0,r0         ;all of the bits to be extracted under
				       ;the regular method
regular33:      mak   r6,r7,5<0>   ;place lower 5 bits of shift into r6
                mak   r8,r8,5<5>   ;shift r8 into width field
                or    r6,r6,r8     ;form field for shifting of upper bits
                br.n  round        ;round and assemble result
                extu  r5,r5,r6     ;form new high word mantissa

d33:            cmp   r6,r7,33     ;is the number of bits to be shifted is 33?
                bb1   gt,r6,d34    ;check to see if # of bits is 34
                bb1   0,r9,nostky33 ;skip checking if old sticky set
                mak   r6,r11,31<0> ;check bits that will be shifted into sticky
                ff1   r8,r8        ;check for ones
                bb1   5,r8,nostky33 ;do not set sticky if there are no ones
                set   r9,r9,1<0>   ;set new sticky bit
nostky33:       bb0   31,r11,nornd33 ;do not set round if bit is not a 1
                set   r9,r9,1<1>   ;set new round bit
nornd33:        bb0   0,r5,nogrd33 ;do not set guard bit if bit is not a 1
                set   r9,r9,1<2>   ;set new guard bit
nogrd33:        extu  r11,r5,31<1> ;shift high bits into low word
                br.n  round        ;round and assemble result
                or    r5,r0,r0     ;clear high word

d34:            cmp   r6,r7,34     ;is the number of bits to be shifted 34?
                bb1   gt,r6,d35    ;check to see if # of bits is >= 35
                bb1   0,r9,nostky34 ;skip checking if old sticky set
                ff1   r8,r11       ;check bits that will be shifted into sticky
                bb1   5,r8,nostky34 ;do not set sticky if there are no ones
                set   r9,r9,1<0>   ;set new sticky bit
nostky34:       bb0   0,r5,nornd34 ;do not set round if bit is not a 1
                set   r9,r9,1<1>   ;set new round bit
nornd34:        bb0   1,r5,nogrd34 ;do not set guard bit if bit is not a 1
                set   r9,r9,1<2>   ;set new guard bit
nogrd34:        extu  r11,r5,30<2> ;shift high bits into low word
                br.n  round        ;round and assemble result
                or    r5,r0,r0     ;clear high word

d35:            cmp   r6,r7,52     ;see if # of shifts is 35 <= X <= 52
                bb1   gt,r6,d53    ;check to see if # of shifts is 52
                bb1.n 0,r9,nostky35 ;skip checking if old sticky set
                sub   r7,r7,34     ;subtract 32 from # of shifts so that opera-
                                   ;tions can be done on the upper word, and 
                                   ;then subtract two more checking guard and
                                   ;sticky bits
                ff1   r8,r11       ;see if lower word has a bit for sticky
                bb1   5,r8,stkycheck35 ;see if upper word has any sticky bits   
                br.n  nostky35     ;quit checking for sticky
                set   r9,r9,1<0>   ;set sticky bit
stkycheck35:    mak   r6,r7,5<5>   ;place width into width field
                mak   r8,r5,r6     ;mask off shifted bits - 2
                ff1   r8,r8        ;see if r8 has any ones
                bb1   5,r8,nostky35 ;do not set sticky if no ones found
                set   r9,r9,1<0>   ;set sticky bit
nostky35:       or    r8,r0,32     ;look at what does not get shifted off plus
                                   ;round and sticky, remember that the r7 value
                                   ;was adjusted so that it did not include
                                   ;new round or new sticky in shifted off bits
                subu  r8,r8,r7     ;complement width 
                mak   r8,r8,5<5>   ;shift width into width field
                or    r8,r7,r8     ;add offset field
                extu  r11,r5,r8    ;extract upper bits into low word
                bb0   0,r11,nornd35 ;do not set new round bit
                set   r9,r9,1<1>   ;set new round bit
nornd35:        bb0   1,r11,nogrd35 ;do not set new guard bit
                set   r9,r9,1<2>   ;set new guard bit
nogrd35:        extu  r11,r11,30<2> ;shift off remaining guard and round bits
                br.n  round         ;round and assemble result
                or    r5,r0,r0      ;clear high word

d53:            cmp   r6,r7,53     ;check to see if # of shifts is 53
                bb1   gt,r6,d54    ;branch to see if shifts = 54
                bb1   0,r9,nostky53 ;skip checking if old sticky set
                ff1   r8,r11       ;see if lower word has a bit for sticky
                bb1   5,r8,stkycheck53 ;see if upper word has any sticky bits   
                br.n  nostky53     ;quit checking for sticky
                set   r9,r9,1<0>   ;set sticky bit
stkycheck53:    mak   r6,r5,19<0>  ;check bits that are shifted into sticky
                ff1   r8,r6        ;see if r6 has any ones
                bb1   5,r8,nostky53 ;do not set sticky if no ones found
                set   r9,r9,1<0>   ;set sticky bit
nostky53:       bb0   19,r5,nornd53 ;do not set new round bit
                set   r9,r9,1<1>   ;set new round bit
nornd53:        set   r9,r9,1<2>   ;set new guard bit,this is hidden bit
                or    r5,r0,r0     ;clear high word
                br.n  round        ;round and assemble result
                or    r11,r0,r0    ;clear low word

d54:            cmp   r6,r7,54     ;check to see if # of shifts is 54
                bb1   gt,r6,d55    ;branch to execute for shifts =>55
                bb1   0,r9,nostky54 ;skip checking if old sticky set
                ff1   r8,r11       ;see if lower word has a bit for sticky
                bb1   5,r8,stkycheck54 ;see if upper word has any sticky bits   
                br.n  nostky54     ;quit checking for sticky
                set   r9,r9,1<0>   ;set sticky bit
stkycheck54:    mak   r6,r5,20<0>  ;check bits that are shifted into sticky
                ff1   r8,r6        ;see if r6 has any ones
                bb1   5,r8,nostky54 ;do not set sticky if no ones found
                set   r9,r9,1<0>   ;set sticky bit
nostky54:       set   r9,r9,1<1>   ;set new round bit,this is hidden bit
                clr   r9,r9,1<2>   ;clear guard bit since nothing shifted in
                or    r5,r0,r0     ;clear high word
                br.n  round        ;round and assemble result
                or    r11,r0,r0    ;clear low word

d55:            set   r9,r9,1<0>   ;set new sticky bit,this contains hidden bit
                clr   r9,r9,2<1>   ;clear guard and round bits since nothing
                                   ;shifted in
                or    r5,r0,r0     ;clear high word
                or    r11,r0,r0    ;clear low word


;The first item that the rounding code does is see if either guard, round,
;or sticky is set.  If all are clear, then there is no denormalization loss
;and no need to round, then branch to assemble answer.
;For rounding, a branch table is set up.  The left two most bits are the 
;rounding mode.  The third bit is either the LSB of the mantissa or the
;sign bit, depending on the rounding mode.  The three LSB's are the guard,
;round and sticky bits.

round:          ff1   r8,r9         ;see if there is denormalization loss
                bb1   5,r8,assemble ;no denormalization loss or inexactness
                extu  r6,r10,2<modelo>   ;extract rounding mode
                bb1.n modehi,r10,signext ;use sign bit instead of LSB
                mak   r6,r6,2<4>    ;shift over rounding mode
                extu  r7,r11,1<0>   ;extract LSB
                br.n  grs           ;skip sign extraction
                mak   r7,r7,1<3>    ;shift over LSB
signext:        extu  r7,r10,1<31>  ;extract sign bit
                mak   r7,r7,1<3>    ;shift sign bit over
grs:            or    r6,r6,r7
                or    r6,r6,r9      ;or in guard, round, and sticky
                lda   r6,r0[r6]     ;scale offset into branch table
                ; lda   r6,r6,roundtable ;add on address of branch table
		or.u	r6,r6,hi16(roundtable)
		add	r6,r6,lo16(roundtable)
                jmp.n r6            ;jump to branch table
                set   r9,r9,1<3>    ;set inexact flag in r9

roundtable:     br    noaddone
r000001:        br    noaddone
r000010:        br    noaddone
r000011:        br    noaddone
r000100:        br    noaddone
r000101:        br    addone
r000110:        br    addone
r000111:        br    addone
r001000:        br    noaddone
r001001:        br    noaddone
r001010:        br    noaddone
r001011:        br    noaddone
r001100:        br    addone
r001101:        br    addone
r001110:        br    addone
r001111:        br    addone
r010000:        br    noaddone
r010001:        br    noaddone
r010010:        br    noaddone
r010011:        br    noaddone
r010100:        br    noaddone
r010101:        br    noaddone
r010110:        br    noaddone
r010111:        br    noaddone
r011000:        br    noaddone
r011001:        br    noaddone
r011010:        br    noaddone
r011011:        br    noaddone
r011100:        br    noaddone
r011101:        br    noaddone
r011110:        br    noaddone
r011111:        br    noaddone
r100000:        br    noaddone
r100001:        br    noaddone
r100010:        br    noaddone
r100011:        br    noaddone
r100100:        br    noaddone
r100101:        br    noaddone
r100110:        br    noaddone
r100111:        br    noaddone
r101000:        br    noaddone
r101001:        br    addone
r101010:        br    addone
r101011:        br    addone
r101100:        br    addone
r101101:        br    addone
r101110:        br    addone
r101111:        br    addone
r110000:        br    noaddone
r110001:        br    addone
r110010:        br    addone
r110011:        br    addone
r110100:        br    addone
r110101:        br    addone
r110110:        br    addone
r110111:        br    addone
r111000:        br    noaddone
r111001:        br    noaddone
r111010:        br    noaddone
r111011:        br    noaddone
r111100:        br    noaddone
r111101:        br    noaddone
r111110:        br    noaddone
r111111:        br    noaddone

;Round by adding a one to the LSB of the mantissa.
addone:         or    r6,r0,1      ;load a 1 into r6 so that add.co can be used
D 2
                add.co r11,r11,r6  ;add a one to the lower word of result
E 2
I 2
                addu.co r11,r11,r6  ;add a one to the lower word of result
E 2
                bb0.n destsize,r12,noaddone ;single result,forget carry
                set   r9,r9,1<4>   ;indicate that a 1 has been added
                add.ci r5,r5,r0    ;propagate carry into high word


;Branch to inexact user handler if there is one.

noaddone:       bb1.n efinx,r12,modformdef ;branch to modify form for user 
                                             ;handler
                or    r2,r2,5      ;set inexact and underflow flags


;Assemble the result of the denormalization routine for writeback to the 
;destination register.  The exponent of a denormalized number is zero,
;so simply assemble the sign and the new mantissa.

assemble:       bb1   destsize,r12,doubassem ;assemble double result
                bb0   sign,r10,exassems ;exit assemble if sign is zero
                set   r11,r11,1<sign>  ;make result negative
exassems:       br    return       ;return from subroutine

doubassem:      bb0.n sign,r10,signclr ;do not set sign in r10
		or    r10,r5,r0    ;load high word from r5 into r10
                set   r10,r10,1<sign> ;high word with sign loaded
signclr:        br    return       ;return from subroutine
                

;modfordef modifies the result of denormalization to the input format of
;the inexact user handler.  This input format is the same format that 
;MANTHI, MANTLO, and IMPCR were initially loaded with.

modformdef:     clr   r12,r12,12<20> ;clear result exponent,IMPCR complete
                clr   r10,r10,4<25>  ;clear old guard,round,sticky,and addone
                mak   r5,r9,3<26>    ;make grs field
                bb0.n 4,r9,newaddone ;do not set new addone in MANTHI
                or    r10,r5,r10     ;or in new grs field
                set   r10,r10,1<25>  ;set new addone
newaddone:
                clr   r10,r10,21<0>  ;clear upper bits of old mantissa
                bb1 destsize,r12,moddefd ;branch to handle double precision
moddefs:        extu  r5,r11,20<3>   ;extract upper bits
                or    r10,r5,r10     ;MANTHI complete
                rot   r11,r11,0<3>   ;MANTLO complete
	sub	r3,r0,1
	jmp	r1
moddefd:
                or    r10,r5,r10     ;MANTHI complete,r5 should be set to OR
	sub	r3,r0,1
	jmp	r1


;Return to fpui.

return:         ;ld    r1,r31,XRETADDR ;load return address
                jmp   r1           ;return from subroutine
  
                data
zeroexp:        word  0x00000f81   ;-127 in decimal for lowest 12 bits
                word  0x00000c01   ;-1023 in decimal for lowest 12 bits




E 1

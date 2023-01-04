	file	"init_color.c"
	text
	align	4
	global	_init_color
_init_color:
	subu	r31,r31,104
	st	r19,r31,72
	st	r20,r31,76
	st	r21,r31,80
	st	r22,r31,84
	st	r23,r31,88
	st	r24,r31,92
	st	r25,r31,96
	st	r1,r31,100
	def	CTMP1,48
                           
;                                  
;                                  
;                                  
;                                  
;                                  
;                                  
;                                  
;                                  
	or	r25,r0,r2
	ext	r25,r25,16<0>
	or	r24,r0,r3
	ext	r24,r24,16<0>
	or	r23,r0,r4
	ext	r23,r23,16<0>
	or	r22,r0,r5
	ext	r22,r22,16<0>
	or.u	r13,r0,hi16(_SP)
	ld	r2,r13,lo16(_SP)
	ld	r21,r2,132
	or.u	r13,r0,hi16(_cur_bools)
	ld	r2,r13,lo16(_cur_bools)
	ld.b	r2,r2,27
	bcnd	eq0,r2,@L99999
	or.u	r13,r0,hi16(_COLORS)
	ld	r2,r13,lo16(_COLORS)
	or	r3,r0,r25
	cmp	r13,r3,r2
	bb1	ge,r13,@L99999
	bcnd	ge0,r25,@L198
@L99999:
	subu	r2,r0,0x1
	br 	@L193
@L198:
	cmp	r13,r24,1000
	bb1	le,r13,@L199
	addu	r24,r0,0x3e8
@L199:
	cmp	r13,r23,1000
	bb1	le,r13,@L200
	addu	r23,r0,0x3e8
@L200:
	cmp	r13,r22,1000
	bb1	le,r13,@L201
	addu	r22,r0,0x3e8
@L201:
	bcnd	ge0,r24,@L202
	addu	r24,r0,0x0
@L202:
	bcnd	ge0,r23,@L203
	addu	r23,r0,0x0
@L203:
	bcnd	ge0,r22,@L204
	addu	r22,r0,0x0
@L204:
	addu	r2,r0,0x6
	or	r3,r0,r25
	mul	r3,r3,r2
	addu	r2,r21,r3
	ld.h	r2,r2,0
	or	r3,r0,r24
	cmp	r13,r2,r3
	bb1	ne,r13,@L205
	addu	r2,r0,0x6
	or	r3,r0,r25
	mul	r3,r3,r2
	addu	r2,r21,r3
	ld.h	r2,r2,2
	or	r3,r0,r23
	cmp	r13,r2,r3
	bb1	ne,r13,@L205
	addu	r2,r0,0x6
	or	r3,r0,r25
	mul	r3,r3,r2
	addu	r2,r21,r3
	ld.h	r2,r2,4
	or	r3,r0,r22
	cmp	r13,r2,r3
	bb1	ne,r13,@L205
	addu	r2,r0,0x0
	br 	@L193
@L205:
	addu	r2,r0,0x6
	or	r3,r0,r25
	mul	r3,r3,r2
	addu	r2,r21,r3
	st.h	r24,r2,0
	addu	r2,r0,0x6
	or	r3,r0,r25
	mul	r3,r3,r2
	addu	r2,r21,r3
	st.h	r23,r2,2
	addu	r2,r0,0x6
	or	r3,r0,r25
	mul	r3,r3,r2
	addu	r2,r21,r3
	st.h	r22,r2,4
	or.u	r13,r0,hi16(_cur_strs)
	ld	r2,r13,lo16(_cur_strs)
	ld	r2,r2,1196
	bcnd	eq0,r2,@L206
	or.u	r13,r0,hi16(_cur_bools)
	ld	r2,r13,lo16(_cur_bools)
	ld.b	r2,r2,29
	bcnd	eq0,r2,@L207
	addu	r2,r31,36
	subu	r31,r31,8
	st	r2,r31,0
	addu	r9,r31,48
	addu	r8,r31,40
	flt.ss	r6,r22
	fadd.dss	r6,r0,r6
	flt.ss	r4,r23
	fadd.dss	r4,r0,r4
	flt.ss	r2,r24
	fadd.dss	r2,r0,r2
	subu	r31,r31,32
	bsr	__rgb_to_hls
	addu	r31,r31,40
	or.u	r13,r0,hi16(__outch)
	addu	r2,r13,lo16(__outch)
	st	r2,r31,CTMP1+8
	addu	r2,r0,0x1
	st	r2,r31,CTMP1+4
	ld	r6,r31,36
	ld	r5,r31,40
	ld	r4,r31,32
	or	r3,r0,r25
	or.u	r13,r0,hi16(_cur_strs)
	ld	r2,r13,lo16(_cur_strs)
	ld	r7,r2,1196
	or	r2,r0,r7
	bsr	_tparm
	ld	r3,r31,CTMP1+4
	ld	r4,r31,CTMP1+8
	bsr	_tputs
	br 	@L209
@L207:
	or.u	r13,r0,hi16(__outch)
	addu	r2,r13,lo16(__outch)
	st	r2,r31,CTMP1+16
	addu	r2,r0,0x1
	st	r2,r31,CTMP1+12
	or	r6,r0,r22
	or	r5,r0,r23
	or	r4,r0,r24
	or	r3,r0,r25
	or.u	r13,r0,hi16(_cur_strs)
	ld	r2,r13,lo16(_cur_strs)
	ld	r7,r2,1196
	or	r2,r0,r7
	bsr	_tparm
	ld	r3,r31,CTMP1+12
	ld	r4,r31,CTMP1+16
	bsr	_tputs
@L209:
	br 	@L210
@L206:
	or.u	r13,r0,hi16(_SP)
	ld	r2,r13,lo16(_SP)
	ld	r19,r2,128
	addu	r20,r0,0x0
	or.u	r13,r0,hi16(_COLOR_PAIRS)
	ld	r2,r13,lo16(_COLOR_PAIRS)
	cmp	r13,r20,r2
	bb1	ge,r13,@L212
@L213:
	mul	r2,r20,6
	addu	r3,r19,r2
	ld.h	r2,r3,0
	or	r3,r0,r25
	cmp	r13,r2,r3
	bb1	eq,r13,@L99998
	mul	r2,r20,6
	addu	r3,r19,r2
	ld.h	r2,r3,2
	or	r3,r0,r25
	cmp	r13,r2,r3
	bb1	ne,r13,@L214
@L99998:
	mul	r2,r20,6
	addu	r4,r19,r2
	ld.h	r2,r4,2
	or	r4,r0,r2
	mul	r2,r20,6
	addu	r3,r19,r2
	ld.h	r2,r3,0
	or	r3,r0,r2
	or	r2,r0,r20
	bsr	__init_HP_pair
@L214:
	addu	r20,r20,1
	or.u	r13,r0,hi16(_COLOR_PAIRS)
	ld	r2,r13,lo16(_COLOR_PAIRS)
	cmp	r13,r20,r2
	bb1	lt,r13,@L213
@L212:
@L210:
	addu	r2,r0,0x0
	br 	@L193
@L193:
	ld	r19,r31,72
	ld	r20,r31,76
	ld	r21,r31,80
	ld	r22,r31,84
	ld	r23,r31,88
	ld	r24,r31,92
	ld	r25,r31,96
	ld	r1,r31,100
	jmp.n	r1
	addu	r31,r31,104
	align	4
	global	__rgb_to_hls
__rgb_to_hls:
	subu	r31,r31,112
	st	r14,r31,56
	st	r15,r31,60
	st	r16,r31,64
	st	r17,r31,68
	st	r18,r31,72
	st	r19,r31,76
	st	r20,r31,80
	st	r21,r31,84
	st	r22,r31,88
	st	r23,r31,92
	st	r24,r31,96
	st	r25,r31,100
	st	r30,r31,104
	st	r1,r31,108
	addu	r30,r31,112
   
;                                  
;                                  
;                                  
;                                  
;                                  
;                                  
	or	r24,r0,r2
	or	r25,r0,r3
	or	r22,r0,r4
	or	r23,r0,r5
	or	r20,r0,r6
	or	r21,r0,r7
	st	r8,r30,24
	st	r9,r30,28
	or.u	r13,r0,hi16(@L220)
	ld.d	r2,r13,lo16(@L220)
	fdiv.ddd	r24,r24,r2
	or.u	r13,r0,hi16(@L221)
	ld.d	r2,r13,lo16(@L221)
	fdiv.ddd	r22,r22,r2
	or.u	r13,r0,hi16(@L222)
	ld.d	r2,r13,lo16(@L222)
	fdiv.ddd	r20,r20,r2
	or	r6,r0,r20
	or	r7,r0,r21
	or	r4,r0,r22
	or	r5,r0,r23
	or	r2,r0,r24
	or	r3,r0,r25
	bsr	_MAX
	fadd.dss	r2,r0,r2
	st.d	r2,r31,48
	ld.d	r2,r31,48
	st.d	r2,r31,32
	or	r6,r0,r20
	or	r7,r0,r21
	or	r4,r0,r22
	or	r5,r0,r23
	or	r2,r0,r24
	or	r3,r0,r25
	bsr	_MIN
	fadd.dss	r2,r0,r2
	st.d	r2,r31,48
	ld.d	r2,r31,48
	st.d	r2,r31,40
	ld.d	r2,r31,32
	ld.d	r4,r31,40
	fadd.ddd	r6,r2,r4
	or.u	r13,r0,hi16(@L223)
	ld.d	r2,r13,lo16(@L223)
	fdiv.ddd	r4,r6,r2
	fadd.ssd	r2,r0,r4
	or	r15,r0,r2
	ld.d	r2,r31,32
	ld.d	r4,r31,40
	fcmp.sdd	r13,r2,r4
	bb1	ne,r13,@L224
	or.u	r13,r0,hi16(@L225)
	ld	r14,r13,lo16(@L225)
	or.u	r13,r0,hi16(@L226)
	ld	r16,r13,lo16(@L226)
	br 	@L227
@L224:
	fadd.dss	r2,r0,r15
	or.u	r13,r0,hi16(@L229)
	ld.d	r4,r13,lo16(@L229)
	fcmp.sdd	r13,r2,r4
	bb1	ge,r13,@L228
	ld.d	r2,r31,32
	ld.d	r4,r31,40
	fsub.ddd	r6,r2,r4
	ld.d	r2,r31,32
	ld.d	r4,r31,40
	fadd.ddd	r8,r2,r4
	fdiv.ddd	r2,r6,r8
	fadd.ssd	r2,r0,r2
	or	r14,r0,r2
	br 	@L230
@L228:
	ld.d	r2,r31,32
	ld.d	r4,r31,40
	fsub.ddd	r6,r2,r4
	or.u	r13,r0,hi16(@L231)
	ld.d	r2,r13,lo16(@L231)
	ld.d	r4,r31,32
	fsub.ddd	r8,r2,r4
	ld.d	r2,r31,40
	fsub.ddd	r4,r8,r2
	fdiv.ddd	r2,r6,r4
	fadd.ssd	r2,r0,r2
	or	r14,r0,r2
@L230:
	ld.d	r2,r31,32
	fsub.ddd	r4,r2,r24
	ld.d	r2,r31,32
	ld.d	r6,r31,40
	fsub.ddd	r8,r2,r6
	fdiv.ddd	r2,r4,r8
	fadd.ssd	r2,r0,r2
	or	r19,r0,r2
	ld.d	r2,r31,32
	fsub.ddd	r4,r2,r22
	ld.d	r2,r31,32
	ld.d	r6,r31,40
	fsub.ddd	r8,r2,r6
	fdiv.ddd	r2,r4,r8
	fadd.ssd	r2,r0,r2
	or	r18,r0,r2
	ld.d	r2,r31,32
	fsub.ddd	r4,r2,r20
	ld.d	r2,r31,32
	ld.d	r6,r31,40
	fsub.ddd	r8,r2,r6
	fdiv.ddd	r2,r4,r8
	fadd.ssd	r2,r0,r2
	or	r17,r0,r2
	ld.d	r2,r31,32
	fcmp.sdd	r13,r24,r2
	bb1	ne,r13,@L232
	fadd.dss	r2,r0,r17
	fadd.dss	r4,r0,r18
	fsub.ddd	r6,r2,r4
	fadd.ssd	r2,r0,r6
	or	r16,r0,r2
	br 	@L233
@L232:
	ld.d	r2,r31,32
	fcmp.sdd	r13,r22,r2
	bb1	ne,r13,@L234
	fadd.dss	r2,r0,r19
	or.u	r13,r0,hi16(@L235)
	ld.d	r4,r13,lo16(@L235)
	fadd.ddd	r6,r2,r4
	fadd.dss	r2,r0,r17
	fsub.ddd	r4,r6,r2
	fadd.ssd	r2,r0,r4
	or	r16,r0,r2
	br 	@L236
@L234:
	fadd.dss	r2,r0,r18
	or.u	r13,r0,hi16(@L237)
	ld.d	r4,r13,lo16(@L237)
	fadd.ddd	r6,r2,r4
	fadd.dss	r2,r0,r19
	fsub.ddd	r4,r6,r2
	fadd.ssd	r2,r0,r4
	or	r16,r0,r2
@L236:
@L233:
	fadd.dss	r2,r0,r16
	or.u	r13,r0,hi16(@L238)
	ld.d	r4,r13,lo16(@L238)
	tb1	0,r0,511
	fmul.ddd	r6,r2,r4
	fadd.ssd	r2,r0,r6
	or	r16,r0,r2
	fadd.dss	r2,r0,r16
	or.u	r13,r0,hi16(@L240)
	ld.d	r4,r13,lo16(@L240)
	fcmp.sdd	r13,r2,r4
	bb1	ge,r13,@L239
	fadd.dss	r2,r0,r16
	or.u	r13,r0,hi16(@L241)
	ld.d	r4,r13,lo16(@L241)
	fadd.ddd	r6,r2,r4
	fadd.ssd	r2,r0,r6
	or	r16,r0,r2
@L239:
	fadd.dss	r2,r0,r16
	or.u	r13,r0,hi16(@L242)
	ld.d	r4,r13,lo16(@L242)
	fadd.ddd	r6,r2,r4
	trnc.sd	r2,r6
	addu	r3,r0,0x168
	div	r4,r2,r3
	mul	r4,r4,r3
	subu	r4,r2,r4
	flt.ss	r2,r4
	or	r16,r0,r2
@L227:
	trnc.ss	r2,r16
	ld	r3,r30,24
	st	r2,r3,0
	fadd.dss	r2,r0,r14
	or.u	r13,r0,hi16(@L243)
	ld.d	r4,r13,lo16(@L243)
	tb1	0,r0,511
	fmul.ddd	r6,r2,r4
	trnc.sd	r2,r6
	ld	r3,r30,32
	st	r2,r3,0
	fadd.dss	r2,r0,r15
	or.u	r13,r0,hi16(@L244)
	ld.d	r4,r13,lo16(@L244)
	tb1	0,r0,511
	fmul.ddd	r6,r2,r4
	trnc.sd	r2,r6
	ld	r3,r30,28
	st	r2,r3,0
@L215:
	ld	r14,r31,56
	ld	r15,r31,60
	ld	r16,r31,64
	ld	r17,r31,68
	ld	r18,r31,72
	ld	r19,r31,76
	ld	r20,r31,80
	ld	r21,r31,84
	ld	r22,r31,88
	ld	r23,r31,92
	ld	r24,r31,96
	ld	r25,r31,100
	ld	r30,r31,104
	ld	r1,r31,108
	jmp.n	r1
	addu	r31,r31,112
	align	4
	global	_MAX
_MAX:
	subu	r31,r31,56
	st	r20,r31,32
	st	r21,r31,36
	st	r22,r31,40
	st	r23,r31,44
	st	r24,r31,48
	st	r25,r31,52

;                                  
;                                  
;                                  
;                                  
;                                  
;                                  
;                                  
;                                  
;                                  
;                                  
	or	r24,r0,r2
	or	r25,r0,r3
	or	r22,r0,r4
	or	r23,r0,r5
	or	r20,r0,r6
	or	r21,r0,r7
	fcmp.sdd	r13,r24,r22
	bb1	lt,r13,@L248
	fcmp.sdd	r13,r24,r20
	bb1	lt,r13,@L249
	fadd.ssd	r2,r0,r24
	br 	@L245
@L249:
	fadd.ssd	r2,r0,r20
	br 	@L245
@L248:
	fcmp.sdd	r13,r20,r22
	bb1	lt,r13,@L250
	fadd.ssd	r2,r0,r20
	br 	@L245
@L250:
	fadd.ssd	r2,r0,r22
	br 	@L245
@L245:
	ld	r20,r31,32
	ld	r21,r31,36
	ld	r22,r31,40
	ld	r23,r31,44
	ld	r24,r31,48
	ld	r25,r31,52
	jmp.n	r1
	addu	r31,r31,56
	align	4
	global	_MIN
_MIN:
	subu	r31,r31,56
	st	r20,r31,32
	st	r21,r31,36
	st	r22,r31,40
	st	r23,r31,44
	st	r24,r31,48
	st	r25,r31,52

;                                  
;                                  
;                                  
;                                  
;                                  
;                                  
;                                  
;                                  
;                                  
;                                  
	or	r24,r0,r2
	or	r25,r0,r3
	or	r22,r0,r4
	or	r23,r0,r5
	or	r20,r0,r6
	or	r21,r0,r7
	fcmp.sdd	r13,r24,r22
	bb1	le,r13,@L254
	fcmp.sdd	r13,r22,r20
	bb1	le,r13,@L255
	fadd.ssd	r2,r0,r20
	br 	@L251
@L255:
	fadd.ssd	r2,r0,r22
	br 	@L251
@L254:
	fcmp.sdd	r13,r24,r20
	bb1	ge,r13,@L256
	fadd.ssd	r2,r0,r24
	br 	@L251
@L256:
	fadd.ssd	r2,r0,r20
	br 	@L251
@L251:
	ld	r20,r31,32
	ld	r21,r31,36
	ld	r22,r31,40
	ld	r23,r31,44
	ld	r24,r31,48
	ld	r25,r31,52
	jmp.n	r1
	addu	r31,r31,56
	data
	data
	ident	"@(#)curses:init_color.c	1.5"
	ident	"@(#)curses:screen/curses_inc.h	1.2"
	ident	"@(#)curses:screen/curses.ed	1.33"
	ident	"@(#)curses:screen/maketerm.ed	1.34"
	ident	"@(#)curses:screen/curshdr.h	1.7"
	data
	data
	align	8
@L220:
	double	1.00000000000000000000e+03
	data
	align	8
@L221:
	double	1.00000000000000000000e+03
	data
	align	8
@L222:
	double	1.00000000000000000000e+03
	data
	align	8
@L223:
	double	2.00000000000000000000e+00
	data
	align	8
@L225:
	float	0.00000000000000000000e+00
	data
	align	8
@L226:
	float	0.00000000000000000000e+00
	data
	align	8
@L229:
	double	5.00000000000000000000e-01
	data
	align	8
@L231:
	double	2.00000000000000000000e+00
	data
	align	8
@L235:
	double	2.00000000000000000000e+00
	data
	align	8
@L237:
	double	4.00000000000000000000e+00
	data
	align	8
@L238:
	double	6.00000000000000000000e+01
	data
	align	8
@L240:
	double	0.00000000000000000000e+00
	data
	align	8
@L241:
	double	3.60000000000000000000e+02
	data
	align	8
@L242:
	double	1.20000000000000000000e+02
	data
	align	8
@L243:
	double	1.00000000000000000000e+02
	data
	align	8
@L244:
	double	1.00000000000000000000e+02
	data
	data
	data

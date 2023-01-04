	file	"hp2631a.c"
	text
	align	4
	global	_main
_main:
	subu	r31,r31,1216
	st	r30,r31,1208
	st	r1,r31,1212
	addu	r30,r31,1216

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
;                                  
	st	r2,r30,0
	st	r3,r30,4
	addu	r2,r0,0xb
	st	r2,r31,1180
	or.u	r13,r0,hi16(_normal)
	addu	r2,r13,lo16(_normal)
	st	r2,r31,1200
	addu	r2,r0,0x1
	bsr	_ttyname
	st	r2,r31,1192
	bcnd	ne0,r2,@L67
	ld	r4,r30,4
	ld	r2,r4,0
	or	r4,r0,r2
	or.u	r13,r0,hi16(@L68)
	addu	r3,r13,lo16(@L68)
	or.u	r13,r0,hi16(__iob+32)
	addu	r2,r13,lo16(__iob+32)
	bsr	_fprintf
	addu	r2,r0,0x1
	bsr	_exit
@L67:
	addu	r2,r0,0x1
	st	r2,r31,1176
	ld	r2,r31,1176
	ld	r3,r30,0
	cmp	r13,r2,r3
	bb1	ge,r13,@L71
@L72:
	ld	r2,r30,4
	ld	r3,r31,1176
	mak	r3,r3,2
	addu	r2,r2,r3
	ld	r3,r2,0
	st	r3,r31,1196
	ld	r2,r31,1196
	addu	r3,r2,1
	st	r3,r31,1196
	ld.b	r2,r2,0
	cmp	r13,r2,45
	bb1	ne,r13,@L73
	ld	r2,r31,1196
	addu	r3,r2,1
	st	r3,r31,1196
	ld.b	r2,r2,0
	br 	@L75
@L76:
	or.u	r13,r0,hi16(_normal)
	addu	r2,r13,lo16(_normal)
	st	r2,r31,1200
	br 	@L74
@L77:
	or.u	r13,r0,hi16(_compress)
	addu	r2,r13,lo16(_compress)
	st	r2,r31,1200
	br 	@L74
@L78:
	or.u	r13,r0,hi16(_expand)
	addu	r2,r13,lo16(_expand)
	st	r2,r31,1200
	br 	@L74
@L79:
	ld	r2,r31,1196
	bsr	_atoi
	st	r2,r31,1184
	bcnd	le0,r2,@L81
	ld	r2,r31,1184
	br 	@L83
@L84:
	addu	r2,r0,0x7
	st	r2,r31,1180
	or.u	r13,r0,hi16(@L85)
	addu	r2,r13,lo16(@L85)
	bsr	_printf
	br 	@L82
@L86:
	addu	r2,r0,0x8
	st	r2,r31,1180
	br 	@L82
@L87:
	addu	r2,r0,0x9
	st	r2,r31,1180
	br 	@L82
@L88:
	addu	r2,r0,0xa
	st	r2,r31,1180
	br 	@L82
@L89:
	addu	r2,r0,0xb
	st	r2,r31,1180
	br 	@L82
@L90:
	br 	@L82
@L83:
	cmp	r13,r2,300

	bb1	eq,r13,@L84
	cmp	r13,r2,600

	bb1	eq,r13,@L86
	cmp	r13,r2,1200

	bb1	eq,r13,@L87
	cmp	r13,r2,1800

	bb1	eq,r13,@L88
	cmp	r13,r2,2400

	bb1	eq,r13,@L89
	br 	@L90
@L82:
@L81:
@L91:
	br 	@L74
@L75:
	subu	r2,r2,98
	cmp	r13,r2,12
	bb1	hi,r13,@L91
	or.u	r3,r0,hi16(@L92)
	or	r3,r3,lo16(@L92)
	lda	r2,r3[r2]
	jmp	r2
@L92:
	br	@L79
	br	@L77
	br	@L91
	br	@L78
	br	@L91
	br	@L91
	br	@L91
	br	@L91
	br	@L91
	br	@L91
	br	@L91
	br	@L91
	br	@L76
@L74:
@L73:
	ld	r2,r31,1176
	addu	r2,r2,1
	st	r2,r31,1176
	ld	r2,r31,1176
	ld	r3,r30,0
	cmp	r13,r2,r3
	bb1	lt,r13,@L72
@L71:
	addu	r2,r0,0x1
	bsr	_close
	addu	r3,r0,0x2
	ld	r2,r31,1192
	bsr	_open
	cmp	r13,r2,1
	bb1	eq,r13,@L95
	ld	r5,r31,1192
	ld	r4,r30,4
	ld	r2,r4,0
	or	r4,r0,r2
	or.u	r13,r0,hi16(@L96)
	addu	r3,r13,lo16(@L96)
	or.u	r13,r0,hi16(__iob+32)
	addu	r2,r13,lo16(__iob+32)
	bsr	_fprintf
	addu	r2,r0,0x2
	bsr	_exit
@L95:
	or.u	r13,r0,hi16(_save)
	addu	r4,r13,lo16(_save)
	addu	r3,r0,0x5401
	addu	r2,r0,0x1
	bsr	_ioctl
	addu	r3,r0,0x1
	addu	r2,r0,0x1
	bsr	_signal
	cmp	r13,r2,1
	bb1	eq,r13,@L98
	or.u	r13,r0,hi16(_restore)
	addu	r3,r13,lo16(_restore)
	addu	r2,r0,0x1
	bsr	_signal
@L98:
	addu	r3,r0,0x1
	addu	r2,r0,0x2
	bsr	_signal
	cmp	r13,r2,1
	bb1	eq,r13,@L99
	or.u	r13,r0,hi16(_restore)
	addu	r3,r13,lo16(_restore)
	addu	r2,r0,0x2
	bsr	_signal
@L99:
	addu	r3,r0,0x1
	addu	r2,r0,0x3
	bsr	_signal
	cmp	r13,r2,1
	bb1	eq,r13,@L100
	or.u	r13,r0,hi16(_restore)
	addu	r3,r13,lo16(_restore)
	addu	r2,r0,0x3
	bsr	_signal
@L100:
	addu	r3,r0,0x1
	addu	r2,r0,0xf
	bsr	_signal
	cmp	r13,r2,1
	bb1	eq,r13,@L101
	or.u	r13,r0,hi16(_restore)
	addu	r3,r13,lo16(_restore)
	addu	r2,r0,0xf
	bsr	_signal
@L101:
	or.u	r13,r0,hi16(_save)
	addu	r12,r13,lo16(_save)
	addu	r11,r31,32
	addu	r4,r0,0x8
@L9999:
	ld.h	r13,r12[r4]
	st.h	r13,r11[r4]
	bcnd.n	ne0,r4,@L9999
	subu	r4,r4,1
	addu	r2,r0,0x24
	st.h	r2,r31,32
	st.h	r0,r31,34
	ld	r2,r31,1180
	or	r2,r2,416
	extu	r2,r2,16<0>
	st.h	r2,r31,36
	st.h	r0,r31,38
	addu	r2,r0,0x1
	st.b	r2,r31,45
	addu	r4,r31,32
	addu	r3,r0,0x5403
	addu	r2,r0,0x1
	bsr	_ioctl
	bsr	_catch14
	bsr	_ack
	addu	r4,r0,0x1
	or.u	r13,r0,hi16(@L105)
	addu	r3,r13,lo16(@L105)
	addu	r2,r0,0x1
	bsr	_write
	addu	r2,r0,0x5
	bsr	_sleep
	addu	r4,r0,0x1
	or.u	r13,r0,hi16(@L107)
	addu	r3,r13,lo16(@L107)
	addu	r2,r0,0x1
	bsr	_write
	addu	r4,r0,0x1
	or.u	r13,r0,hi16(@L108)
	addu	r3,r13,lo16(@L108)
	addu	r2,r0,0x1
	bsr	_write
	addu	r2,r0,0x5
	bsr	_sleep
	addu	r4,r0,0x1
	or.u	r13,r0,hi16(@L109)
	addu	r3,r13,lo16(@L109)
	addu	r2,r0,0x1
	bsr	_write
	addu	r4,r0,0x1
	or.u	r13,r0,hi16(@L110)
	addu	r3,r13,lo16(@L110)
	addu	r2,r0,0x1
	bsr	_write
	bsr	_ack
	addu	r4,r0,0x5
	ld	r3,r31,1200
	addu	r2,r0,0x1
	bsr	_write
	addu	r4,r0,0x1
	or.u	r13,r0,hi16(@L111)
	addu	r3,r13,lo16(@L111)
	addu	r2,r0,0x1
	bsr	_write
	addu	r4,r0,0x1
	or.u	r13,r0,hi16(@L112)
	addu	r3,r13,lo16(@L112)
	addu	r2,r0,0x1
	bsr	_write
	addu	r2,r0,0x1
	st	r2,r31,1176
	ld	r2,r31,1176
	cmp	r13,r2,15
	bb1	gt,r13,@L114
@L115:
	bsr	_ack
	addu	r4,r0,0x1
	or.u	r13,r0,hi16(@L116)
	addu	r3,r13,lo16(@L116)
	addu	r2,r0,0x1
	bsr	_write
	addu	r4,r0,0x1
	or.u	r13,r0,hi16(@L117)
	addu	r3,r13,lo16(@L117)
	addu	r2,r0,0x1
	bsr	_write
	addu	r4,r0,0x1
	or.u	r13,r0,hi16(@L118)
	addu	r3,r13,lo16(@L118)
	addu	r2,r0,0x1
	bsr	_write
	addu	r4,r0,0x1
	or.u	r13,r0,hi16(@L119)
	addu	r3,r13,lo16(@L119)
	addu	r2,r0,0x1
	bsr	_write
	addu	r4,r0,0x1
	or.u	r13,r0,hi16(@L120)
	addu	r3,r13,lo16(@L120)
	addu	r2,r0,0x1
	bsr	_write
	addu	r4,r0,0x1
	or.u	r13,r0,hi16(@L121)
	addu	r3,r13,lo16(@L121)
	addu	r2,r0,0x1
	bsr	_write
	addu	r4,r0,0x1
	or.u	r13,r0,hi16(@L122)
	addu	r3,r13,lo16(@L122)
	addu	r2,r0,0x1
	bsr	_write
	addu	r4,r0,0x1
	or.u	r13,r0,hi16(@L123)
	addu	r3,r13,lo16(@L123)
	addu	r2,r0,0x1
	bsr	_write
	addu	r4,r0,0x1
	or.u	r13,r0,hi16(@L124)
	addu	r3,r13,lo16(@L124)
	addu	r2,r0,0x1
	bsr	_write
	addu	r4,r0,0x1
	or.u	r13,r0,hi16(@L125)
	addu	r3,r13,lo16(@L125)
	addu	r2,r0,0x1
	bsr	_write
	ld	r2,r31,1176
	addu	r2,r2,1
	st	r2,r31,1176
	ld	r2,r31,1176
	cmp	r13,r2,15
	bb1	le,r13,@L115
@L114:
	addu	r4,r0,0x1
	or.u	r13,r0,hi16(@L126)
	addu	r3,r13,lo16(@L126)
	addu	r2,r0,0x1
	bsr	_write
	st	r0,r31,1172
	ld	r2,r31,1172
	st	r2,r31,1168
	br 	@L129
@L130:
	st	r0,r31,1164
	br 	@L132
@L133:
	st	r0,r31,1160
	br 	@L135
@L136:
	ld	r2,r31,1172
	bcnd	eq0,r2,@L137
	st	r0,r31,1172
	ld	r2,r31,1164
	addu	r3,r2,1
	st	r3,r31,1164
	addu	r3,r31,50
	addu	r2,r2,r3
	ld.b	r3,r2,0
	st.b	r3,r31,1188
	ld.b	r2,r31,1188
	br 	@L139
@L140:
@L141:
@L142:
@L143:
@L144:
@L145:
@L146:
@L147:
	addu	r2,r0,0x1
	st	r2,r31,1168
	br 	@L138
@L148:
@L149:
@L150:
@L151:
	addu	r2,r0,0x1
	st	r2,r31,1168
	br 	@L138
@L152:
	addu	r2,r0,0x1
	st	r2,r31,1168
	addu	r2,r0,0x1
	st	r2,r31,1172
	br 	@L138
@L153:
	ld	r2,r31,1160
	addu	r3,r2,1
	st	r3,r31,1160
	addu	r3,r31,1074
	addu	r2,r2,r3
	addu	r3,r0,0x1b
	st.b	r3,r2,0
	br 	@L138
@L139:
	cmp	r13,r2,69
	bb1	eq,r13,@L141
	bb1	gt,r13,@L154
	cmp	r13,r2,17
	bb1	eq,r13,@L151
	bb1	gt,r13,@L155
	cmp	r13,r2,14
	bb1	eq,r13,@L149
	bb1	gt,r13,@L156
	cmp	r13,r2,5
	bb1	eq,r13,@L148
	br 	@L153
@L156:
	cmp	r13,r2,15
	bb1	eq,r13,@L150
	br 	@L153
@L155:
	cmp	r13,r2,40
	bb1	eq,r13,@L146
	bb1	gt,r13,@L157
	cmp	r13,r2,27
	bb1	eq,r13,@L152
	br 	@L153
@L157:
	cmp	r13,r2,41
	bb1	eq,r13,@L147
	br 	@L153
@L154:
	cmp	r13,r2,111
	bb1	eq,r13,@L143
	bb1	gt,r13,@L158
	cmp	r13,r2,90
	bb1	eq,r13,@L145
	bb1	gt,r13,@L159
	cmp	r13,r2,89
	bb1	eq,r13,@L144
	br 	@L153
@L159:
	cmp	r13,r2,110
	bb1	eq,r13,@L142
	br 	@L153
@L158:
	cmp	r13,r2,122
	bb1	eq,r13,@L140
	br 	@L153
	br 	@L153
@L138:
	br 	@L160
@L137:
	ld	r2,r31,1164
	addu	r3,r2,1
	st	r3,r31,1164
	addu	r3,r31,50
	addu	r2,r2,r3
	ld.b	r3,r2,0
	st.b	r3,r31,1188
	ld.b	r2,r31,1188
	br 	@L162
@L163:
	ld	r2,r31,1160
	addu	r3,r2,1
	st	r3,r31,1160
	addu	r3,r31,1074
	addu	r2,r2,r3
	addu	r3,r0,0xd
	st.b	r3,r2,0
	br 	@L161
@L164:
@L165:
@L166:
@L167:
	addu	r2,r0,0x1
	st	r2,r31,1168
	br 	@L161
@L168:
	addu	r2,r0,0x1
	st	r2,r31,1172
	addu	r2,r0,0x1
	st	r2,r31,1168
	br 	@L161
@L169:
	br 	@L161
@L162:
	cmp	r13,r2,5

	bb1	eq,r13,@L164
	cmp	r13,r2,10

	bb1	eq,r13,@L163
	cmp	r13,r2,14

	bb1	eq,r13,@L165
	cmp	r13,r2,15

	bb1	eq,r13,@L166
	cmp	r13,r2,17

	bb1	eq,r13,@L167
	cmp	r13,r2,27

	bb1	eq,r13,@L168
	br 	@L169
@L161:
@L160:
	ld	r2,r31,1168
	bcnd	eq0,r2,@L170
	st	r0,r31,1168
	br 	@L171
@L170:
	ld.b	r2,r31,1188
	ld	r3,r31,1160
	addu	r4,r3,1
	st	r4,r31,1160
	addu	r4,r31,1074
	addu	r3,r3,r4
	st.b	r2,r3,0
@L171:
@L135:
	ld	r2,r31,1160
	cmp	r13,r2,78
	bb1	ge,r13,@L9998
	ld	r2,r31,1164
	ld	r3,r31,1156
	cmp	r13,r2,r3
	bb1	lt,r13,@L136
@L9998:
@L134:
	bsr	_ack
	ld	r4,r31,1160
	addu	r3,r31,1074
	addu	r2,r0,0x1
	bsr	_write
@L132:
	ld	r2,r31,1164
	ld	r3,r31,1156
	cmp	r13,r2,r3
	bb1	lt,r13,@L133
@L131:
@L129:
	addu	r4,r0,0x400
	addu	r3,r31,50
	addu	r2,r0,0x0
	bsr	_read
	st	r2,r31,1156
	bcnd	gt0,r2,@L130
@L128:
	bsr	_restore
@L62:
	ld	r30,r31,1208
	ld	r1,r31,1212
	jmp.n	r1
	addu	r31,r31,1216
	align	4
	global	_catch14
_catch14:
	subu	r31,r31,40
	st	r1,r31,32
    
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
;                                  
;                                  
	or.u	r13,r0,hi16(_catch14)
	addu	r3,r13,lo16(_catch14)
	addu	r2,r0,0xe
	bsr	_signal
@L172:
	ld	r1,r31,32
	jmp.n	r1
	addu	r31,r31,40
	align	4
	global	_ack
_ack:
	subu	r31,r31,48
	st	r1,r31,40
    
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
;                                  
;                                  
	st.b	r0,r31,32
	br 	@L180
@L181:
	addu	r4,r0,0x1
	or.u	r13,r0,hi16(@L182)
	addu	r3,r13,lo16(@L182)
	addu	r2,r0,0x1
	bsr	_write
	addu	r2,r0,0x5
	bsr	_alarm
	addu	r4,r0,0x1
	addu	r3,r31,32
	addu	r2,r0,0x1
	bsr	_read
	addu	r2,r0,0x0
	bsr	_alarm
@L180:
	ld.b	r2,r31,32
	cmp	r13,r2,6
	bb1	ne,r13,@L181
@L179:
@L175:
	ld	r1,r31,40
	jmp.n	r1
	addu	r31,r31,48
	align	4
	global	_restore
_restore:
	subu	r31,r31,40
	st	r1,r31,32
    
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
;                                  
;                                  
	or.u	r13,r0,hi16(_save)
	addu	r4,r13,lo16(_save)
	addu	r3,r0,0x5403
	addu	r2,r0,0x1
	bsr	_ioctl
	addu	r2,r0,0x0
	bsr	_exit
@L183:
	ld	r1,r31,32
	jmp.n	r1
	addu	r31,r31,40
	data
	data
	ident	"@(#)lp:filter/hp2631a.c	1.4"
	ident	"@(#)head:stdio.h	2.14"
	ident	"@(#)head:signal.h	1.5"
	ident	"@(#)kern-port:sys/signal.h	10.9"
	ident	"@(#)kern-port:sys/ioctl.h	10.2"
	ident	"@(#)head:termio.h	1.3"
	ident	"@(#)kern-port:sys/termio.h	10.3"
	global	_reset
_reset:
	word	0x1b450000
	global	_online
_online:
	word	0x1b6e0000
	global	_normal
_normal:
	word	0x1b266b30
	word	0x53000000
	global	_compress
_compress:
	word	0x1b266b32
	word	0x53000000
	global	_expand
_expand:
	word	0x1b266b31
	word	0x53000000
	global	_clrtabs
_clrtabs:
	word	0x1b330000
	global	_tabset
_tabset:
	word	0x20202020
	word	0x20202020
	word	0x1b310000
	global	_save
	comm	_save,18
	data
	data
	data
	data
@L68:
	byte	0x25,0x73,0x3a,0x20,0x73,0x74,0x61,0x6e
	byte	0x64,0x61,0x72,0x64,0x20,0x6f,0x75,0x74
	byte	0x70,0x75,0x74,0x20,0x6e,0x6f,0x74,0x20
	byte	0x61,0x20,0x70,0x72,0x69,0x6e,0x74,0x65
	byte	0x72,0xa,0x0
@L85:
	byte	0x33,0x30,0x30,0x20,0x62,0x61,0x75,0x64
	byte	0xa,0x0
@L96:
	byte	0x25,0x73,0x3a,0x20,0x63,0x61,0x6e,0x27
	byte	0x74,0x20,0x6f,0x70,0x65,0x6e,0x20,0x25
	byte	0x73,0xa,0x0
@L105:
	byte	0xc,0x0
@L107:
	byte	0x1b,0x0
@L108:
	byte	0x45,0x0
@L109:
	byte	0x1b,0x0
@L110:
	byte	0x6e,0x0
@L111:
	byte	0x1b,0x0
@L112:
	byte	0x33,0x0
@L116:
	byte	0x20,0x0
@L117:
	byte	0x20,0x0
@L118:
	byte	0x20,0x0
@L119:
	byte	0x20,0x0
@L120:
	byte	0x20,0x0
@L121:
	byte	0x20,0x0
@L122:
	byte	0x20,0x0
@L123:
	byte	0x20,0x0
@L124:
	byte	0x1b,0x0
@L125:
	byte	0x31,0x0
@L126:
	byte	0xd,0x0
@L182:
	byte	0x5,0x0

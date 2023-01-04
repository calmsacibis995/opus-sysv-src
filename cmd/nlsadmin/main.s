	.file	"main.c"
	.data
	.ident	"@(#)nlsadmin:main.c	1.12"
	.ident	"@(#)types.h	1.2	5/6/86 Copyright (c) 1986 by National Semiconductor Corp."
	.ident	"@(#)tiuser.h	1.2	5/6/86 Copyright (c) 1986 by National Semiconductor Corp."
	.ident	"@(#)stat.h	1.2	5/6/86 Copyright (c) 1986 by National Semiconductor Corp."
	.ident	"@(#)dir.h	1.2	5/6/86 Copyright (c) 1986 by National Semiconductor Corp."
	.ident	"@(#)s5dir.h	1.1	5/5/86 Copyright (c) 1986 by National Semiconductor Corp."
	.ident	"@(#)head:stdio.h	2.14"
	.ident	"@(#)head:ctype.h	1.6"
	.ident	"@(#)head:signal.h	1.5"
	.ident	"@(#)signal.h	1.2	5/6/86 Copyright (c) 1986 by National Semiconductor Corp."
	.ident	"@(#)head:string.h	1.7"
	.ident	"@(#)head:fcntl.h	1.8"
	.ident	"@(#)fcntl.h	1.2	5/6/86 Copyright (c) 1986 by National Semiconductor Corp."
	.ident	"@(#)nlsadmin:nlsadmin.h	1.6"
	.ident	"@(#)param.h	1.9	9/25/86 Copyright (c) 1986 by National Semiconductor Corp."
	.ident	"@(#)machine.h	1.4	9/25/86 Copyright (c) 1986 by National Semiconductor Corp."
	.ident	"@(#)s5param.h	1.1	5/5/86 Copyright (c) 1986 by National Semiconductor Corp."
	.ident	"@(#)head:errno.h	1.4"
	.ident	"@(#)errno.h	1.2	5/6/86 Copyright (c) 1986 by National Semiconductor Corp."
	.ident	"@(#)head:pwd.h	1.3"
	.ident	"@(#)head:grp.h	1.3"
	.globl	_arg0
	.comm	_arg0,4
	.globl	_Errno
	.comm	_Errno,4
	.globl	_serv_str
	.comm	_serv_str,16
	.globl	_flag_str
	.comm	_flag_str,10
	.globl	_mod_str
	.comm	_mod_str,512
	.globl	_path_str
	.comm	_path_str,512
	.globl	_cmnt_str
	.comm	_cmnt_str,512
	.globl	_homedir
	.comm	_homedir,512
	.globl	_tempfile
	.comm	_tempfile,200
	.align	4
_tempname:
	.data
	.double	.L87
	.globl	_tfp
	.comm	_tfp,4
	.globl	_nlsuid
	.comm	_nlsuid,4
	.globl	_nlsgid
	.comm	_nlsgid,4
	.globl	_uid
	.comm	_uid,4
	.globl	_Netspec
	.comm	_Netspec,4
.globl	_main
	.data
	.align	4
_usagemsg:
	.data
	.double	.L282
	.text
	.align	1
	.align	1
_main:
	enter	[r4,r5,r6,r7],$60
	movqd	$0,r7
	movqd	$0,r5
	movqd	$0,r4
	movqd	$0,-4(fp)
	movqd	$0,-8(fp)
	movqd	$0,-12(fp)
	movqd	$0,-16(fp)
	movqd	$0,-20(fp)
	movqd	$0,-24(fp)
	movqd	$0,-28(fp)
	movqd	$0,-32(fp)
	movqd	$0,-36(fp)
	movqd	$0,-40(fp)
	movqd	$0,-44(fp)
	movd	0(12(fp)),@_arg0
	movqd	$0,@_Errno
	cmpqd	$1,8(fp)
	bne	.L112
	jsr	@_usage
.L112:
	jsr	@_getuid
	movd	r0,@_uid
	addr	@.L115,tos
	jsr	@_getgrnam
	adjspb	$-4
	movd	r0,-52(fp)
	cmpqd	$0,r0
	bne	.L116
	addr	@.L118,tos
	movd	@_arg0,tos
	addr	@.L117,tos
	addr	@__iob+32,tos
	jsr	@_fprintf
	adjspb	$-16
	addr	@24,tos
	jsr	@_exit
	adjspb	$-4
.L116:
	movd	8(-52(fp)),@_nlsgid
	jsr	@_endgrent
	addr	@.L119,tos
	jsr	@_getpwnam
	adjspb	$-4
	movd	r0,-48(fp)
	cmpqd	$0,r0
	beq	.L9999
	movd	28(-48(fp)),tos
	jsr	@_strlen
	adjspb	$-4
	cmpqd	$0,r0
	bne	.L120
.L9999:
	addr	@.L122,tos
	movd	@_arg0,tos
	addr	@.L121,tos
	addr	@__iob+32,tos
	jsr	@_fprintf
	adjspb	$-16
	addr	@26,tos
	jsr	@_exit
	adjspb	$-4
.L120:
	movd	8(-48(fp)),@_nlsuid
	movd	28(-48(fp)),tos
	addr	@_homedir,tos
	jsr	@_strcpy
	adjspb	$-8
	jsr	@_endpwent
	addr	@18,tos
	jsr	@_umask
	adjspb	$-4
	br	.L127
.L128:
	movd	r6,r0
	addr	@113,r1
	cmpd	r0,r1
	beq	.L149
	bgt	.L165
	addr	@108,r1
	cmpd	r0,r1
	beq	.L141
	bgt	.L166
	addr	@100,r1
	cmpd	r0,r1
	beq	.L135
	bgt	.L167
	addr	@97,r1
	cmpd	r0,r1
	beq	.L131
	bgt	.L168
	addr	@63,r1
	cmpd	r0,r1
	beq	.L163
	br	.L164
.L131:
	movqd	$2,r0
	br	.I0
.L135:
	addr	@16,r0
.I0:
	ord	r0,r7
	br	.I1
.L141:
	orb	$128,r7
	sbitb	$1,-16(fp)
	movd	@_optarg,r0
	cmpb	0(r0),$45
	beq	.L142
	movd	@_optarg,-24(fp)
	br	.L143
.L149:
	movqd	$1,-4(fp)
	br	.L129
.L168:
	addr	@99,r1
	cmpd	r0,r1
	bne	.L162
	sbitb	$1,r7
	movd	@_optarg,r0
	cmpb	0(r0),$45
	beq	.L134
	movd	@_optarg,-36(fp)
	br	.L162
.L167:
	addr	@105,r1
	cmpd	r0,r1
	beq	.L139
	bgt	.L169
	addr	@101,r1
	cmpd	r0,r1
	bne	.L162
	addr	@8,r0
	br	.I0
.L139:
	movqd	$1,r0
	br	.I2
.L169:
	addr	@107,r1
	cmpd	r0,r1
	bne	.L162
	addr	@32,r0
.I2:
	ord	r0,r7
	br	.L129
.L166:
	addr	@110,r1
	cmpd	r0,r1
	beq	.L146
	bgt	.L170
	addr	@109,r1
	cmpd	r0,r1
	bne	.L162
	movqd	$1,-20(fp)
	br	.L129
.L146:
	movqd	$1,-8(fp)
	br	.L129
.L170:
	addr	@112,r1
	cmpd	r0,r1
	bne	.L162
	sbitb	$1,r7
	movd	@_optarg,r0
	cmpb	0(r0),$45
	beq	.L148
	movd	@_optarg,-44(fp)
	br	.L162
.L165:
	addr	@118,r1
	cmpd	r0,r1
	beq	.L157
	bgt	.L171
	addr	@115,r1
	cmpd	r0,r1
	beq	.L152
	bgt	.L172
	addr	@114,r1
	cmpd	r0,r1
	bne	.L162
	movqd	$4,r0
	br	.I0
.L152:
	addr	@64,r0
	br	.I2
.L157:
	orw	$256,r7
	br	.L129
.L172:
	addr	@116,r1
	cmpd	r0,r1
	bne	.L162
	orb	$128,r7
	sbitb	$0,-16(fp)
	movd	@_optarg,r0
	cmpb	0(r0),$45
	beq	.L154
	movd	@_optarg,-28(fp)
	br	.L155
.L154:
.L142:
	movd	@_optarg,tos
	jsr	@_strlen
	adjspb	$-4
	cmpqd	$1,r0
	beq	.L144
.L163:
	movqd	$1,-12(fp)
	br	.L129
.L171:
	addr	@121,r1
	cmpd	r0,r1
	beq	.L159
	bgt	.L173
	addr	@120,r1
	cmpd	r0,r1
	bne	.L162
	orw	$512,r7
	br	.L129
.L159:
	sbitb	$1,r7
	movd	@_optarg,r0
	cmpb	0(r0),$45
	beq	.L160
	movd	@_optarg,-40(fp)
	br	.L162
.L173:
	addr	@122,r1
	cmpd	r0,r1
	bne	.L162
	orw	$1024,r7
.I1:
	movd	@_optarg,r0
	cmpb	0(r0),$45
	beq	.L162
	movd	@_optarg,r4
.L162:
.L160:
.L155:
.L148:
.L144:
.L143:
.L134:
.L164:
.L129:
.L127:
	addr	@.L125,tos
	movd	12(fp),tos
	movd	8(fp),tos
	jsr	@_getopt
	adjspb	$-12
	movd	r0,r6
	cmpqd	$-1,r0
	bne	.L128
	cmpqd	$0,-12(fp)
	beq	.L174
	jsr	@_usage
.L174:
	cmpd	@_optind,8(fp)
	bge	.L175
	movd	@_optind,r0
	movd	0(12(fp))[r0:d],r5
	movd	r5,@_Netspec
	addqd	$1,@_optind
.L175:
	cmpd	@_optind,8(fp)
	bge	.L176
	jsr	@_usage
.L176:
	cmpqd	$0,r4
	beq	.L177
	movd	r4,tos
	jsr	@_strlen
	adjspb	$-4
	movd	r0,r6
	cmpqd	$0,r6
	beq	.L9998
	addr	@16,r0
	cmpd	r6,r0
	blt	.L178
.L9998:
	addr	@8,tos
	movd	@_errsvc,tos
	jsr	@_error
	adjspb	$-8
.L178:
	cmpqd	$5,r6
	ble	.L181
	movd	r4,tos
	jsr	@_isnum
	adjspb	$-4
	cmpqd	$0,r0
	beq	.L181
	movd	r4,tos
	jsr	@_atoi
	adjspb	$-4
	movd	r0,r6
	cmpqd	$0,r6
	bne	.L183
	addr	@8,tos
	movd	@_errsvc,tos
	jsr	@_error
	adjspb	$-8
.L183:
	cmpqd	$2,r7
	bne	.L184
	cmpqd	$0,-20(fp)
	beq	.L185
	cmpqd	$2,r6
	bge	.L187
.I3:
	addr	@8,tos
	movd	@_errsvc,tos
	jsr	@_error
	adjspb	$-8
	br	.L187
.L185:
	addr	@101,r0
	cmpd	r6,r0
	blt	.I3
	br	.L188
.L181:
	cmpqd	$2,r7
	bne	.L190
	cmpqd	$0,-20(fp)
	bne	.I3
.L188:
.L187:
.L184:
.L190:
.L177:
	cmpqd	$0,r5
	bne	.L191
	cmpd	r7,$512
	beq	.L192
	jsr	@_usage
	br	.L192
.L191:
	movd	r5,tos
	jsr	@_ok_netspec
	adjspb	$-4
	cmpqd	$0,r0
	bne	.L195
	jsr	@_usage
.L195:
	movd	r5,tos
	jsr	@_strlen
	adjspb	$-4
	movd	r0,-56(fp)
	addr	@.L196,tos
	jsr	@_strlen
	adjspb	$-4
	movd	r0,-60(fp)
	addr	@_homedir,tos
	jsr	@_strlen
	adjspb	$-4
	addd	-56(fp),r0
	addd	-60(fp),r0
	addqd	$3,r0
	movd	r0,tos
	jsr	@_malloc
	adjspb	$-4
	movd	r0,-32(fp)
	cmpqd	$0,r0
	bne	.L197
	movqd	$4,tos
	movd	@_errmalloc,tos
	jsr	@_error
	adjspb	$-8
.L197:
	addr	@.L199,tos
	movd	r5,tos
	addr	@_homedir,tos
	addr	@.L198,tos
	movd	-32(fp),tos
	jsr	@_sprintf
	adjspb	$-20
.L192:
	cmpqd	$0,r5
	beq	.L200
	addr	@_homedir,tos
	addr	@_tempfile,tos
	jsr	@_strcpy
	adjspb	$-8
	addr	@.L201,tos
	addr	@_tempfile,tos
	jsr	@_strcat
	adjspb	$-8
	movd	r5,tos
	addr	@_tempfile,tos
	jsr	@_strcat
	adjspb	$-8
	movd	@_tempname,tos
	addr	@_tempfile,tos
	jsr	@_strcat
	adjspb	$-8
.L200:
	movd	r7,r0
	addr	@64,r1
	cmpd	r0,r1
	beq	.L245
	bgt	.L276
	addr	@4,r1
	cmpd	r0,r1
	beq	.L224
	bgt	.L277
	addr	@1,r1
	cmpd	r0,r1
	beq	.L207
	bgt	.L278
	addr	@0,r1
	cmpd	r0,r1
	beq	.L254
	br	.L275
.L224:
	cmpqd	$0,-20(fp)
	bne	.L9993
	cmpqd	$0,-4(fp)
	bne	.L9993
	cmpqd	$0,-8(fp)
	bne	.L9993
	cmpqd	$0,-16(fp)
	bne	.L9993
	cmpqd	$0,r5
	bne	.L225
.L9993:
	jsr	@_usage
.L225:
	cmpqd	$0,r4
	bne	.L226
	jsr	@_usage
.L226:
	cmpqd	$0,@_uid
	beq	.L227
	movqd	$2,tos
	movd	@_errperm,tos
	jsr	@_error
	adjspb	$-8
.L227:
	movd	r4,tos
	movd	-32(fp),tos
	jsr	@_rem_nls
	adjspb	$-8
	cmpqd	$0,r0
	bne	.L229
	movd	@_Errno,tos
	addr	@.L230,tos
	br	.I4
.L245:
	cmpqd	$0,-20(fp)
	bne	.L9990
	cmpqd	$0,-4(fp)
	bne	.L9990
	cmpqd	$0,-16(fp)
	bne	.L9990
	cmpqd	$0,r5
	bne	.L246
.L9990:
	jsr	@_usage
.L246:
	cmpqd	$0,@_uid
	beq	.L247
	movqd	$2,tos
	movd	@_errperm,tos
	jsr	@_error
	adjspb	$-8
.L247:
	movd	-8(fp),tos
	movd	r5,tos
	jsr	@_start_nls
.I5:
	adjspb	$-8
.L243:
.L236:
.L229:
.L202:
	movd	@_Errno,tos
	jsr	@_exit
	adjspb	$-4
	exit	[r4,r5,r6,r7]
	ret	$0
.L278:
	addr	@2,r1
	cmpd	r0,r1
	beq	.L217
	br	.L275
.L277:
	addr	@16,r1
	cmpd	r0,r1
	beq	.L238
	bgt	.L279
	addr	@8,r1
	cmpd	r0,r1
	bne	.L275
	cmpqd	$0,-20(fp)
	bne	.L9992
	cmpqd	$0,-4(fp)
	bne	.L9992
	cmpqd	$0,-8(fp)
	bne	.L9992
	cmpqd	$0,-16(fp)
	bne	.L9992
	cmpqd	$0,r5
	bne	.L232
.L9992:
	jsr	@_usage
.L232:
	cmpqd	$0,r4
	bne	.L233
	jsr	@_usage
.L233:
	cmpqd	$0,@_uid
	beq	.L234
	movqd	$2,tos
	movd	@_errperm,tos
	jsr	@_error
	adjspb	$-8
.L234:
	movd	r4,tos
	movd	-32(fp),tos
	jsr	@_enable_nls
	adjspb	$-8
	cmpqd	$0,r0
	bne	.L236
	movd	@_Errno,tos
	addr	@.L237,tos
.I4:
	jsr	@_error
	br	.I5
.L238:
	cmpqd	$0,-20(fp)
	bne	.L9991
	cmpqd	$0,-4(fp)
	bne	.L9991
	cmpqd	$0,-8(fp)
	bne	.L9991
	cmpqd	$0,-16(fp)
	bne	.L9991
	cmpqd	$0,r5
	bne	.L239
.L9991:
	jsr	@_usage
.L239:
	cmpqd	$0,r4
	bne	.L240
	jsr	@_usage
.L240:
	cmpqd	$0,@_uid
	beq	.L241
	movqd	$2,tos
	movd	@_errperm,tos
	jsr	@_error
	adjspb	$-8
.L241:
	movd	r4,tos
	movd	-32(fp),tos
	jsr	@_disable_nls
	adjspb	$-8
	cmpqd	$0,r0
	bne	.L243
	movd	@_Errno,tos
	addr	@.L244,tos
	br	.I4
.L279:
	addr	@32,r1
	cmpd	r0,r1
	bne	.L275
	cmpqd	$0,-20(fp)
	bne	.L9989
	cmpqd	$0,-4(fp)
	bne	.L9989
	cmpqd	$0,-8(fp)
	bne	.L9989
	cmpqd	$0,-16(fp)
	bne	.L9989
	cmpqd	$0,r5
	bne	.L250
.L9989:
	jsr	@_usage
.L250:
	cmpqd	$0,@_uid
	beq	.L251
	movqd	$2,tos
	movd	@_errperm,tos
	jsr	@_error
	adjspb	$-8
.L251:
	movd	r5,tos
	jsr	@_kill_nls
	adjspb	$-4
	cmpqd	$0,r0
	bne	.L253
	movd	@_Errno,tos
	jsr	@_exit
	adjspb	$-4
.L253:
	movqd	$0,tos
	jsr	@_exit
	adjspb	$-4
.L254:
	cmpqd	$0,-20(fp)
	bne	.L9988
	cmpqd	$0,-8(fp)
	bne	.L9988
	cmpqd	$0,-16(fp)
	bne	.L9988
	cmpqd	$0,r5
	bne	.L255
.L9988:
	jsr	@_usage
.L255:
	cmpqd	$0,-4(fp)
	beq	.L256
	movd	r5,tos
	jsr	@_isactive
	adjspb	$-4
	cmpqd	$0,r0
	bne	.L9987
	movqd	$1,r0
	br	.L9986
.L9987:
	movqd	$0,r0
.L9986:
	movd	r0,tos
	jsr	@_exit
	adjspb	$-4
	br	.L258
.L256:
	movd	r5,tos
	jsr	@_isactive
	adjspb	$-4
	cmpqd	$0,r0
	beq	.L9985
	addr	@.L260,r0
	br	.L9984
.L9985:
	addr	@.L261,r0
.L9984:
	movd	r0,tos
	movd	r5,tos
	addr	@.L259,tos
	jsr	@_printf
	adjspb	$-12
.L258:
	movqd	$0,tos
	jsr	@_exit
	adjspb	$-4
.L262:
	cmpqd	$0,-20(fp)
	bne	.L9983
	cmpqd	$0,-4(fp)
	bne	.L9983
	cmpqd	$0,-8(fp)
	bne	.L9983
	cmpqd	$0,-16(fp)
	bne	.L9983
	cmpqd	$0,r5
	bne	.L263
.L9983:
	jsr	@_usage
.L263:
	movd	-32(fp),tos
	jsr	@_print_spec
	adjspb	$-4
	movqd	$0,tos
	jsr	@_exit
	adjspb	$-4
.L265:
	cmpqd	$0,-20(fp)
	bne	.L9982
	cmpqd	$0,-4(fp)
	bne	.L9982
	cmpqd	$0,-8(fp)
	bne	.L9982
	cmpqd	$0,r5
	bne	.L266
.L9982:
	jsr	@_usage
.L266:
	cmpqd	$0,-24(fp)
	bne	.L9981
	cmpqd	$0,-28(fp)
	beq	.L267
.L9981:
	cmpqd	$0,@_uid
	beq	.L267
	movqd	$2,tos
	movd	@_errperm,tos
	jsr	@_error
	adjspb	$-8
.L267:
	movd	-16(fp),tos
	movd	-28(fp),tos
	movd	-24(fp),tos
	movd	r5,tos
	jsr	@_chg_addr
	adjspb	$-16
	cmpqd	$0,r0
	bne	.L269
	movd	@_Errno,tos
	addr	@.L270,tos
	jsr	@_error
	adjspb	$-8
.L269:
	movqd	$0,tos
	jsr	@_exit
	adjspb	$-4
.L271:
	cmpqd	$0,-20(fp)
	bne	.L9980
	cmpqd	$0,-8(fp)
	bne	.L9980
	cmpqd	$0,-16(fp)
	bne	.L9980
	cmpqd	$0,r5
	bne	.L272
.L9980:
	jsr	@_usage
.L272:
	cmpqd	$0,r4
	bne	.L273
	jsr	@_usage
.L273:
	movd	-4(fp),tos
	movd	r4,tos
	movd	-32(fp),tos
	jsr	@_print_serv
	adjspb	$-12
	br	.L202
.L276:
	addr	@512,r1
	cmpd	r0,r1
	beq	.L204
	bgt	.L280
	addr	@256,r1
	cmpd	r0,r1
	beq	.L262
	bgt	.L275
	addr	@128,r1
	cmpd	r0,r1
	beq	.L265
	br	.L275
.L204:
	cmpqd	$0,-20(fp)
	bne	.L9997
	cmpqd	$0,-4(fp)
	bne	.L9997
	cmpqd	$0,-8(fp)
	bne	.L9997
	cmpqd	$0,-16(fp)
	bne	.L9997
	cmpqd	$0,r5
	beq	.L205
.L9997:
	jsr	@_usage
.L205:
	addr	@_homedir,tos
	jsr	@_print_all
	adjspb	$-4
	movd	@_Errno,tos
	jsr	@_exit
	adjspb	$-4
.L207:
	cmpqd	$0,-20(fp)
	bne	.L9996
	cmpqd	$0,-4(fp)
	bne	.L9996
	cmpqd	$0,-8(fp)
	bne	.L9996
	cmpqd	$0,-16(fp)
	bne	.L9996
	cmpqd	$0,r5
	bne	.L208
.L9996:
	jsr	@_usage
.L208:
	cmpqd	$0,@_uid
	beq	.L209
	movqd	$2,tos
	movd	@_errperm,tos
	jsr	@_error
	adjspb	$-8
.L209:
	movqd	$0,tos
	movd	-32(fp),tos
	jsr	@_access
	adjspb	$-8
	cmpqd	$0,r0
	ble	.L211
	movd	r5,tos
	movd	-32(fp),tos
	jsr	@_make_db
	adjspb	$-8
	cmpqd	$0,r0
	bne	.L213
	movd	@_Errno,tos
	addr	@.L214,tos
	br	.I6
.L211:
	movqd	$7,tos
	addr	@.L216,tos
.I6:
	jsr	@_error
	adjspb	$-8
.L213:
	movqd	$0,tos
	jsr	@_exit
	adjspb	$-4
.L217:
	cmpqd	$0,-4(fp)
	bne	.L9995
	cmpqd	$0,-8(fp)
	bne	.L9995
	cmpqd	$0,-16(fp)
	bne	.L9995
	cmpqd	$0,r5
	bne	.L218
.L9995:
	jsr	@_usage
.L218:
	cmpqd	$0,r4
	beq	.L9994
	cmpqd	$0,-36(fp)
	beq	.L9994
	cmpqd	$0,-40(fp)
	bne	.L219
.L9994:
	jsr	@_usage
.L219:
	cmpqd	$0,@_uid
	beq	.L220
	movqd	$2,tos
	movd	@_errperm,tos
	jsr	@_error
	adjspb	$-8
.L220:
	movd	-20(fp),tos
	movd	-40(fp),tos
	movd	-36(fp),tos
	movd	-44(fp),tos
	movd	r4,tos
	movd	-32(fp),tos
	jsr	@_add_nls
	adjspb	$-24
	cmpqd	$0,r0
	bne	.L202
	movd	@_Errno,tos
	addr	@.L223,tos
	br	.I4
.L280:
	addr	@1024,r1
	cmpd	r0,r1
	beq	.L271
.L275:
	jsr	@_usage
	br	.L202
.globl	_usage
	.data
	.text
	.align	1
_usage:
	enter	[],$0
	movd	@_arg0,tos
	movd	@_arg0,tos
	movd	@_arg0,tos
	movd	@_usagemsg,tos
	addr	@__iob+32,tos
	jsr	@_fprintf
	adjspb	$-20
	movqd	$3,tos
	jsr	@_exit
	adjspb	$-4
	exit	[]
	ret	$0
.globl	_error
	.data
	.text
	.align	1
_error:
	enter	[],$0
	movd	8(fp),tos
	movd	@_arg0,tos
	addr	@.L289,tos
	addr	@__iob+32,tos
	jsr	@_fprintf
	adjspb	$-16
	movd	12(fp),tos
	jsr	@_exit
	adjspb	$-4
	exit	[]
	ret	$0
.globl	_add_nls
	.data
	.text
	.align	1
_add_nls:
	enter	[r6,r7],$32
	movd	20(fp),r7
	movqd	$0,r6
	br	.L295
.L296:
	addr	@__ctype+1,r0
	movxbd	0(r7),r1
	tbitb	$3,r0[r1:b]
	bfc	.L294
	addqd	$1,r7
.L295:
	cmpqb	$0,0(r7)
	bne	.L296
.L294:
	addr	@.L298,tos
	movd	8(fp),tos
	jsr	@_fopen
	adjspb	$-8
	movd	r0,r6
	cmpqd	$0,r0
	beq	.I7
	movd	12(fp),tos
	jsr	@_strlen
	adjspb	$-4
	addr	@15,r1
	cmpd	r0,r1
	bls	.L300
	addr	@8,tos
	addr	@.L301,tos
	jsr	@_error
	adjspb	$-8
.L300:
	movd	12(fp),tos
	movd	r6,tos
	jsr	@_find_serv
	adjspb	$-8
	cmpqd	$-1,r0
	beq	.L303
	movd	8(fp),tos
	movd	12(fp),tos
	movd	@_arg0,tos
	addr	@.L304,tos
	addr	@__iob+32,tos
	jsr	@_fprintf
	adjspb	$-20
	addr	@9,@_Errno
.I8:
	movqd	$0,r0
.L290:
	exit	[r6,r7]
	ret	$0
.L303:
	movd	r6,tos
	jsr	@_fclose
	adjspb	$-4
	addr	@.L305,tos
	movd	8(fp),tos
	jsr	@_fopen
	adjspb	$-8
	movd	r0,r6
	cmpqd	$0,r0
	bne	.L306
.I7:
	movd	@_Netspec,tos
	movd	@_arg0,tos
	movd	@_erropen,tos
	addr	@__iob+32,tos
	jsr	@_fprintf
	adjspb	$-16
	movqd	$5,@_Errno
	br	.I8
.L306:
	cmpqd	$0,16(fp)
	beq	.L307
	movd	24(fp),tos
	movd	r7,tos
	movd	16(fp),tos
	cmpqd	$0,28(fp)
	beq	.L9979
	addr	@.L309,r0
	br	.L9978
.L9979:
	addr	@.L310,r0
.L9978:
	movd	r0,tos
	movd	12(fp),tos
	addr	@.L308,tos
	movd	r6,tos
	jsr	@_fprintf
	adjspb	$-28
	br	.L311
.L307:
	movd	24(fp),tos
	movd	r7,tos
	cmpqd	$0,28(fp)
	beq	.L9977
	addr	@.L313,r0
	br	.L9976
.L9977:
	addr	@.L314,r0
.L9976:
	movd	r0,tos
	movd	12(fp),tos
	addr	@.L312,tos
	movd	r6,tos
	jsr	@_fprintf
	adjspb	$-24
.L311:
	movd	r6,tos
	jsr	@_fclose
	adjspb	$-4
	cmpqd	$0,r0
	beq	.L315
	movd	@_errno,tos
	movd	8(fp),tos
	movd	@_arg0,tos
	movd	@_errclose,tos
	addr	@__iob+32,tos
	jsr	@_fprintf
	adjspb	$-20
	addr	@10,@_Errno
	br	.I8
.L315:
	addr	@.L316,tos
	movd	r7,tos
	jsr	@_strtok
	adjspb	$-8
	movqd	$0,tos
	movd	r7,tos
	jsr	@_access
	adjspb	$-8
	cmpqd	$0,r0
	bne	.L317
	addr	-32(fp),tos
	movd	r7,tos
	jsr	@_stat
	adjspb	$-8
	movzwd	-28(fp),r0
	addr	@73,r1
	andd	r1,r0
	cmpqd	$0,r0
	bne	.L321
	movd	r7,tos
	movd	@_arg0,tos
	addr	@.L320,tos
	br	.I9
.L317:
	movd	r7,tos
	movd	@_arg0,tos
	addr	@.L322,tos
.I9:
	addr	@__iob+32,tos
	jsr	@_fprintf
	adjspb	$-16
.L321:
	movqd	$1,r0
	br	.L290
.globl	_rem_nls
	.data
	.text
	.align	1
_rem_nls:
	enter	[r7],$4
	addr	@.L326,tos
	movd	8(fp),tos
	jsr	@_fopen
	adjspb	$-8
	movd	r0,r7
	cmpqd	$0,r0
	bne	.L327
	movd	@_Netspec,tos
	movd	@_arg0,tos
	movd	@_erropen,tos
	addr	@__iob+32,tos
	jsr	@_fprintf
	adjspb	$-16
	movqd	$5,@_Errno
	br	.IA
.L327:
	movd	12(fp),tos
	movd	r7,tos
	jsr	@_find_serv
	adjspb	$-8
	movd	r0,-4(fp)
	cmpqd	$-1,r0
	bne	.L328
	movd	8(fp),tos
	movd	12(fp),tos
	movd	@_arg0,tos
	movd	@_errscode,tos
	addr	@__iob+32,tos
	jsr	@_fprintf
	adjspb	$-20
	addr	@11,@_Errno
.IA:
	movqd	$0,r0
.L323:
	exit	[r7]
	ret	$0
.L328:
	jsr	@_open_temp
	cmpqd	$0,r0
	beq	.IA
	cmpqd	$0,-4(fp)
	beq	.L331
	addr	-1(-4(fp)),tos
	movqd	$0,tos
	movd	r7,tos
	jsr	@_copy_file
	adjspb	$-12
.L331:
	movqd	$-1,tos
	addr	1(-4(fp)),tos
	movd	r7,tos
	jsr	@_copy_file
	adjspb	$-12
	movd	8(fp),tos
	jsr	@_close_temp
	adjspb	$-4
	cmpqd	$0,r0
	beq	.IA
	movqd	$1,r0
	br	.L323
.globl	_enable_nls
	.data
	.text
	.align	1
_enable_nls:
	enter	[r4,r5,r6,r7],$12
	addr	@.L338,tos
	movd	8(fp),tos
	jsr	@_fopen
	adjspb	$-8
	movd	r0,r7
	cmpqd	$0,r0
	bne	.L339
	movd	@_Netspec,tos
	movd	@_arg0,tos
	movd	@_erropen,tos
	addr	@__iob+32,tos
	jsr	@_fprintf
	adjspb	$-16
	movqd	$5,@_Errno
	br	.IB
.L339:
	movd	12(fp),tos
	movd	r7,tos
	jsr	@_find_serv
	adjspb	$-8
	movd	r0,-12(fp)
	cmpqd	$-1,r0
	bne	.L340
	movd	8(fp),tos
	movd	12(fp),tos
	movd	@_arg0,tos
	movd	@_errscode,tos
	addr	@__iob+32,tos
	jsr	@_fprintf
	adjspb	$-20
	addr	@11,@_Errno
.IB:
	movqd	$0,r0
.L335:
	exit	[r4,r5,r6,r7]
	ret	$0
.L340:
	jsr	@_open_temp
	cmpqd	$0,r0
	beq	.IB
	cmpqd	$0,-12(fp)
	beq	.L342
	addr	-1(-12(fp)),tos
	movqd	$0,tos
	movd	r7,tos
	jsr	@_copy_file
	br	.IC
.L342:
	movqd	$0,tos
	movqd	$0,tos
	movd	r7,tos
	jsr	@_fseek
.IC:
	adjspb	$-12
	addr	@512,tos
	jsr	@_malloc
	adjspb	$-4
	movd	r0,-4(fp)
	cmpqd	$0,r0
	bne	.L344
	movqd	$4,tos
	movd	@_errmalloc,tos
	jsr	@_error
	adjspb	$-8
.L344:
	addr	@512,tos
	jsr	@_malloc
	adjspb	$-4
	movd	r0,-8(fp)
	cmpqd	$0,r0
	bne	.L345
	movqd	$4,tos
	movd	@_errmalloc,tos
	jsr	@_error
	adjspb	$-8
.L345:
	movd	r7,tos
	addr	@512,tos
	movd	-4(fp),tos
	jsr	@_fgets
	adjspb	$-12
	cmpqd	$0,r0
	bne	.L346
	addr	@_tempfile,tos
	jsr	@_unlink
	adjspb	$-4
	addr	@20,tos
	movd	@_errdbf,tos
	jsr	@_error
	adjspb	$-8
.L346:
	addr	@58,tos
	movd	-4(fp),tos
	jsr	@_strchr
	adjspb	$-8
	movd	r0,r4
	movd	-8(fp),r6
	movd	-4(fp),r5
	br	.L350
.L351:
	movb	0(r5),0(r6)
	addqd	$1,r5
	addqd	$1,r6
.L350:
	cmpd	r5,r4
	blt	.L351
	movb	0(r5),0(r6)
	addqd	$1,r5
	addqd	$1,r6
	addr	@58,tos
	movd	r5,tos
	jsr	@_strchr
	adjspb	$-8
	movd	r0,r4
	br	.L354
.L355:
	cmpb	0(r5),$120
	beq	.L9975
	cmpb	0(r5),$88
	bne	.L356
.L9975:
	addqd	$1,r5
	br	.L352
.L356:
	movb	0(r5),0(r6)
	addqd	$1,r5
	addqd	$1,r6
.L352:
.L354:
	cmpd	r5,r4
	blt	.L355
	br	.L359
.L360:
	movb	0(r5),0(r6)
	addqd	$1,r5
	addqd	$1,r6
.L359:
	cmpqb	$0,0(r5)
	bne	.L360
	movd	-8(fp),tos
	addr	@.L361,tos
	movd	@_tfp,tos
	jsr	@_fprintf
	adjspb	$-12
	movd	-4(fp),tos
	jsr	@_free
	adjspb	$-4
	movd	-8(fp),tos
	jsr	@_free
	adjspb	$-4
	movqd	$-1,tos
	addr	1(-12(fp)),tos
	movd	r7,tos
	jsr	@_copy_file
	adjspb	$-12
	movd	r7,tos
	jsr	@_fclose
	adjspb	$-4
	movd	8(fp),tos
	jsr	@_close_temp
	adjspb	$-4
	cmpqd	$0,r0
	beq	.IB
	movqd	$1,r0
	br	.L335
.globl	_disable_nls
	.data
	.text
	.align	1
_disable_nls:
	enter	[r4,r5,r6,r7],$12
	addr	@.L367,tos
	movd	8(fp),tos
	jsr	@_fopen
	adjspb	$-8
	movd	r0,r7
	cmpqd	$0,r0
	bne	.L368
	movd	@_Netspec,tos
	movd	@_arg0,tos
	movd	@_erropen,tos
	addr	@__iob+32,tos
	jsr	@_fprintf
	adjspb	$-16
	movqd	$5,@_Errno
	br	.ID
.L368:
	movd	12(fp),tos
	movd	r7,tos
	jsr	@_find_serv
	adjspb	$-8
	movd	r0,-12(fp)
	cmpqd	$-1,r0
	bne	.L369
	movd	8(fp),tos
	movd	12(fp),tos
	movd	@_arg0,tos
	movd	@_errscode,tos
	addr	@__iob+32,tos
	jsr	@_fprintf
	adjspb	$-20
	addr	@11,@_Errno
.ID:
	movqd	$0,r0
.L364:
	exit	[r4,r5,r6,r7]
	ret	$0
.L369:
	jsr	@_open_temp
	cmpqd	$0,r0
	beq	.ID
	cmpqd	$0,-12(fp)
	beq	.L371
	addr	-1(-12(fp)),tos
	movqd	$0,tos
	movd	r7,tos
	jsr	@_copy_file
	br	.IE
.L371:
	movqd	$0,tos
	movqd	$0,tos
	movd	r7,tos
	jsr	@_fseek
.IE:
	adjspb	$-12
	addr	@512,tos
	jsr	@_malloc
	adjspb	$-4
	movd	r0,-4(fp)
	cmpqd	$0,r0
	bne	.L373
	movqd	$4,tos
	movd	@_errmalloc,tos
	jsr	@_error
	adjspb	$-8
.L373:
	addr	@512,tos
	jsr	@_malloc
	adjspb	$-4
	movd	r0,-8(fp)
	cmpqd	$0,r0
	bne	.L374
	movqd	$4,tos
	movd	@_errmalloc,tos
	jsr	@_error
	adjspb	$-8
.L374:
	movd	r7,tos
	addr	@512,tos
	movd	-4(fp),tos
	jsr	@_fgets
	adjspb	$-12
	cmpqd	$0,r0
	bne	.L375
	addr	@_tempfile,tos
	jsr	@_unlink
	adjspb	$-4
	addr	@20,tos
	movd	@_errdbf,tos
	jsr	@_error
	adjspb	$-8
.L375:
	addr	@58,tos
	movd	-4(fp),tos
	jsr	@_strchr
	adjspb	$-8
	movd	r0,r4
	movd	-8(fp),r6
	movd	-4(fp),r5
	br	.L378
.L379:
	movb	0(r5),0(r6)
	addqd	$1,r5
	addqd	$1,r6
.L378:
	cmpd	r5,r4
	blt	.L379
	movb	0(r5),0(r6)
	addqd	$1,r5
	addqd	$1,r6
	addr	@58,tos
	movd	r5,tos
	jsr	@_strchr
	adjspb	$-8
	movd	r0,r4
	br	.L382
.L383:
	cmpb	0(r5),$120
	beq	.L9974
	cmpb	0(r5),$88
	bne	.L384
.L9974:
	addqd	$1,r5
	br	.L380
.L384:
	movb	0(r5),0(r6)
	addqd	$1,r5
	addqd	$1,r6
.L380:
.L382:
	cmpd	r5,r4
	blt	.L383
	movb	$120,0(r6)
	br	.IF
.L388:
	movb	0(r5),0(r6)
	addqd	$1,r5
.IF:
	addqd	$1,r6
	cmpqb	$0,0(r5)
	bne	.L388
	movd	-8(fp),tos
	addr	@.L389,tos
	movd	@_tfp,tos
	jsr	@_fprintf
	adjspb	$-12
	movd	-4(fp),tos
	jsr	@_free
	adjspb	$-4
	movd	-8(fp),tos
	jsr	@_free
	adjspb	$-4
	movqd	$-1,tos
	addr	1(-12(fp)),tos
	movd	r7,tos
	jsr	@_copy_file
	adjspb	$-12
	movd	r7,tos
	jsr	@_fclose
	adjspb	$-4
	movd	8(fp),tos
	jsr	@_close_temp
	adjspb	$-4
	cmpqd	$0,r0
	beq	.ID
	movqd	$1,r0
	br	.L364
.globl	_isdir
	.data
	.text
	.align	1
_isdir:
	enter	[],$32
	addr	-32(fp),tos
	movd	8(fp),tos
	jsr	@_stat
	adjspb	$-8
	cmpqd	$0,r0
	beq	.L395
	movd	@_errno,tos
	movd	8(fp),tos
	movd	@_arg0,tos
	addr	@.L396,tos
	addr	@__iob+32,tos
	jsr	@_fprintf
	adjspb	$-20
	addr	@12,@_Errno
	br	.L397
.L395:
	movzwd	-28(fp),r0
	andd	$61440,r0
	cmpd	r0,$16384
	bne	.L397
	movqd	$1,r0
.L392:
	exit	[]
	ret	$0
.L397:
	movqd	$0,r0
	br	.L392
.globl	_isnum
	.data
	.text
	.align	1
_isnum:
	enter	[r7],$0
	movd	8(fp),r7
	cmpqb	$0,0(r7)
	bne	.L401
.I10:
	movqd	$0,r0
.L398:
	exit	[r7]
	ret	$0
.L401:
.L405:
	cmpqb	$0,0(r7)
	bne	.L404
	movqd	$1,r0
	br	.L398
.L404:
	movd	r7,r0
	addqd	$1,r7
	movxbd	0(r0),r0
	tbitb	$2,__ctype+1[r0:b]
	bfs	.L405
	br	.I10
.globl	_isactive
	.data
	.text
	.align	1
_isactive:
	enter	[],$236
	movd	8(fp),tos
	addr	@_homedir,tos
	addr	@.L409,tos
	addr	-200(fp),tos
	jsr	@_sprintf
	adjspb	$-16
	movqd	$0,tos
	addr	-200(fp),tos
	jsr	@_access
	adjspb	$-8
	cmpqd	$0,r0
	ble	.L410
	movd	8(fp),tos
	movd	@_arg0,tos
	addr	@.L411,tos
	addr	@__iob+32,tos
	jsr	@_fprintf
	adjspb	$-16
	addr	@29,tos
	jsr	@_exit
	adjspb	$-4
.L410:
	addr	@.L412,tos
	addr	-200(fp),tos
	jsr	@_strcat
	adjspb	$-8
	addr	@.L413,tos
	addr	-200(fp),tos
	jsr	@_strcat
	adjspb	$-8
	addr	-236(fp),tos
	addr	-200(fp),tos
	jsr	@_stat
	adjspb	$-8
	cmpqd	$0,r0
	bne	.I11
	movzwd	-232(fp),r0
	andd	$511,r0
	cmpd	r0,$438
	beq	.L415
	cmpqd	$0,@_uid
	beq	.L9973
	cmpd	@_uid,@_nlsuid
	bne	.L416
.L9973:
	addr	@438,tos
	addr	-200(fp),tos
	jsr	@_chmod
	adjspb	$-8
	br	.L418
.L416:
	addr	-200(fp),tos
	movd	@_arg0,tos
	addr	@.L419,tos
	addr	@__iob+32,tos
	jsr	@_fprintf
	adjspb	$-16
	movqd	$2,tos
	jsr	@_exit
	adjspb	$-4
.L418:
.L415:
	movqd	$2,tos
	addr	-200(fp),tos
	jsr	@_open
	adjspb	$-8
	movd	r0,-204(fp)
	cmpqd	$0,r0
	bgt	.I11
	movqd	$0,tos
	movqd	$2,tos
	movd	-204(fp),tos
	jsr	@_lockf
	adjspb	$-12
	cmpqd	$-1,r0
	bne	.L422
	movd	-204(fp),tos
	jsr	@_close
	adjspb	$-4
	movqd	$1,r0
.L406:
	exit	[]
	ret	$0
.L422:
	movd	-204(fp),tos
	jsr	@_close
	adjspb	$-4
.I11:
	movqd	$0,r0
	br	.L406
.globl	_make_db
	.data
	.text
	.align	1
_make_db:
	enter	[r4,r5,r6,r7],$604
	movqd	$0,r7
	movqd	$0,r6
	movd	12(fp),tos
	addr	@_homedir,tos
	addr	@.L427,tos
	addr	-204(fp),tos
	jsr	@_sprintf
	adjspb	$-16
	addr	@_homedir,tos
	addr	-604(fp),tos
	jsr	@_strcpy
	adjspb	$-8
	addr	@_homedir,tos
	jsr	@_strlen
	adjspb	$-4
	addr	-204(fp),r1
	addd	r1,r0
	addqd	$1,r0
	movd	r0,r4
.L430:
	movd	r4,-4(fp)
	addr	@47,tos
	movd	r4,tos
	jsr	@_strchr
	adjspb	$-8
	movd	r0,r4
	cmpd	r4,-4(fp)
	beq	.L9972
	cmpqd	$0,r4
	bne	.L431
.L9972:
	addr	-204(fp),tos
	jsr	@_strlen
	adjspb	$-4
	movd	r0,r5
	br	.L432
.L431:
	addr	-204(fp),r0
	movd	r4,r1
	subd	r0,r1
	movd	r1,r5
.L432:
	movd	r5,tos
	addr	-204(fp),tos
	addr	-404(fp),tos
	jsr	@_strncpy
	adjspb	$-12
	movqb	$0,-404(fp)[r5:b]
	movqd	$0,tos
	addr	-404(fp),tos
	jsr	@_access
	adjspb	$-8
	cmpqd	$0,r0
	ble	.L433
	cmpqd	$2,@_errno
	bne	.L433
	addr	-404(fp),tos
	addr	-604(fp),tos
	jsr	@_makedir
	adjspb	$-8
	cmpqd	$0,r0
	beq	.I12
.L433:
	addr	-404(fp),tos
	addr	-604(fp),tos
	jsr	@_strcpy
	adjspb	$-8
	cmpd	r4,-4(fp)
	beq	.L9971
	movd	r4,r0
	addqd	$1,r4
	cmpqd	$0,r0
	bne	.L430
.L9971:
	addr	@.L436,tos
	addr	-204(fp),tos
	jsr	@_strcat
	adjspb	$-8
	addr	@.L437,tos
	addr	-204(fp),tos
	jsr	@_strcat
	adjspb	$-8
	addr	@.L438,tos
	movd	8(fp),tos
	jsr	@_fopen
	adjspb	$-8
	movd	r0,r7
	cmpqd	$0,r0
	bne	.L439
	movd	@_errno,tos
	movd	8(fp),tos
	movd	@_arg0,tos
	addr	@.L440,tos
	br	.I13
.L439:
	addr	@.L441,tos
	addr	-204(fp),tos
	jsr	@_fopen
	adjspb	$-8
	movd	r0,r6
	cmpqd	$0,r0
	bne	.L442
	movd	@_errno,tos
	addr	-204(fp),tos
	movd	@_arg0,tos
	addr	@.L443,tos
.I13:
	addr	@__iob+32,tos
	jsr	@_fprintf
	adjspb	$-20
	movqd	$6,@_Errno
	br	.I12
.L442:
	movd	r7,tos
	jsr	@_init_db
	adjspb	$-4
	movd	r7,tos
	jsr	@_fclose
	adjspb	$-4
	cmpqd	$0,r0
	beq	.L445
	movd	@_errno,tos
	movd	8(fp),tos
	br	.I14
.L445:
	movd	@_nlsgid,tos
	movd	@_nlsuid,tos
	movd	8(fp),tos
	jsr	@_chown
	adjspb	$-12
	cmpqd	$0,r0
	ble	.L447
	movd	@_errno,tos
	movd	8(fp),tos
	br	.I15
.L447:
	addr	@.L448,tos
	movd	r6,tos
	jsr	@_fprintf
	adjspb	$-8
	movd	r6,tos
	jsr	@_fclose
	adjspb	$-4
	cmpqd	$0,r0
	bne	.I16
	movd	@_nlsgid,tos
	movd	@_nlsuid,tos
	addr	-204(fp),tos
	jsr	@_chown
	adjspb	$-12
	cmpqd	$0,r0
	bgt	.I17
	movqd	$0,tos
	jsr	@_umask
	adjspb	$-4
	addr	@.L452,tos
	movd	12(fp),tos
	addr	@_homedir,tos
	addr	@.L451,tos
	addr	-204(fp),tos
	jsr	@_sprintf
	adjspb	$-20
	addr	@.L453,tos
	addr	-204(fp),tos
	jsr	@_fopen
	adjspb	$-8
	movd	r0,r7
	cmpqd	$0,r0
	bne	.L454
	movd	@_errno,tos
	addr	-204(fp),tos
	movd	@_arg0,tos
	addr	@.L455,tos
	br	.I13
.L454:
	movd	r7,tos
	jsr	@_fclose
	adjspb	$-4
	cmpqd	$0,r0
	beq	.L456
.I16:
	movd	@_errno,tos
	addr	-204(fp),tos
.I14:
	movd	@_arg0,tos
	movd	@_errclose,tos
	addr	@__iob+32,tos
	jsr	@_fprintf
	adjspb	$-20
	addr	@10,@_Errno
	br	.I12
.L456:
	movd	@_nlsgid,tos
	movd	@_nlsuid,tos
	addr	-204(fp),tos
	jsr	@_chown
	adjspb	$-12
	cmpqd	$0,r0
	ble	.L457
.I17:
	movd	@_errno,tos
	addr	-204(fp),tos
.I15:
	movd	@_arg0,tos
	movd	@_errchown,tos
	addr	@__iob+32,tos
	jsr	@_fprintf
	adjspb	$-20
	addr	@25,@_Errno
.I12:
	movqd	$0,r0
.L424:
	exit	[r4,r5,r6,r7]
	ret	$0
.L457:
	addr	@18,tos
	jsr	@_umask
	adjspb	$-4
	movqd	$1,r0
	br	.L424
.globl	_chg_addr
	.data
	.text
	.align	1
_chg_addr:
	enter	[r6,r7],$332
	movd	20(fp),r7
	movqd	$0,r6
	movqb	$0,-264(fp)
	movqb	$0,-328(fp)
	addr	@.L462,tos
	movd	8(fp),tos
	addr	@_homedir,tos
	addr	@.L461,tos
	addr	-200(fp),tos
	jsr	@_sprintf
	adjspb	$-20
	addr	@.L463,tos
	addr	-200(fp),tos
	jsr	@_fopen
	adjspb	$-8
	movd	r0,r6
	cmpqd	$0,r0
	beq	.I18
	movd	r6,tos
	addr	@64,tos
	addr	-264(fp),tos
	jsr	@_fgets
	adjspb	$-12
	movd	r6,tos
	addr	@64,tos
	addr	-328(fp),tos
	jsr	@_fgets
	adjspb	$-12
	tbitb	$5,12(r6)
	bfc	.L465
	movd	@_arg0,tos
	addr	@.L466,tos
	addr	@__iob+32,tos
	jsr	@_fprintf
	adjspb	$-12
	movd	r6,tos
	jsr	@_fclose
	adjspb	$-4
	addr	@22,@_Errno
.I1A:
	movqd	$0,r0
.L458:
	exit	[r6,r7]
	ret	$0
.L465:
	movd	r6,tos
	jsr	@_fclose
	adjspb	$-4
	cmpb	-264(fp),$0
	bne	.L467
	movb	$10,-264(fp)
.L467:
	cmpb	-328(fp),$0
	bne	.L468
	movb	$10,-264(fp)
.L468:
	addr	@10,tos
	addr	-264(fp),tos
	jsr	@_strrchr
	adjspb	$-8
	movd	r0,-332(fp)
	cmpqd	$0,r0
	beq	.L469
	movqb	$0,0(-332(fp))
.L469:
	addr	@10,tos
	addr	-328(fp),tos
	jsr	@_strrchr
	adjspb	$-8
	movd	r0,-332(fp)
	cmpqd	$0,r0
	beq	.L470
	movqb	$0,0(-332(fp))
.L470:
	movqd	$0,r6
	cmpqd	$0,12(fp)
	bne	.L471
	tbitb	$1,r7
	bfc	.L474
	addr	-264(fp),tos
	addr	@.L473,tos
	jsr	@_printf
	adjspb	$-8
	br	.L474
.L471:
	addr	@.L475,tos
	addr	-200(fp),tos
	jsr	@_fopen
	adjspb	$-8
	movd	r0,r6
	cmpqd	$0,r0
	beq	.I18
	movd	12(fp),tos
	addr	@.L477,tos
	movd	r6,tos
	jsr	@_fprintf
	adjspb	$-12
.L474:
	cmpqd	$0,16(fp)
	bne	.L478
	tbitb	$0,r7
	bfc	.L479
	addr	-328(fp),tos
	addr	@.L480,tos
	jsr	@_printf
	adjspb	$-8
.L479:
	cmpqd	$0,r6
	beq	.L481
	addr	-328(fp),tos
	addr	@.L482,tos
	br	.I19
.L478:
	cmpqd	$0,r6
	bne	.L484
	addr	@.L485,tos
	addr	-200(fp),tos
	jsr	@_fopen
	adjspb	$-8
	movd	r0,r6
	cmpqd	$0,r0
	bne	.L486
.I18:
	movd	@_Netspec,tos
	movd	@_arg0,tos
	movd	@_erropen,tos
	addr	@__iob+32,tos
	jsr	@_fprintf
	adjspb	$-16
	movqd	$5,@_Errno
	br	.I1A
.L486:
	addr	-264(fp),tos
	addr	@.L487,tos
	movd	r6,tos
	jsr	@_fprintf
	adjspb	$-12
.L484:
	movd	16(fp),tos
	addr	@.L488,tos
.I19:
	movd	r6,tos
	jsr	@_fprintf
	adjspb	$-12
.L481:
	cmpqd	$0,r6
	beq	.L489
	movd	r6,tos
	jsr	@_fclose
	adjspb	$-4
	cmpqd	$0,r0
	beq	.L490
	movd	@_errno,tos
	addr	-200(fp),tos
	movd	@_arg0,tos
	movd	@_errclose,tos
	addr	@__iob+32,tos
	jsr	@_fprintf
	adjspb	$-20
	addr	@10,@_Errno
	br	.I1A
.L490:
	movd	@_nlsgid,tos
	movd	@_nlsuid,tos
	addr	-200(fp),tos
	jsr	@_chown
	adjspb	$-12
	cmpqd	$0,r0
	ble	.L491
	movd	@_errno,tos
	addr	-200(fp),tos
	movd	@_arg0,tos
	movd	@_errchown,tos
	addr	@__iob+32,tos
	jsr	@_fprintf
	adjspb	$-20
	addr	@25,@_Errno
	br	.I1A
.L491:
.L489:
	movqd	$1,r0
	br	.L458
.globl	_init_db
	.data
	.text
	.align	1
_init_db:
	enter	[r7],$0
	movqd	$0,r7
	br	.L497
.L498:
	movd	_init[r7:d],tos
	addr	@.L499,tos
	movd	8(fp),tos
	jsr	@_fprintf
	adjspb	$-12
	addqd	$1,r7
.L497:
	cmpqd	$0,_init[r7:d]
	bne	.L498
	exit	[r7]
	ret	$0
.globl	_copy_file
	.data
	.text
	.align	1
_copy_file:
	enter	[r4,r5,r6,r7],$512
	movd	8(fp),r7
	movd	12(fp),r6
	movd	16(fp),r5
	movqd	$0,tos
	movqd	$0,tos
	movd	r7,tos
	jsr	@_fseek
	adjspb	$-12
	cmpqd	$0,r6
	beq	.L503
	movqd	$0,r4
	br	.L506
.L507:
	movd	r7,tos
	addr	@512,tos
	addr	-512(fp),tos
	jsr	@_fgets
	adjspb	$-12
	cmpqd	$0,r0
	bne	.L508
	addr	@_tempfile,tos
	jsr	@_unlink
	adjspb	$-4
	addr	@20,tos
	movd	@_errdbf,tos
	jsr	@_error
	adjspb	$-8
.L508:
	addqd	$1,r4
.L506:
	cmpd	r4,r6
	blt	.L507
.L503:
	cmpqd	$-1,r5
	beq	.L509
	movd	r6,r4
	br	.L512
.L513:
	movd	r7,tos
	addr	@512,tos
	addr	-512(fp),tos
	jsr	@_fgets
	adjspb	$-12
	cmpqd	$0,r0
	bne	.L514
	addr	@_tempfile,tos
	jsr	@_unlink
	adjspb	$-4
	addr	@20,tos
	movd	@_errdbf,tos
	jsr	@_error
	adjspb	$-8
.L514:
	movd	@_tfp,tos
	addr	-512(fp),tos
	jsr	@_fputs
	adjspb	$-8
	cmpqd	$-1,r0
	bne	.L515
	addr	@_tempfile,tos
	jsr	@_unlink
	adjspb	$-4
	addr	@21,tos
	addr	@.L516,tos
	jsr	@_error
	adjspb	$-8
.L515:
	addqd	$1,r4
.L512:
	cmpd	r4,r5
	ble	.L513
.L519:
	exit	[r4,r5,r6,r7]
	ret	$0
.L509:
.L520:
	movd	r7,tos
	addr	@512,tos
	addr	-512(fp),tos
	jsr	@_fgets
	adjspb	$-12
	cmpqd	$0,r0
	bne	.L521
	movxbd	12(r7),r0
	tbitb	$4,r0
	bfs	.L519
	addr	@_tempfile,tos
	jsr	@_unlink
	adjspb	$-4
	addr	@20,tos
	movd	@_errdbf,tos
	jsr	@_error
	adjspb	$-8
.L521:
	movd	@_tfp,tos
	addr	-512(fp),tos
	jsr	@_fputs
	adjspb	$-8
	cmpqd	$-1,r0
	bne	.L520
	addr	@_tempfile,tos
	jsr	@_unlink
	adjspb	$-4
	addr	@21,tos
	addr	@.L524,tos
	jsr	@_error
	adjspb	$-8
	br	.L520
.globl	_find_pid
	.data
	.text
	.align	1
_find_pid:
	enter	[],$216
	addr	@.L530,tos
	movd	8(fp),tos
	addr	@_homedir,tos
	addr	@.L529,tos
	addr	-200(fp),tos
	jsr	@_sprintf
	adjspb	$-20
	addr	@.L531,tos
	addr	-200(fp),tos
	jsr	@_fopen
	adjspb	$-8
	movd	r0,-216(fp)
	cmpqd	$0,r0
	beq	.I1B
	addr	-210(fp),tos
	addr	@.L533,tos
	movd	-216(fp),tos
	jsr	@_fscanf
	adjspb	$-12
	cmpqd	$1,r0
	beq	.L534
	movd	-216(fp),tos
	jsr	@_fclose
	adjspb	$-4
.I1B:
	movqd	$0,r0
.L526:
	exit	[]
	ret	$0
.L534:
	movd	-216(fp),tos
	jsr	@_fclose
	adjspb	$-4
	addr	@10,tos
	movqd	$0,tos
	addr	-210(fp),tos
	jsr	@_strtol
	adjspb	$-12
	br	.L526
.globl	_start_nls
	.data
	.text
	.align	1
_start_nls:
	enter	[r6,r7],$316
	movqd	$0,-308(fp)
	movqd	$0,-312(fp)
	movd	8(fp),tos
	jsr	@_isactive
	adjspb	$-4
	cmpqd	$0,r0
	beq	.L541
	addr	@16,tos
	addr	@.L542,tos
	jsr	@_error
	adjspb	$-8
.L541:
	cmpqd	$0,12(fp)
	beq	.L543
	addr	@.L545,tos
	movd	8(fp),tos
	addr	@_homedir,tos
	addr	@.L544,tos
	addr	-300(fp),tos
	jsr	@_sprintf
	adjspb	$-20
	br	.L546
.L543:
	addr	@.L548,tos
	addr	@_homedir,tos
	addr	@.L547,tos
	addr	-300(fp),tos
	jsr	@_sprintf
	adjspb	$-16
.L546:
	addr	@.L550,tos
	movd	8(fp),tos
	addr	@_homedir,tos
	addr	@.L549,tos
	addr	-200(fp),tos
	jsr	@_sprintf
	adjspb	$-20
	addr	@.L551,tos
	addr	-200(fp),tos
	jsr	@_fopen
	adjspb	$-8
	movd	r0,-304(fp)
	cmpqd	$0,r0
	bne	.L552
	movd	@_Netspec,tos
	movd	@_arg0,tos
	movd	@_erropen,tos
	addr	@__iob+32,tos
	jsr	@_fprintf
	adjspb	$-16
	movqd	$5,tos
	jsr	@_exit
	adjspb	$-4
.L552:
	addr	@64,tos
	jsr	@_malloc
	adjspb	$-4
	movd	r0,r7
	cmpqd	$0,r0
	bne	.L553
	movqd	$4,tos
	movd	@_errmalloc,tos
	jsr	@_error
	adjspb	$-8
.L553:
	addr	@64,tos
	jsr	@_malloc
	adjspb	$-4
	movd	r0,r6
	cmpqd	$0,r0
	bne	.L554
	movqd	$4,tos
	movd	@_errmalloc,tos
	jsr	@_error
	adjspb	$-8
.L554:
	movqb	$0,0(r7)
	movqb	$0,0(r6)
	movd	-304(fp),tos
	addr	@64,tos
	movd	r7,tos
	jsr	@_fgets
	adjspb	$-12
	movd	-304(fp),tos
	addr	@64,tos
	movd	r6,tos
	jsr	@_fgets
	adjspb	$-12
	cmpb	0(r7),$10
	beq	.L9970
	cmpb	0(r6),$10
	bne	.L555
.L9970:
	movd	8(fp),tos
	movd	@_arg0,tos
	addr	@.L556,tos
	addr	@__iob+32,tos
	jsr	@_fprintf
	adjspb	$-16
	movd	-304(fp),tos
	jsr	@_fclose
	adjspb	$-4
	addr	@22,tos
	jsr	@_exit
	adjspb	$-4
.L555:
	cmpb	0(r7),$0
	beq	.L9969
	cmpb	0(r6),$0
	bne	.L557
.L9969:
	movd	@_arg0,tos
	addr	@.L558,tos
	addr	@__iob+32,tos
	jsr	@_fprintf
	adjspb	$-12
	movd	-304(fp),tos
	jsr	@_fclose
	adjspb	$-4
	addr	@22,tos
	jsr	@_exit
	adjspb	$-4
.L557:
	movd	r7,tos
	jsr	@_strlen
	adjspb	$-4
	addd	r7,r0
	addqd	$-1,r0
	movd	r0,-316(fp)
	movqb	$0,0(-316(fp))
	movd	r6,tos
	jsr	@_strlen
	adjspb	$-4
	addd	r6,r0
	addqd	$-1,r0
	movd	r0,-316(fp)
	movqb	$0,0(-316(fp))
	movd	-308(fp),tos
	movd	r7,tos
	jsr	@_stoa
	adjspb	$-8
	movd	r0,-308(fp)
	cmpqd	$0,r0
	bne	.L559
	movqd	$4,tos
	movd	@_errmalloc,tos
	jsr	@_error
	adjspb	$-8
.L559:
	movd	-312(fp),tos
	movd	r6,tos
	jsr	@_stoa
	adjspb	$-8
	movd	r0,-312(fp)
	cmpqd	$0,r0
	bne	.L560
	movqd	$4,tos
	movd	@_errmalloc,tos
	jsr	@_error
	adjspb	$-8
.L560:
	movd	r7,tos
	jsr	@_free
	adjspb	$-4
	movd	r6,tos
	jsr	@_free
	adjspb	$-4
	addr	@129,tos
	jsr	@_malloc
	adjspb	$-4
	movd	r0,r7
	cmpqd	$0,r0
	bne	.L561
	movqd	$4,tos
	movd	@_errmalloc,tos
	jsr	@_error
	adjspb	$-8
.L561:
	addr	@129,tos
	jsr	@_malloc
	adjspb	$-4
	movd	r0,r6
	cmpqd	$0,r0
	bne	.L562
	movqd	$4,tos
	movd	@_errmalloc,tos
	jsr	@_error
	adjspb	$-8
.L562:
	movd	4(-308(fp)),tos
	movd	8(-308(fp)),tos
	movd	r7,tos
	jsr	@_nlsaddr2c
	adjspb	$-12
	movd	4(-312(fp)),tos
	movd	8(-312(fp)),tos
	movd	r6,tos
	jsr	@_nlsaddr2c
	adjspb	$-12
	movd	-304(fp),tos
	jsr	@_fclose
	adjspb	$-4
	movqd	$0,tos
	movd	r6,tos
	addr	@.L567,tos
	movd	r7,tos
	addr	@.L566,tos
	movd	8(fp),tos
	addr	@.L565,tos
	addr	@.L564,tos
	addr	-300(fp),tos
	jsr	@_execl
	adjspb	$-36
	addr	@27,tos
	addr	@.L568,tos
	jsr	@_error
	adjspb	$-8
	exit	[r6,r7]
	ret	$0
.globl	_kill_nls
	.data
	.text
	.align	1
_kill_nls:
	enter	[],$4
	movd	8(fp),tos
	jsr	@_isactive
	adjspb	$-4
	cmpqd	$0,r0
	bne	.L572
	movd	8(fp),tos
	movd	@_arg0,tos
	addr	@.L573,tos
	addr	@__iob+32,tos
	jsr	@_fprintf
	adjspb	$-16
	addr	@23,@_Errno
	br	.I1C
.L572:
	movd	8(fp),tos
	jsr	@_find_pid
	adjspb	$-4
	movd	r0,-4(fp)
	cmpqd	$0,r0
	bne	.L574
	movd	@_arg0,tos
	addr	@.L575,tos
	addr	@__iob+32,tos
	jsr	@_fprintf
	adjspb	$-12
	addr	@13,@_Errno
.I1C:
	movqd	$0,r0
.L569:
	exit	[]
	ret	$0
.L574:
	addr	@15,tos
	movd	-4(fp),tos
	jsr	@_kill
	adjspb	$-8
	cmpqd	$0,r0
	beq	.L577
	movd	@_errno,tos
	movd	@_arg0,tos
	addr	@.L578,tos
	addr	@__iob+32,tos
	jsr	@_fprintf
	adjspb	$-16
	addr	@15,@_Errno
	br	.I1C
.L577:
	movqd	$1,r0
	br	.L569
.globl	_close_temp
	.data
	.text
	.align	1
_close_temp:
	enter	[],$0
	movd	@_tfp,tos
	jsr	@_fclose
	adjspb	$-4
	cmpqd	$-1,r0
	bne	.L582
	movd	@_errno,tos
	movd	@_arg0,tos
	movd	@_errclose,tos
	addr	@__iob+32,tos
	jsr	@_fprintf
	adjspb	$-16
	addr	@10,@_Errno
	br	.I1D
.L582:
	movd	8(fp),tos
	jsr	@_unlink
	adjspb	$-4
	cmpqd	$0,r0
	beq	.L583
	movd	@_errno,tos
	movd	8(fp),tos
	movd	@_arg0,tos
	addr	@.L584,tos
	addr	@__iob+32,tos
	jsr	@_fprintf
	adjspb	$-20
	addr	@18,@_Errno
.I1D:
	addr	@_tempfile,tos
	jsr	@_unlink
	adjspb	$-4
	br	.I1E
.L583:
	movd	8(fp),tos
	addr	@_tempfile,tos
	jsr	@_link
	adjspb	$-8
	cmpqd	$0,r0
	beq	.L586
	movd	@_errno,tos
	movd	8(fp),tos
	addr	@_tempfile,tos
	movd	@_arg0,tos
	addr	@.L587,tos
	addr	@__iob+32,tos
	jsr	@_fprintf
	adjspb	$-24
	addr	@17,@_Errno
	br	.I1D
.L586:
	addr	@_tempfile,tos
	jsr	@_unlink
	adjspb	$-4
	cmpqd	$0,r0
	beq	.L588
	movd	@_errno,tos
	addr	@_tempfile,tos
	movd	@_arg0,tos
	addr	@.L589,tos
	addr	@__iob+32,tos
	jsr	@_fprintf
	adjspb	$-20
	addr	@18,@_Errno
.I1E:
	movqd	$0,r0
.L579:
	exit	[]
	ret	$0
.L588:
	movd	@_nlsgid,tos
	movd	@_nlsuid,tos
	movd	8(fp),tos
	jsr	@_chown
	adjspb	$-12
	cmpqd	$0,r0
	ble	.L590
	movd	@_errno,tos
	movd	8(fp),tos
	movd	@_arg0,tos
	movd	@_errchown,tos
	addr	@__iob+32,tos
	jsr	@_fprintf
	adjspb	$-20
	addr	@25,@_Errno
	br	.I1E
.L590:
	movqd	$1,r0
	br	.L579
.globl	_open_temp
	.data
	.text
	.align	1
_open_temp:
	enter	[],$0
	movqd	$1,tos
	movqd	$1,tos
	jsr	@_signal
	adjspb	$-8
	movqd	$1,tos
	movqd	$2,tos
	jsr	@_signal
	adjspb	$-8
	movqd	$1,tos
	movqd	$3,tos
	jsr	@_signal
	adjspb	$-8
	addr	@219,tos
	jsr	@_umask
	adjspb	$-4
	movqd	$0,tos
	addr	@_tempfile,tos
	jsr	@_access
	adjspb	$-8
	cmpqd	$0,r0
	bgt	.L594
	movd	@_arg0,tos
	addr	@.L595,tos
	addr	@__iob+32,tos
	jsr	@_fprintf
	adjspb	$-12
	addr	@28,@_Errno
	br	.I1F
.L594:
	addr	@.L596,tos
	addr	@_tempfile,tos
	jsr	@_fopen
	adjspb	$-8
	movd	r0,@_tfp
	cmpqd	$0,r0
	bne	.L597
	movd	@_errno,tos
	movd	@_arg0,tos
	addr	@.L598,tos
	addr	@__iob+32,tos
	jsr	@_fprintf
	adjspb	$-16
	movqd	$6,@_Errno
.I1F:
	movqd	$0,r0
.L591:
	exit	[]
	ret	$0
.L597:
	addr	@18,tos
	jsr	@_umask
	adjspb	$-4
	movqd	$1,r0
	br	.L591
.globl	_find_serv
	.data
	.text
	.align	1
_find_serv:
	enter	[r6,r7],$0
	movqd	$-1,r6
	br	.L604
.L605:
	cmpqd	$-1,r6
	bne	.L606
	movd	r7,r6
	br	.L607
.L606:
	movd	r6,r0
	addd	r7,r0
	addqd	$1,r0
	movd	r0,r6
.L607:
	addr	@16,tos
	movd	12(fp),tos
	addr	@_serv_str,tos
	jsr	@_strncmp
	adjspb	$-12
	cmpqd	$0,r0
	bne	.L608
	movd	r6,r0
.L599:
	exit	[r6,r7]
	ret	$0
.L608:
.L604:
	movd	8(fp),tos
	jsr	@_read_dbf
	adjspb	$-4
	movd	r0,r7
	cmpqd	$0,r0
	ble	.L605
	cmpqd	$-2,r7
	bne	.L609
	movd	@_Errno,tos
	addr	@.L610,tos
	jsr	@_error
	adjspb	$-8
.L609:
	movqd	$-1,r0
	br	.L599
.globl	_print_spec
	.data
	.text
	.align	1
_print_spec:
	enter	[r7],$12
	addr	@.L614,tos
	movd	8(fp),tos
	jsr	@_fopen
	adjspb	$-8
	movd	r0,-4(fp)
	cmpqd	$0,r0
	bne	.L615
	movd	@_Netspec,tos
	movd	@_arg0,tos
	movd	@_erropen,tos
	addr	@__iob+32,tos
	jsr	@_fprintf
	adjspb	$-16
	movqd	$5,@_Errno
.L611:
	exit	[r7]
	ret	$0
.L618:
	addr	@88,tos
	addr	@_flag_str,tos
	jsr	@_strchr
	adjspb	$-8
	movd	r0,-12(fp)
	addr	@120,tos
	addr	@_flag_str,tos
	jsr	@_strchr
	adjspb	$-8
	addd	-12(fp),r0
	movd	r0,-8(fp)
	cmpb	@_mod_str,$0
	bne	.L619
	addr	@_cmnt_str,tos
	addr	@_path_str,tos
	cmpqd	$0,-8(fp)
	beq	.L9968
	addr	@.L621,r0
	br	.L9967
.L9968:
	addr	@.L622,r0
.L9967:
	movd	r0,tos
	addr	@_serv_str,tos
	addr	@.L620,tos
	jsr	@_printf
	adjspb	$-20
	br	.L623
.L619:
	addr	@_cmnt_str,tos
	addr	@_path_str,tos
	addr	@_mod_str,tos
	cmpqd	$0,-8(fp)
	beq	.L9966
	addr	@.L625,r0
	br	.L9965
.L9966:
	addr	@.L626,r0
.L9965:
	movd	r0,tos
	addr	@_serv_str,tos
	addr	@.L624,tos
	jsr	@_printf
	adjspb	$-24
.L615:
.L623:
	movd	-4(fp),tos
	jsr	@_read_dbf
	adjspb	$-4
	movd	r0,r7
	cmpqd	$0,r0
	ble	.L618
	cmpqd	$-2,r7
	bne	.L627
	movd	@_Errno,tos
	addr	@.L628,tos
	jsr	@_error
	adjspb	$-8
.L627:
	movd	-4(fp),tos
	jsr	@_fclose
	adjspb	$-4
	br	.L611
.globl	_print_all
	.data
	.text
	.align	1
_print_all:
	enter	[r7],$4
	movd	8(fp),tos
	jsr	@_strlen
	adjspb	$-4
	addr	16(r0),tos
	jsr	@_malloc
	adjspb	$-4
	movd	r0,-4(fp)
	cmpqd	$0,r0
	bne	.L633
	movqd	$4,tos
	movd	@_errmalloc,tos
	jsr	@_error
	adjspb	$-8
.L633:
	movd	8(fp),tos
	jsr	@_read_dir
	adjspb	$-4
	movd	r0,r7
	cmpqd	$0,r0
	bne	.L634
.L629:
	exit	[r7]
	ret	$0
.L638:
	movd	r7,tos
	movd	8(fp),tos
	addr	@.L639,tos
	movd	-4(fp),tos
	jsr	@_sprintf
	adjspb	$-16
	movd	-4(fp),tos
	jsr	@_isdir
	adjspb	$-4
	cmpqd	$0,r0
	beq	.L640
	movd	-4(fp),tos
	jsr	@_process_dir
	adjspb	$-4
.L640:
	movd	16(r7),r7
.L634:
	cmpqd	$0,16(r7)
	bne	.L638
	br	.L629
.globl	_read_dir
	.data
	.text
	.align	1
_read_dir:
	enter	[r4,r5,r6,r7],$20
	movqd	$0,r7
	addr	@.L645,tos
	movd	8(fp),tos
	jsr	@_fopen
	adjspb	$-8
	movd	r0,-4(fp)
	cmpqd	$0,r0
	bne	.L646
	movd	8(fp),tos
	movd	@_arg0,tos
	addr	@.L647,tos
	addr	@__iob+32,tos
	jsr	@_fprintf
	adjspb	$-16
	addr	@19,@_Errno
	movqd	$0,r0
.L642:
	exit	[r4,r5,r6,r7]
	ret	$0
.L646:
	addr	@20,tos
	jsr	@_malloc
	adjspb	$-4
	movd	r0,r7
	cmpqd	$0,r0
	bne	.L648
	movqd	$4,tos
	movd	@_errmalloc,tos
	jsr	@_error
	adjspb	$-8
.L648:
	movqd	$0,16(r7)
	movd	r7,r6
.L651:
	movd	-4(fp),tos
	movqd	$1,tos
	addr	@16,tos
	addr	-20(fp),tos
	jsr	@_fread
	adjspb	$-16
	cmpqd	$1,r0
	bne	.L650
	cmpqw	$0,-20(fp)
	beq	.L651
	cmpb	-18(fp),$46
	beq	.L651
	addr	@20,tos
	jsr	@_malloc
	adjspb	$-4
	movd	r0,r5
	cmpqd	$0,r0
	bne	.L654
	movqd	$4,tos
	movd	@_errmalloc,tos
	jsr	@_error
	adjspb	$-8
.L654:
	movqd	$0,16(r5)
	movqd	$0,r4
	br	.L657
.L650:
	movd	-4(fp),tos
	jsr	@_fclose
	adjspb	$-4
	movd	r7,r0
	br	.L642
.L658:
	movb	-18(fp)[r4:b],r6[r4:b]
	addqd	$1,r4
.L657:
	addr	@14,r0
	cmpd	r4,r0
	blt	.L658
	movd	r5,16(r6)
	movd	r5,r6
	movqd	$0,r5
	br	.L651
.globl	_process_dir
	.data
	.text
	.align	1
_process_dir:
	enter	[r6,r7],$4
	movd	8(fp),tos
	jsr	@_read_dir
	adjspb	$-4
	movd	r0,r7
	cmpqd	$0,r0
	bne	.L662
.L659:
	exit	[r6,r7]
	ret	$0
.L666:
	movd	8(fp),tos
	jsr	@_strlen
	adjspb	$-4
	addr	16(r0),tos
	jsr	@_malloc
	adjspb	$-4
	movd	r0,r6
	cmpqd	$0,r0
	bne	.L667
	movqd	$4,tos
	movd	@_errmalloc,tos
	jsr	@_error
	adjspb	$-8
.L667:
	movd	r7,tos
	movd	8(fp),tos
	addr	@.L668,tos
	movd	r6,tos
	jsr	@_sprintf
	adjspb	$-16
	movd	r6,tos
	jsr	@_isdir
	adjspb	$-4
	cmpqd	$0,r0
	beq	.L669
	movd	r6,tos
	jsr	@_process_dir
	adjspb	$-4
	br	.L670
.L669:
	addr	@.L671,tos
	movd	r7,tos
	jsr	@_strcmp
	adjspb	$-8
	cmpqd	$0,r0
	bne	.L672
	addr	@_homedir,tos
	jsr	@_strlen
	adjspb	$-4
	addd	8(fp),r0
	addqd	$1,r0
	movd	r0,-4(fp)
	movd	r0,tos
	jsr	@_isactive
	adjspb	$-4
	cmpqd	$0,r0
	beq	.L9963
	addr	@.L674,r0
	br	.L9962
.L9963:
	addr	@.L675,r0
.L9962:
	movd	r0,tos
	movd	-4(fp),tos
	addr	@.L673,tos
	jsr	@_printf
	adjspb	$-12
.L672:
.L670:
	movd	r6,tos
	jsr	@_free
	adjspb	$-4
	movd	16(r7),r7
.L662:
	cmpqd	$0,16(r7)
	bne	.L666
	br	.L659
.globl	_makedir
	.data
	.text
	.align	1
_makedir:
	enter	[r7],$4
	movd	12(fp),tos
	jsr	@_strlen
	adjspb	$-4
	addqd	$5,r0
	movd	r0,tos
	jsr	@_malloc
	adjspb	$-4
	movd	r0,r7
	cmpqd	$0,r0
	bne	.L679
	movqd	$4,tos
	movd	@_errmalloc,tos
	jsr	@_error
	adjspb	$-8
.L679:
	movd	12(fp),tos
	movd	r7,tos
	jsr	@_strcpy
	adjspb	$-8
	movqd	$0,tos
	movd	$16895,tos
	movd	r7,tos
	jsr	@_mknod
	adjspb	$-12
	cmpqd	$0,r0
	beq	.L681
	movd	@_errno,tos
	movd	r7,tos
	movd	@_arg0,tos
	addr	@.L682,tos
	addr	@__iob+32,tos
	jsr	@_fprintf
	adjspb	$-20
	movqd	$6,@_Errno
	br	.I20
.L681:
	movd	@_nlsgid,tos
	movd	@_nlsuid,tos
	movd	r7,tos
	jsr	@_chown
	adjspb	$-12
	cmpqd	$0,r0
	ble	.L683
	movd	@_errno,tos
	movd	r7,tos
	movd	@_arg0,tos
	movd	@_errchown,tos
	addr	@__iob+32,tos
	jsr	@_fprintf
	adjspb	$-20
	addr	@25,@_Errno
.I20:
	movqd	$0,r0
.L676:
	exit	[r7]
	ret	$0
.L683:
	addr	@.L684,tos
	movd	r7,tos
	jsr	@_strcat
	adjspb	$-8
	movd	r7,tos
	movd	12(fp),tos
	jsr	@_link
	adjspb	$-8
	cmpqd	$0,r0
	beq	.L685
	movd	@_errno,tos
	movd	r7,tos
	movd	@_arg0,tos
	addr	@.L686,tos
	addr	@__iob+32,tos
	jsr	@_fprintf
	adjspb	$-20
	addr	@17,@_Errno
	br	.I21
.L685:
	addr	@.L687,tos
	movd	r7,tos
	jsr	@_strcat
	adjspb	$-8
	movd	r7,tos
	movd	8(fp),tos
	jsr	@_link
	adjspb	$-8
	cmpqd	$0,r0
	beq	.L688
	movd	@_errno,tos
	movd	r7,tos
	movd	@_arg0,tos
	addr	@.L689,tos
	addr	@__iob+32,tos
	jsr	@_fprintf
	adjspb	$-20
	addr	@17,@_Errno
	movd	r7,tos
	jsr	@_strlen
	adjspb	$-4
	addd	r7,r0
	addqd	$-1,r0
	movd	r0,-4(fp)
	movqb	$0,0(-4(fp))
	movd	r7,tos
	jsr	@_unlink
	adjspb	$-4
.I21:
	movd	12(fp),tos
	jsr	@_unlink
	adjspb	$-4
	br	.I20
.L688:
	movqd	$1,r0
	br	.L676
.globl	_print_serv
	.data
	.text
	.align	1
_print_serv:
	enter	[r6,r7],$4
	addr	@.L693,tos
	movd	8(fp),tos
	jsr	@_fopen
	adjspb	$-8
	movd	r0,r7
	cmpqd	$0,r0
	bne	.L694
	movd	@_Netspec,tos
	movd	@_arg0,tos
	movd	@_erropen,tos
	addr	@__iob+32,tos
	jsr	@_fprintf
	adjspb	$-16
	movqd	$5,tos
	jsr	@_exit
	adjspb	$-4
.L694:
	movd	12(fp),tos
	movd	r7,tos
	jsr	@_find_serv
	adjspb	$-8
	cmpqd	$-1,r0
	bne	.L695
	movd	8(fp),tos
	movd	12(fp),tos
	movd	@_arg0,tos
	movd	@_errscode,tos
	addr	@__iob+32,tos
	jsr	@_fprintf
	adjspb	$-20
	addr	@11,tos
	jsr	@_exit
	adjspb	$-4
.L695:
	movd	r7,tos
	jsr	@_fclose
	adjspb	$-4
	addr	@88,tos
	addr	@_flag_str,tos
	jsr	@_strchr
	adjspb	$-8
	movd	r0,-4(fp)
	addr	@120,tos
	addr	@_flag_str,tos
	jsr	@_strchr
	adjspb	$-8
	addd	-4(fp),r0
	movd	r0,r6
	cmpqd	$0,r6
	beq	.L696
	movqd	$1,r6
.L696:
	cmpqd	$0,16(fp)
	beq	.L697
	movd	r6,tos
	jsr	@_exit
	adjspb	$-4
.L697:
	cmpb	@_mod_str,$0
	bne	.L698
	addr	@_cmnt_str,tos
	addr	@_path_str,tos
	cmpqd	$0,r6
	beq	.L9961
	addr	@.L700,r0
	br	.L9960
.L9961:
	addr	@.L701,r0
.L9960:
	movd	r0,tos
	addr	@_serv_str,tos
	addr	@.L699,tos
	jsr	@_printf
	adjspb	$-20
.L702:
	movqd	$0,tos
	jsr	@_exit
	adjspb	$-4
	exit	[r6,r7]
	ret	$0
.L698:
	addr	@_cmnt_str,tos
	addr	@_path_str,tos
	addr	@_mod_str,tos
	cmpqd	$0,r6
	beq	.L9959
	addr	@.L704,r0
	br	.L9958
.L9959:
	addr	@.L705,r0
.L9958:
	movd	r0,tos
	addr	@_serv_str,tos
	addr	@.L703,tos
	jsr	@_printf
	adjspb	$-24
	br	.L702
.globl	_ok_netspec
	.data
.L87:
	.byte	0x2f,0x2e,0x6e,0x6c,0x73,0x74,0x6d,0x70
	.byte	0x0
.L115:
	.byte	0x61,0x64,0x6d,0x0
.L117:
	.byte	0x25,0x73,0x3a,0x20,0x6e,0x6f,0x20,0x67
	.byte	0x72,0x6f,0x75,0x70,0x20,0x65,0x6e,0x74
	.byte	0x72,0x79,0x20,0x66,0x6f,0x72,0x20,0x25
	.byte	0x73,0x3f,0xa,0x0
.L118:
	.byte	0x61,0x64,0x6d,0x0
.L119:
	.byte	0x6c,0x69,0x73,0x74,0x65,0x6e,0x0
.L121:
	.byte	0x25,0x73,0x3a,0x20,0x63,0x6f,0x75,0x6c
	.byte	0x64,0x20,0x6e,0x6f,0x74,0x20,0x66,0x69
	.byte	0x6e,0x64,0x20,0x63,0x6f,0x72,0x72,0x65
	.byte	0x63,0x74,0x20,0x70,0x61,0x73,0x77,0x6f
	.byte	0x72,0x64,0x20,0x65,0x6e,0x74,0x72,0x79
	.byte	0x20,0x66,0x6f,0x72,0x20,0x25,0x73,0xa
	.byte	0x0
.L122:
	.byte	0x6c,0x69,0x73,0x74,0x65,0x6e,0x0
.L125:
	.byte	0x61,0x3a,0x63,0x3a,0x64,0x3a,0x65,0x3a
	.byte	0x69,0x6b,0x6c,0x3a,0x6d,0x6e,0x70,0x3a
	.byte	0x71,0x72,0x3a,0x73,0x74,0x3a,0x76,0x78
	.byte	0x79,0x3a,0x7a,0x3a,0x0
.L196:
	.byte	0x64,0x62,0x66,0x0
.L198:
	.byte	0x25,0x73,0x2f,0x25,0x73,0x2f,0x25,0x73
	.byte	0x0
.L199:
	.byte	0x64,0x62,0x66,0x0
.L201:
	.byte	0x2f,0x0
.L214:
	.byte	0x65,0x72,0x72,0x6f,0x72,0x20,0x69,0x6e
	.byte	0x20,0x6d,0x61,0x6b,0x69,0x6e,0x67,0x20
	.byte	0x64,0x61,0x74,0x61,0x62,0x61,0x73,0x65
	.byte	0x20,0x66,0x69,0x6c,0x65,0x0
.L216:
	.byte	0x6e,0x65,0x74,0x5f,0x73,0x70,0x65,0x63
	.byte	0x20,0x61,0x6c,0x72,0x65,0x61,0x64,0x79
	.byte	0x20,0x69,0x6e,0x69,0x74,0x69,0x61,0x6c
	.byte	0x69,0x7a,0x65,0x64,0x0
.L223:
	.byte	0x63,0x6f,0x75,0x6c,0x64,0x20,0x6e,0x6f
	.byte	0x74,0x20,0x61,0x64,0x64,0x20,0x73,0x65
	.byte	0x72,0x76,0x69,0x63,0x65,0x0
.L230:
	.byte	0x63,0x6f,0x75,0x6c,0x64,0x20,0x6e,0x6f
	.byte	0x74,0x20,0x72,0x65,0x6d,0x6f,0x76,0x65
	.byte	0x20,0x73,0x65,0x72,0x76,0x69,0x63,0x65
	.byte	0x0
.L237:
	.byte	0x63,0x6f,0x75,0x6c,0x64,0x20,0x6e,0x6f
	.byte	0x74,0x20,0x65,0x6e,0x61,0x62,0x6c,0x65
	.byte	0x20,0x73,0x65,0x72,0x76,0x69,0x63,0x65
	.byte	0x0
.L244:
	.byte	0x63,0x6f,0x75,0x6c,0x64,0x20,0x6e,0x6f
	.byte	0x74,0x20,0x64,0x69,0x73,0x61,0x62,0x6c
	.byte	0x65,0x20,0x73,0x65,0x72,0x76,0x69,0x63
	.byte	0x65,0x0
.L259:
	.byte	0x25,0x73,0x9,0x25,0x73,0xa,0x0
.L260:
	.byte	0x41,0x43,0x54,0x49,0x56,0x45,0x0
.L261:
	.byte	0x49,0x4e,0x41,0x43,0x54,0x49,0x56,0x45
	.byte	0x0
.L270:
	.byte	0x65,0x72,0x72,0x6f,0x72,0x20,0x69,0x6e
	.byte	0x20,0x61,0x63,0x63,0x65,0x73,0x73,0x69
	.byte	0x6e,0x67,0x20,0x61,0x64,0x64,0x72,0x65
	.byte	0x73,0x73,0x20,0x66,0x69,0x6c,0x65,0x0
.L282:
	.byte	0x25,0x73,0x3a,0x20,0x75,0x73,0x61,0x67
	.byte	0x65,0x3a,0x20,0x25,0x73,0x20,0x2d,0x78
	.byte	0x20,0x20,0x20,0x6f,0x72,0x20,0x20,0x20
	.byte	0x25,0x73,0x20,0x5b,0x20,0x6f,0x70,0x74
	.byte	0x69,0x6f,0x6e,0x73,0x20,0x5d,0x20,0x6e
	.byte	0x65,0x74,0x5f,0x73,0x70,0x65,0x63,0xa
	.byte	0x9,0x77,0x68,0x65,0x72,0x65,0x20,0x5b
	.byte	0x20,0x6f,0x70,0x74,0x69,0x6f,0x6e,0x73
	.byte	0x20,0x5d,0x20,0x61,0x72,0x65,0x3a,0xa
	.byte	0x5b,0x2d,0x71,0x5d,0x20,0x20,0x7c,0x20
	.byte	0x20,0x5b,0x2d,0x76,0x5d,0x20,0x20,0x7c
	.byte	0x20,0x20,0x5b,0x2d,0x73,0x5d,0x20,0x20
	.byte	0x7c,0x20,0x20,0x5b,0x2d,0x6b,0x5d,0x20
	.byte	0x20,0x7c,0x20,0x20,0x5b,0x2d,0x69,0x5d
	.byte	0x20,0x20,0x7c,0x20,0xa,0x5b,0x20,0x5b
	.byte	0x2d,0x6c,0x20,0x61,0x64,0x64,0x72,0x65
	.byte	0x73,0x73,0x7c,0x2d,0x5d,0x20,0x20,0x5b
	.byte	0x2d,0x74,0x20,0x61,0x64,0x64,0x72,0x65
	.byte	0x73,0x73,0x7c,0x2d,0x5d,0x20,0x5d,0x20
	.byte	0x20,0x7c,0xa,0x5b,0x20,0x5b,0x2d,0x6d
	.byte	0x5d,0x20,0x2d,0x61,0x20,0x73,0x76,0x63
	.byte	0x5f,0x63,0x6f,0x64,0x65,0x20,0x2d,0x63
	.byte	0x20,0x22,0x63,0x6d,0x64,0x22,0x20,0x2d
	.byte	0x79,0x20,0x22,0x63,0x6f,0x6d,0x6d,0x65
	.byte	0x6e,0x74,0x22,0x20,0x20,0x5b,0x2d,0x70
	.byte	0x20,0x6d,0x6f,0x64,0x75,0x6c,0x65,0x5f
	.byte	0x6c,0x69,0x73,0x74,0x5d,0x20,0x5d,0x20
	.byte	0x20,0x7c,0xa,0x5b,0x5b,0x2d,0x71,0x5d
	.byte	0x20,0x2d,0x7a,0x20,0x73,0x76,0x63,0x5f
	.byte	0x63,0x6f,0x64,0x65,0x5d,0x20,0x20,0x7c
	.byte	0x20,0x20,0x5b,0x2d,0x72,0x20,0x73,0x76
	.byte	0x63,0x5f,0x63,0x6f,0x64,0x65,0x5d,0x20
	.byte	0x20,0x7c,0x20,0x20,0x5b,0x2d,0x64,0x20
	.byte	0x73,0x76,0x63,0x5f,0x63,0x6f,0x64,0x65
	.byte	0x5d,0x20,0x20,0x7c,0x20,0x20,0x5b,0x2d
	.byte	0x65,0x20,0x73,0x76,0x63,0x5f,0x63,0x6f
	.byte	0x64,0x65,0x5d,0xa,0x0
.L289:
	.byte	0x25,0x73,0x3a,0x20,0x25,0x73,0xa,0x0
.L298:
	.byte	0x72,0x0
.L301:
	.byte	0x73,0x65,0x72,0x76,0x69,0x63,0x65,0x20
	.byte	0x63,0x6f,0x64,0x65,0x20,0x74,0x6f,0x6f
	.byte	0x20,0x6c,0x6f,0x6e,0x67,0x0
.L304:
	.byte	0x25,0x73,0x3a,0x20,0x73,0x65,0x72,0x76
	.byte	0x69,0x63,0x65,0x20,0x63,0x6f,0x64,0x65
	.byte	0x20,0x25,0x73,0x20,0x61,0x6c,0x72,0x65
	.byte	0x61,0x64,0x79,0x20,0x65,0x78,0x69,0x73
	.byte	0x74,0x73,0x20,0x69,0x6e,0x20,0x25,0x73
	.byte	0xa,0x0
.L305:
	.byte	0x61,0x0
.L308:
	.byte	0x25,0x73,0x3a,0x25,0x73,0x3a,0x4e,0x55
	.byte	0x4c,0x4c,0x2c,0x25,0x73,0x2c,0x3a,0x25
	.byte	0x73,0x9,0x23,0x25,0x73,0xa,0x0
.L309:
	.byte	0x6e,0x61,0x0
.L310:
	.byte	0x6e,0x0
.L312:
	.byte	0x25,0x73,0x3a,0x25,0x73,0x3a,0x4e,0x55
	.byte	0x4c,0x4c,0x2c,0x3a,0x25,0x73,0x9,0x23
	.byte	0x25,0x73,0xa,0x0
.L313:
	.byte	0x6e,0x61,0x0
.L314:
	.byte	0x6e,0x0
.L316:
	.byte	0x20,0x9,0x0
.L320:
	.byte	0x25,0x73,0x3a,0x20,0x77,0x61,0x72,0x6e
	.byte	0x69,0x6e,0x67,0x20,0x2d,0x20,0x25,0x73
	.byte	0x20,0x6e,0x6f,0x74,0x20,0x65,0x78,0x65
	.byte	0x63,0x75,0x74,0x61,0x62,0x6c,0x65,0xa
	.byte	0x0
.L322:
	.byte	0x25,0x73,0x3a,0x20,0x77,0x61,0x72,0x6e
	.byte	0x69,0x6e,0x67,0x20,0x2d,0x20,0x25,0x73
	.byte	0x20,0x6e,0x6f,0x74,0x20,0x66,0x6f,0x75
	.byte	0x6e,0x64,0xa,0x0
.L326:
	.byte	0x72,0x0
.L338:
	.byte	0x72,0x0
.L361:
	.byte	0x25,0x73,0x0
.L367:
	.byte	0x72,0x0
.L389:
	.byte	0x25,0x73,0x0
.L396:
	.byte	0x25,0x73,0x3a,0x20,0x63,0x6f,0x75,0x6c
	.byte	0x64,0x20,0x6e,0x6f,0x74,0x20,0x73,0x74
	.byte	0x61,0x74,0x20,0x25,0x73,0x2c,0x20,0x65
	.byte	0x72,0x72,0x6e,0x6f,0x20,0x3d,0x20,0x25
	.byte	0x64,0xa,0x0
.L409:
	.byte	0x25,0x73,0x2f,0x25,0x73,0x0
.L411:
	.byte	0x25,0x73,0x3a,0x20,0x6e,0x65,0x74,0x5f
	.byte	0x73,0x70,0x65,0x63,0x20,0x25,0x73,0x20
	.byte	0x6e,0x6f,0x74,0x20,0x66,0x6f,0x75,0x6e
	.byte	0x64,0xa,0x0
.L412:
	.byte	0x2f,0x0
.L413:
	.byte	0x6c,0x6f,0x63,0x6b,0x0
.L419:
	.byte	0x25,0x73,0x3a,0x20,0x62,0x61,0x64,0x20
	.byte	0x6d,0x6f,0x64,0x65,0x73,0x20,0x66,0x6f
	.byte	0x72,0x20,0x66,0x69,0x6c,0x65,0x20,0x25
	.byte	0x73,0x20,0x2d,0x20,0x72,0x65,0x74,0x72
	.byte	0x79,0x20,0x61,0x73,0x20,0x73,0x75,0x70
	.byte	0x65,0x72,0x20,0x75,0x73,0x65,0x72,0xa
	.byte	0x0
.L427:
	.byte	0x25,0x73,0x2f,0x25,0x73,0x0
.L436:
	.byte	0x2f,0x0
.L437:
	.byte	0x61,0x64,0x64,0x72,0x0
.L438:
	.byte	0x77,0x0
.L440:
	.byte	0x25,0x73,0x3a,0x20,0x63,0x6f,0x75,0x6c
	.byte	0x64,0x20,0x6e,0x6f,0x74,0x20,0x63,0x72
	.byte	0x65,0x61,0x74,0x65,0x20,0x25,0x73,0x2c
	.byte	0x20,0x65,0x72,0x72,0x6e,0x6f,0x20,0x3d
	.byte	0x20,0x25,0x64,0xa,0x0
.L441:
	.byte	0x77,0x0
.L443:
	.byte	0x25,0x73,0x3a,0x20,0x63,0x6f,0x75,0x6c
	.byte	0x64,0x20,0x6e,0x6f,0x74,0x20,0x63,0x72
	.byte	0x65,0x61,0x74,0x65,0x20,0x25,0x73,0x2c
	.byte	0x20,0x65,0x72,0x72,0x6e,0x6f,0x20,0x3d
	.byte	0x20,0x25,0x64,0xa,0x0
.L448:
	.byte	0xa,0xa,0x0
.L451:
	.byte	0x25,0x73,0x2f,0x25,0x73,0x2f,0x25,0x73
	.byte	0x0
.L452:
	.byte	0x6c,0x6f,0x63,0x6b,0x0
.L453:
	.byte	0x77,0x0
.L455:
	.byte	0x25,0x73,0x3a,0x20,0x63,0x6f,0x75,0x6c
	.byte	0x64,0x20,0x6e,0x6f,0x74,0x20,0x63,0x72
	.byte	0x65,0x61,0x74,0x65,0x20,0x25,0x73,0x2c
	.byte	0x20,0x65,0x72,0x72,0x6e,0x6f,0x20,0x3d
	.byte	0x20,0x25,0x64,0xa,0x0
.L461:
	.byte	0x25,0x73,0x2f,0x25,0x73,0x2f,0x25,0x73
	.byte	0x0
.L462:
	.byte	0x61,0x64,0x64,0x72,0x0
.L463:
	.byte	0x72,0x0
.L466:
	.byte	0x25,0x73,0x3a,0x20,0x65,0x72,0x72,0x6f
	.byte	0x72,0x20,0x69,0x6e,0x20,0x72,0x65,0x61
	.byte	0x64,0x69,0x6e,0x67,0x20,0x61,0x64,0x64
	.byte	0x72,0x65,0x73,0x73,0x20,0x66,0x69,0x6c
	.byte	0x65,0xa,0x0
.L473:
	.byte	0x25,0x73,0xa,0x0
.L475:
	.byte	0x77,0x0
.L477:
	.byte	0x25,0x73,0xa,0x0
.L480:
	.byte	0x25,0x73,0xa,0x0
.L482:
	.byte	0x25,0x73,0xa,0x0
.L485:
	.byte	0x77,0x0
.L487:
	.byte	0x25,0x73,0xa,0x0
.L488:
	.byte	0x25,0x73,0xa,0x0
.L499:
	.byte	0x25,0x73,0xa,0x0
.L516:
	.byte	0x65,0x72,0x72,0x6f,0x72,0x20,0x69,0x6e
	.byte	0x20,0x77,0x72,0x69,0x74,0x69,0x6e,0x67
	.byte	0x20,0x74,0x65,0x6d,0x70,0x66,0x69,0x6c
	.byte	0x65,0x0
.L524:
	.byte	0x65,0x72,0x72,0x6f,0x72,0x20,0x69,0x6e
	.byte	0x20,0x77,0x72,0x69,0x74,0x69,0x6e,0x67
	.byte	0x20,0x74,0x65,0x6d,0x70,0x66,0x69,0x6c
	.byte	0x65,0x0
.L529:
	.byte	0x25,0x73,0x2f,0x25,0x73,0x2f,0x25,0x73
	.byte	0x0
.L530:
	.byte	0x70,0x69,0x64,0x0
.L531:
	.byte	0x72,0x0
.L533:
	.byte	0x25,0x73,0x0
.L542:
	.byte	0x6c,0x69,0x73,0x74,0x65,0x6e,0x65,0x72
	.byte	0x20,0x61,0x6c,0x72,0x65,0x61,0x64,0x79
	.byte	0x20,0x61,0x63,0x74,0x69,0x76,0x65,0x0
.L544:
	.byte	0x25,0x73,0x2f,0x25,0x73,0x2f,0x25,0x73
	.byte	0x0
.L545:
	.byte	0x6c,0x69,0x73,0x74,0x65,0x6e,0x0
.L547:
	.byte	0x25,0x73,0x2f,0x25,0x73,0x0
.L548:
	.byte	0x6c,0x69,0x73,0x74,0x65,0x6e,0x0
.L549:
	.byte	0x25,0x73,0x2f,0x25,0x73,0x2f,0x25,0x73
	.byte	0x0
.L550:
	.byte	0x61,0x64,0x64,0x72,0x0
.L551:
	.byte	0x72,0x0
.L556:
	.byte	0x25,0x73,0x3a,0x20,0x61,0x64,0x64,0x72
	.byte	0x65,0x73,0x73,0x65,0x73,0x20,0x6e,0x6f
	.byte	0x74,0x20,0x69,0x6e,0x69,0x74,0x69,0x61
	.byte	0x6c,0x69,0x7a,0x65,0x64,0x20,0x66,0x6f
	.byte	0x72,0x20,0x25,0x73,0xa,0x0
.L558:
	.byte	0x25,0x73,0x3a,0x20,0x65,0x72,0x72,0x6f
	.byte	0x72,0x20,0x69,0x6e,0x20,0x72,0x65,0x61
	.byte	0x64,0x69,0x6e,0x67,0x2c,0x20,0x6f,0x72
	.byte	0x20,0x62,0x61,0x64,0x20,0x61,0x64,0x64
	.byte	0x72,0x65,0x73,0x73,0x20,0x66,0x69,0x6c
	.byte	0x65,0xa,0x0
.L564:
	.byte	0x6c,0x69,0x73,0x74,0x65,0x6e,0x0
.L565:
	.byte	0x2d,0x6e,0x0
.L566:
	.byte	0x2d,0x6c,0x0
.L567:
	.byte	0x2d,0x72,0x0
.L568:
	.byte	0x63,0x6f,0x75,0x6c,0x64,0x20,0x6e,0x6f
	.byte	0x74,0x20,0x65,0x78,0x65,0x63,0x20,0x6c
	.byte	0x69,0x73,0x74,0x65,0x6e,0x65,0x72,0x0
.L573:
	.byte	0x25,0x73,0x3a,0x20,0x74,0x68,0x65,0x72
	.byte	0x65,0x20,0x69,0x73,0x20,0x6e,0x6f,0x20
	.byte	0x6c,0x69,0x73,0x74,0x65,0x6e,0x65,0x72
	.byte	0x20,0x61,0x63,0x74,0x69,0x76,0x65,0x20
	.byte	0x66,0x6f,0x72,0x20,0x25,0x73,0xa,0x0
.L575:
	.byte	0x25,0x73,0x3a,0x20,0x63,0x6f,0x75,0x6c
	.byte	0x64,0x20,0x6e,0x6f,0x74,0x20,0x66,0x69
	.byte	0x6e,0x64,0x20,0x70,0x69,0x64,0x20,0x74
	.byte	0x6f,0x20,0x73,0x65,0x6e,0x64,0x20,0x53
	.byte	0x49,0x47,0x54,0x45,0x52,0x4d,0xa,0x0
.L578:
	.byte	0x25,0x73,0x3a,0x20,0x63,0x6f,0x75,0x6c
	.byte	0x64,0x20,0x6e,0x6f,0x74,0x20,0x73,0x65
	.byte	0x6e,0x64,0x20,0x53,0x49,0x47,0x54,0x45
	.byte	0x52,0x4d,0x20,0x74,0x6f,0x20,0x6c,0x69
	.byte	0x73,0x74,0x65,0x6e,0x65,0x72,0x2c,0x20
	.byte	0x65,0x72,0x72,0x6e,0x6f,0x20,0x3d,0x20
	.byte	0x25,0x64,0xa,0x0
.L584:
	.byte	0x25,0x73,0x3a,0x20,0x63,0x61,0x6e,0x6e
	.byte	0x6f,0x74,0x20,0x75,0x6e,0x6c,0x69,0x6e
	.byte	0x6b,0x20,0x25,0x73,0x2c,0x20,0x65,0x72
	.byte	0x72,0x6e,0x6f,0x20,0x3d,0x20,0x25,0x64
	.byte	0xa,0x0
.L587:
	.byte	0x25,0x73,0x3a,0x20,0x63,0x61,0x6e,0x6e
	.byte	0x6f,0x74,0x20,0x6c,0x69,0x6e,0x6b,0x20
	.byte	0x25,0x73,0x20,0x74,0x6f,0x20,0x25,0x73
	.byte	0x2c,0x20,0x65,0x72,0x72,0x6e,0x6f,0x20
	.byte	0x3d,0x20,0x25,0x64,0xa,0x0
.L589:
	.byte	0x25,0x73,0x3a,0x20,0x63,0x61,0x6e,0x6e
	.byte	0x6f,0x74,0x20,0x75,0x6e,0x6c,0x69,0x6e
	.byte	0x6b,0x20,0x25,0x73,0x2c,0x20,0x65,0x72
	.byte	0x72,0x6e,0x6f,0x20,0x3d,0x20,0x25,0x64
	.byte	0xa,0x0
.L595:
	.byte	0x25,0x73,0x3a,0x20,0x74,0x65,0x6d,0x70
	.byte	0x66,0x69,0x6c,0x65,0x20,0x62,0x75,0x73
	.byte	0x79,0x3b,0x20,0x74,0x72,0x79,0x20,0x61
	.byte	0x67,0x61,0x69,0x6e,0x20,0x6c,0x61,0x74
	.byte	0x65,0x72,0xa,0x0
.L596:
	.byte	0x77,0x0
.L598:
	.byte	0x25,0x73,0x3a,0x20,0x63,0x61,0x6e,0x6e
	.byte	0x6f,0x74,0x20,0x63,0x72,0x65,0x61,0x74
	.byte	0x65,0x20,0x74,0x65,0x6d,0x70,0x66,0x69
	.byte	0x6c,0x65,0x2c,0x20,0x65,0x72,0x72,0x6e
	.byte	0x6f,0x20,0x3d,0x20,0x25,0x64,0xa,0x0
.L610:
	.byte	0x65,0x72,0x72,0x6f,0x72,0x20,0x69,0x6e
	.byte	0x20,0x72,0x65,0x61,0x64,0x69,0x6e,0x67
	.byte	0x20,0x64,0x61,0x74,0x61,0x62,0x61,0x73
	.byte	0x65,0x20,0x66,0x69,0x6c,0x65,0x0
.L614:
	.byte	0x72,0x0
.L620:
	.byte	0x25,0x73,0x9,0x25,0x73,0x9,0x4e,0x4f
	.byte	0x4d,0x4f,0x44,0x55,0x4c,0x45,0x53,0x9
	.byte	0x25,0x73,0x9,0x25,0x73,0xa,0x0
.L621:
	.byte	0x44,0x49,0x53,0x41,0x42,0x4c,0x45,0x44
	.byte	0x0
.L622:
	.byte	0x45,0x4e,0x41,0x42,0x4c,0x45,0x44,0x20
	.byte	0x0
.L624:
	.byte	0x25,0x73,0x9,0x25,0x73,0x9,0x25,0x73
	.byte	0x9,0x25,0x73,0x9,0x25,0x73,0xa,0x0
.L625:
	.byte	0x44,0x49,0x53,0x41,0x42,0x4c,0x45,0x44
	.byte	0x0
.L626:
	.byte	0x45,0x4e,0x41,0x42,0x4c,0x45,0x44,0x20
	.byte	0x0
.L628:
	.byte	0x65,0x72,0x72,0x6f,0x72,0x20,0x69,0x6e
	.byte	0x20,0x72,0x65,0x61,0x64,0x69,0x6e,0x67
	.byte	0x20,0x64,0x61,0x74,0x61,0x62,0x61,0x73
	.byte	0x65,0x20,0x66,0x69,0x6c,0x65,0x0
.L639:
	.byte	0x25,0x73,0x2f,0x25,0x73,0x0
.L645:
	.byte	0x72,0x0
.L647:
	.byte	0x25,0x73,0x3a,0x20,0x25,0x73,0x20,0x75
	.byte	0x6e,0x72,0x65,0x61,0x64,0x61,0x62,0x6c
	.byte	0x65,0xa,0x0
.L668:
	.byte	0x25,0x73,0x2f,0x25,0x73,0x0
.L671:
	.byte	0x64,0x62,0x66,0x0
.L673:
	.byte	0x25,0x73,0x9,0x25,0x73,0xa,0x0
.L674:
	.byte	0x41,0x43,0x54,0x49,0x56,0x45,0x0
.L675:
	.byte	0x49,0x4e,0x41,0x43,0x54,0x49,0x56,0x45
	.byte	0x0
.L682:
	.byte	0x25,0x73,0x3a,0x20,0x63,0x6f,0x75,0x6c
	.byte	0x64,0x20,0x6e,0x6f,0x74,0x20,0x63,0x72
	.byte	0x65,0x61,0x74,0x65,0x20,0x25,0x73,0x2c
	.byte	0x20,0x65,0x72,0x72,0x6e,0x6f,0x20,0x3d
	.byte	0x20,0x25,0x64,0xa,0x0
.L684:
	.byte	0x2f,0x2e,0x0
.L686:
	.byte	0x25,0x73,0x3a,0x20,0x63,0x6f,0x75,0x6c
	.byte	0x64,0x20,0x6e,0x6f,0x74,0x20,0x6c,0x69
	.byte	0x6e,0x6b,0x20,0x25,0x73,0x2c,0x20,0x65
	.byte	0x72,0x72,0x6e,0x6f,0x20,0x3d,0x20,0x25
	.byte	0x64,0xa,0x0
.L687:
	.byte	0x2e,0x0
.L689:
	.byte	0x25,0x73,0x3a,0x20,0x63,0x6f,0x75,0x6c
	.byte	0x64,0x20,0x6e,0x6f,0x74,0x20,0x6c,0x69
	.byte	0x6e,0x6b,0x20,0x25,0x73,0x2c,0x20,0x65
	.byte	0x72,0x72,0x6e,0x6f,0x20,0x3d,0x20,0x25
	.byte	0x64,0xa,0x0
.L693:
	.byte	0x72,0x0
.L699:
	.byte	0x25,0x73,0x9,0x25,0x73,0x9,0x4e,0x4f
	.byte	0x4d,0x4f,0x44,0x55,0x4c,0x45,0x53,0x9
	.byte	0x25,0x73,0x9,0x25,0x73,0xa,0x0
.L700:
	.byte	0x44,0x49,0x53,0x41,0x42,0x4c,0x45,0x44
	.byte	0x0
.L701:
	.byte	0x45,0x4e,0x41,0x42,0x4c,0x45,0x44,0x20
	.byte	0x0
.L703:
	.byte	0x25,0x73,0x9,0x25,0x73,0x9,0x25,0x73
	.byte	0x9,0x25,0x73,0x9,0x25,0x73,0xa,0x0
.L704:
	.byte	0x44,0x49,0x53,0x41,0x42,0x4c,0x45,0x44
	.byte	0x0
.L705:
	.byte	0x45,0x4e,0x41,0x42,0x4c,0x45,0x44,0x20
	.byte	0x0
	.text
	.align	1
_ok_netspec:
	enter	[r5,r6,r7],$0
	cmpqd	$0,8(fp)
	beq	.I22
	cmpb	0(8(fp)),$47
	beq	.I22
	movd	8(fp),r6
	br	.L713
.L714:
	cmpb	0(r6),$46
	bne	.L715
.I22:
	movqd	$0,r0
.L706:
	exit	[r5,r6,r7]
	ret	$0
.L715:
	addr	@47,tos
	movd	r6,tos
	jsr	@_strchr
	adjspb	$-8
	movd	r0,r5
	cmpqd	$0,r0
	bne	.L716
	movd	r6,tos
	jsr	@_strlen
	adjspb	$-4
	br	.I23
.L716:
	movd	r5,r0
	subd	r6,r0
	addqd	$1,r0
.I23:
	movd	r0,r7
	addd	r7,r6
.L713:
	cmpqb	$0,0(r6)
	bne	.L714
	movqd	$1,r0
	br	.L706

;	M88000 gettimeofday(2) Routine
;
	file	"gettimeofday.s"
	text
	global	__gettimeofday
__gettimeofday:
	bcnd.n	eq0,r2,chktz
	or	r5,r3,0			; save tz
	or	r4,r2,0			; save tp
	or	r9,r0,59		; time system call
	tb0	0,r0,450		; trap to Unix
	br	cerror			; return error to caller
	st	r2,r4,0			; secs
	st	r3,r4,4			; usecs
chktz:
	bcnd	eq0,r5,@Lnulltz
	subu	r31,r31,40
	st	r1,r31,32
	st	r5,r31,36
	bsr	__tzset
	ld	r1,r31,32
	ld	r5,r31,36
	addu	r31,r31,40
	or.u	r2,r0,hi16(_timezone)
	ld	r2,r2,lo16(_timezone)
	divu	r2,r2,60
	st	r2,r5,0
	or.u	r2,r0,hi16(_daylight)
	ld	r2,r2,lo16(_daylight)
	st	r2,r5,4
@Lnulltz:
	jmp.n	r1			; return to caller
	addu	r2,r0,r0

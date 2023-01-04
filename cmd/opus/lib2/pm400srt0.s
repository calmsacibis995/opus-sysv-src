; @(;)pm400srt0.s	1.2 01/31/88
; Startup code for standalone programs

; Page table entry definitions
	def	PG_V,0x1		; Page Valid
	def	PG_S,0x100		; Supervisor Page
	def	PG_WT,0x200		; Write-through Page
	def	PG_CI,0x40		; Cache-inhibit Page
	def	PG_MOD,0x200		; Write thru Page

; Opus specific
	def	COMMPAGE,0x0000		; communications page
	def	HPAGE,0x5ffff000	; ppn of hardware registers i/o style
	def	F_AT,0x16
	def	CLR_AT,6
	def	CLR_CP,4
	def	CR0,0x5fffff30
	def	SAPR,0x200		; supervisor area pointer register
	def	SCTR,0x104		; system control register
	def	SCMID,0
	def	SCMR,4
	def	SADR,0xc
	def	CSSP,0x880
	def	DCMMUBASE,0xfff07000
	def	ICMMUBASE,0xfff06000
	def	INVUATC,	0x3b		; Invalidate all user ATC
	def	INVSATC,	0x3f		; Invalidate all supv ATC

	global	start
	global	_edata
	global	_end
	global	_main
	global	__init
	global	_exit
	global	__exit
	global	_trap
	global	__r
	global	__opus
	global	_environ
	global  __dmapte
	global	__tlpagetab
	global	__pagetab0
	global	__pagetables

	text

; PAGE 0
;	The commpage

commpage:
	br	start			; branch to startup code
	zero	4092

; PAGE 1
;	Vector page
vectpage:
	br	Xtrap  		; 0 Reset vector (not the real one though)
	br	Xtrap		;
	br	interrupt	; 8 interrupt vector
	br	interrupt	; 8 interrupt vector
	br	Xcaccflt	; 10 Code fault vector
	br	Xcaccflt	; 10 Code fault vector
	br	Xdaccflt	; 18 Data fault vector - Don't ask why.
	br	Xdaccflt	; 18 Data fault vector
	br	Xaddr		; 20 Misaligned access error
	br	Xaddr		; 20 Misaligned access error
	br	Xinst		; 28 Unimplemented opcode
	br	Xinst		; 28 Unimplemented opcode
	br	Xpriv		; 30 Privelege violation vector
	br	Xpriv		; 30 Privelege violation vector
	br	Xtbnd		; 38 Trap on bounds violation vector
	br	Xtbnd		; 38 Trap on bounds violation vector
	br	Xdivz		; 40 Integer divide by zero vector
	br	Xdivz		; 40 Integer divide by zero vector
	br	Xiovf		; 48 Integer overflow vector
	br	Xiovf		; 48 Integer overflow vector
	br	Xerror		; 50 Error exception
	br	Xerror		; 50 Error exception
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap
	br	Xtrap

; PAGE 2
; 1st level page table entries
__tlpagetab:
	word	0x3000+PG_V		; second level page table
	zero	1528
	word	0x2000+PG_CI+PG_V	; hardware page table
	zero	1792
	word	0x4000+PG_CI+PG_V	; comm page table
	zero	760
	word	HPAGE+PG_CI+PG_V	; 2nd level hardware page (also IO page)

; PAGE 3
; opmon dma map entries and 2nd level page table entries
__dmapte:
__pagetab0:
	word	0x0000+PG_CI+PG_V	; commpage
	word	0x1000+PG_CI+PG_V	; vector page
	word	0x2000+PG_CI+PG_V	; 1st level page table entries + hw 2nd
	word	0x3000+PG_CI+PG_V	; 2nd level page table entries
	word	0x4000+PG_CI+PG_V	; comm page table / startup / tmp stack
	word	0x5000+PG_MOD+PG_V	
	word	0x6000+PG_MOD+PG_V	
	word	0x7000+PG_MOD+PG_V	
	word	0x8000+PG_MOD+PG_V	
	word	0x9000+PG_MOD+PG_V	
	word	0xa000+PG_MOD+PG_V	
	word	0xb000+PG_MOD+PG_V	
	word	0xc000+PG_MOD+PG_V
	word	0xd000+PG_MOD+PG_V
	word	0xe000+PG_MOD+PG_V
	word	0xf000+PG_MOD+PG_V
	word	0x10000+PG_MOD+PG_V
	word	0x11000+PG_MOD+PG_V
	word	0x12000+PG_MOD+PG_V
	word	0x13000+PG_MOD+PG_V
	word	0x14000+PG_MOD+PG_V
	word	0x15000+PG_MOD+PG_V
	word	0x16000+PG_MOD+PG_V
	word	0x17000+PG_MOD+PG_V
	word	0x18000+PG_MOD+PG_V
	word	0x19000+PG_MOD+PG_V
	word	0x1a000+PG_MOD+PG_V
	word	0x1b000+PG_MOD+PG_V
	word	0x1c000+PG_MOD+PG_V
	word	0x1d000+PG_MOD+PG_V
	word	0x1e000+PG_MOD+PG_V
	word	0x1f000+PG_MOD+PG_V
	word	0x20000+PG_MOD+PG_V
	word	0x21000+PG_MOD+PG_V
	word	0x22000+PG_MOD+PG_V
	word	0x23000+PG_MOD+PG_V
	word	0x24000+PG_MOD+PG_V
	word	0x25000+PG_MOD+PG_V
	word	0x26000+PG_MOD+PG_V
	word	0x27000+PG_MOD+PG_V
	word	0x28000+PG_MOD+PG_V
	word	0x29000+PG_MOD+PG_V
	word	0x2a000+PG_MOD+PG_V
	word	0x2b000+PG_MOD+PG_V
	word	0x2c000+PG_MOD+PG_V
	word	0x2d000+PG_MOD+PG_V
	word	0x2e000+PG_MOD+PG_V
	word	0x2f000+PG_MOD+PG_V
	word	0x30000+PG_MOD+PG_V
	word	0x31000+PG_MOD+PG_V
	word	0x32000+PG_MOD+PG_V
	word	0x33000+PG_MOD+PG_V
	word	0x34000+PG_MOD+PG_V
	word	0x35000+PG_MOD+PG_V
	word	0x36000+PG_MOD+PG_V
	word	0x37000+PG_MOD+PG_V
	word	0x38000+PG_MOD+PG_V
	word	0x39000+PG_MOD+PG_V
	word	0x3a000+PG_MOD+PG_V
	word	0x3b000+PG_MOD+PG_V
	word	0x3c000+PG_MOD+PG_V
	word	0x3d000+PG_MOD+PG_V
	word	0x3e000+PG_MOD+PG_V
	word	0x3f000+PG_MOD+PG_V
	word	0x40000+PG_MOD+PG_V
	word	0x41000+PG_MOD+PG_V
	word	0x42000+PG_MOD+PG_V
	word	0x43000+PG_MOD+PG_V
	word	0x44000+PG_MOD+PG_V
	word	0x45000+PG_MOD+PG_V
	word	0x46000+PG_MOD+PG_V
	word	0x47000+PG_MOD+PG_V
	word	0x48000+PG_MOD+PG_V
	word	0x49000+PG_MOD+PG_V
	word	0x4a000+PG_MOD+PG_V
	word	0x4b000+PG_MOD+PG_V
	word	0x4c000+PG_MOD+PG_V
	word	0x4d000+PG_MOD+PG_V
	word	0x4e000+PG_MOD+PG_V
	word	0x4f000+PG_MOD+PG_V
	word	0x50000+PG_MOD+PG_V
	word	0x51000+PG_MOD+PG_V
	word	0x52000+PG_MOD+PG_V
	word	0x53000+PG_MOD+PG_V
	word	0x54000+PG_MOD+PG_V
	word	0x55000+PG_MOD+PG_V
	word	0x56000+PG_MOD+PG_V
	word	0x57000+PG_MOD+PG_V
	word	0x58000+PG_MOD+PG_V
	word	0x59000+PG_MOD+PG_V
	word	0x5a000+PG_MOD+PG_V
	word	0x5b000+PG_MOD+PG_V
	word	0x5c000+PG_MOD+PG_V
	word	0x5d000+PG_MOD+PG_V
	word	0x5e000+PG_MOD+PG_V
	word	0x5f000+PG_MOD+PG_V
	word	0x60000+PG_MOD+PG_V
	word	0x61000+PG_MOD+PG_V
	word	0x62000+PG_MOD+PG_V
	word	0x63000+PG_MOD+PG_V
	word	0x64000+PG_MOD+PG_V
	word	0x65000+PG_MOD+PG_V
	word	0x66000+PG_MOD+PG_V
	word	0x67000+PG_MOD+PG_V
	word	0x68000+PG_MOD+PG_V
	word	0x69000+PG_MOD+PG_V
	word	0x6a000+PG_MOD+PG_V
	word	0x6b000+PG_MOD+PG_V
	word	0x6c000+PG_MOD+PG_V
	word	0x6d000+PG_MOD+PG_V
	word	0x6e000+PG_MOD+PG_V
	word	0x6f000+PG_MOD+PG_V
	word	0x70000+PG_MOD+PG_V
	word	0x71000+PG_MOD+PG_V
	word	0x72000+PG_MOD+PG_V
	word	0x73000+PG_MOD+PG_V
	word	0x74000+PG_MOD+PG_V
	word	0x75000+PG_MOD+PG_V
	word	0x76000+PG_MOD+PG_V
	word	0x77000+PG_MOD+PG_V
	word	0x78000+PG_MOD+PG_V
	word	0x79000+PG_MOD+PG_V
	word	0x7a000+PG_MOD+PG_V
	word	0x7b000+PG_MOD+PG_V
	word	0x7c000+PG_MOD+PG_V
	word	0x7d000+PG_MOD+PG_V
	word	0x7e000+PG_MOD+PG_V
	word	0x7f000+PG_MOD+PG_V
	word	0x80000+PG_MOD+PG_V
	word	0x81000+PG_MOD+PG_V
	word	0x82000+PG_MOD+PG_V
	word	0x83000+PG_MOD+PG_V
	word	0x84000+PG_MOD+PG_V
	word	0x85000+PG_MOD+PG_V
	word	0x86000+PG_MOD+PG_V
	word	0x87000+PG_MOD+PG_V
	word	0x88000+PG_MOD+PG_V
	word	0x89000+PG_MOD+PG_V
	word	0x8a000+PG_MOD+PG_V
	word	0x8b000+PG_MOD+PG_V
	word	0x8c000+PG_MOD+PG_V
	word	0x8d000+PG_MOD+PG_V
	word	0x8e000+PG_MOD+PG_V
	word	0x8f000+PG_MOD+PG_V
	word	0x90000+PG_MOD+PG_V
	word	0x91000+PG_MOD+PG_V
	word	0x92000+PG_MOD+PG_V
	word	0x93000+PG_MOD+PG_V
	word	0x94000+PG_MOD+PG_V
	word	0x95000+PG_MOD+PG_V
	word	0x96000+PG_MOD+PG_V
	word	0x97000+PG_MOD+PG_V
	word	0x98000+PG_MOD+PG_V
	word	0x99000+PG_MOD+PG_V
	word	0x9a000+PG_MOD+PG_V
	word	0x9b000+PG_MOD+PG_V
	word	0x9c000+PG_MOD+PG_V
	word	0x9d000+PG_MOD+PG_V
	word	0x9e000+PG_MOD+PG_V
	word	0x9f000+PG_MOD+PG_V
	word	0xa0000+PG_MOD+PG_V
	word	0xa1000+PG_MOD+PG_V
	word	0xa2000+PG_MOD+PG_V
	word	0xa3000+PG_MOD+PG_V
	word	0xa4000+PG_MOD+PG_V
	word	0xa5000+PG_MOD+PG_V
	word	0xa6000+PG_MOD+PG_V
	word	0xa7000+PG_MOD+PG_V
	word	0xa8000+PG_MOD+PG_V
	word	0xa9000+PG_MOD+PG_V
	word	0xaa000+PG_MOD+PG_V
	word	0xab000+PG_MOD+PG_V
	word	0xac000+PG_MOD+PG_V
	word	0xad000+PG_MOD+PG_V
	word	0xae000+PG_MOD+PG_V
	word	0xaf000+PG_MOD+PG_V
	word	0xb0000+PG_MOD+PG_V
	word	0xb1000+PG_MOD+PG_V
	word	0xb2000+PG_MOD+PG_V
	word	0xb3000+PG_MOD+PG_V
	word	0xb4000+PG_MOD+PG_V
	word	0xb5000+PG_MOD+PG_V
	word	0xb6000+PG_MOD+PG_V
	word	0xb7000+PG_MOD+PG_V
	word	0xb8000+PG_MOD+PG_V
	word	0xb9000+PG_MOD+PG_V
	word	0xba000+PG_MOD+PG_V
	word	0xbb000+PG_MOD+PG_V
	word	0xbc000+PG_MOD+PG_V
	word	0xbd000+PG_MOD+PG_V
	word	0xbe000+PG_MOD+PG_V
	word	0xbf000+PG_MOD+PG_V
	word	0xc0000+PG_MOD+PG_V
	word	0xc1000+PG_MOD+PG_V
	word	0xc2000+PG_MOD+PG_V
	word	0xc3000+PG_MOD+PG_V
	word	0xc4000+PG_MOD+PG_V
	word	0xc5000+PG_MOD+PG_V
	word	0xc6000+PG_MOD+PG_V
	word	0xc7000+PG_MOD+PG_V
	word	0xc8000+PG_MOD+PG_V
	word	0xc9000+PG_MOD+PG_V
	word	0xca000+PG_MOD+PG_V
	word	0xcb000+PG_MOD+PG_V
	word	0xcc000+PG_MOD+PG_V
	word	0xcd000+PG_MOD+PG_V
	word	0xce000+PG_MOD+PG_V
	word	0xcf000+PG_MOD+PG_V
	word	0xd0000+PG_MOD+PG_V
	word	0xd1000+PG_MOD+PG_V
	word	0xd2000+PG_MOD+PG_V
	word	0xd3000+PG_MOD+PG_V
	word	0xd4000+PG_MOD+PG_V
	word	0xd5000+PG_MOD+PG_V
	word	0xd6000+PG_MOD+PG_V
	word	0xd7000+PG_MOD+PG_V
	word	0xd8000+PG_MOD+PG_V
	word	0xd9000+PG_MOD+PG_V
	word	0xda000+PG_MOD+PG_V
	word	0xdb000+PG_MOD+PG_V
	word	0xdc000+PG_MOD+PG_V
	word	0xdd000+PG_MOD+PG_V
	word	0xde000+PG_MOD+PG_V
	word	0xdf000+PG_MOD+PG_V
	word	0xe0000+PG_MOD+PG_V
	word	0xe1000+PG_MOD+PG_V
	word	0xe2000+PG_MOD+PG_V
	word	0xe3000+PG_MOD+PG_V
	word	0xe4000+PG_MOD+PG_V
	word	0xe5000+PG_MOD+PG_V
	word	0xe6000+PG_MOD+PG_V
	word	0xe7000+PG_MOD+PG_V
	word	0xe8000+PG_MOD+PG_V
	word	0xe9000+PG_MOD+PG_V
	word	0xea000+PG_MOD+PG_V
	word	0xeb000+PG_MOD+PG_V
	word	0xec000+PG_MOD+PG_V
	word	0xed000+PG_MOD+PG_V
	word	0xee000+PG_MOD+PG_V
	word	0xef000+PG_MOD+PG_V
	word	0xf0000+PG_MOD+PG_V
	word	0xf1000+PG_MOD+PG_V
	word	0xf2000+PG_MOD+PG_V
	word	0xf3000+PG_MOD+PG_V
	word	0xf4000+PG_MOD+PG_V
	word	0xf5000+PG_MOD+PG_V
	word	0xf6000+PG_MOD+PG_V
	word	0xf7000+PG_MOD+PG_V
	word	0xf8000+PG_MOD+PG_V
	word	0xf9000+PG_MOD+PG_V
	word	0xfa000+PG_MOD+PG_V
	word	0xfb000+PG_MOD+PG_V
	word	0xfc000+PG_MOD+PG_V
	word	0xfd000+PG_MOD+PG_V
	word	0xfe000+PG_MOD+PG_V
	word	0xff000+PG_MOD+PG_V
	word	0x100000+PG_MOD+PG_V	; memory page
	word	0x101000+PG_MOD+PG_V	; memory page
	word	0x102000+PG_MOD+PG_V	; memory page
	word	0x103000+PG_MOD+PG_V	; memory page
	word	0x104000+PG_MOD+PG_V	; memory page
	word	0x105000+PG_MOD+PG_V	; memory page
	word	0x106000+PG_MOD+PG_V	; memory page
	word	0x107000+PG_MOD+PG_V	; memory page
	word	0x108000+PG_MOD+PG_V	; memory page
	word	0x109000+PG_MOD+PG_V	; memory page
	word	0x10a000+PG_MOD+PG_V	; memory page
	word	0x10b000+PG_MOD+PG_V	; memory page
	word	0x10c000+PG_MOD+PG_V	; memory page
	word	0x10d000+PG_MOD+PG_V	; memory page
	word	0x10e000+PG_MOD+PG_V	; memory page
	word	0x10f000+PG_MOD+PG_V	; memory page
	word	0x110000+PG_MOD+PG_V	; memory page
	word	0x111000+PG_MOD+PG_V	; memory page
	word	0x112000+PG_MOD+PG_V	; memory page
	word	0x113000+PG_MOD+PG_V	; memory page
	word	0x114000+PG_MOD+PG_V	; memory page
	word	0x115000+PG_MOD+PG_V	; memory page
	word	0x116000+PG_MOD+PG_V	; memory page
	word	0x117000+PG_MOD+PG_V	; memory page
	word	0x118000+PG_MOD+PG_V	; memory page
	word	0x119000+PG_MOD+PG_V	; memory page
	word	0x11a000+PG_MOD+PG_V	; memory page
	word	0x11b000+PG_MOD+PG_V	; memory page
	word	0x11c000+PG_MOD+PG_V	; memory page
	word	0x11d000+PG_MOD+PG_V	; memory page
	word	0x11e000+PG_MOD+PG_V	; memory page
	word	0x11f000+PG_MOD+PG_V	; memory page
	word	0x120000+PG_MOD+PG_V	; memory page
	word	0x121000+PG_MOD+PG_V	; memory page
	word	0x122000+PG_MOD+PG_V	; memory page
	word	0x123000+PG_MOD+PG_V	; memory page
	word	0x124000+PG_MOD+PG_V	; memory page
	word	0x125000+PG_MOD+PG_V	; memory page
	word	0x126000+PG_MOD+PG_V	; memory page
	word	0x127000+PG_MOD+PG_V	; memory page
	word	0x128000+PG_MOD+PG_V	; memory page
	word	0x129000+PG_MOD+PG_V	; memory page
	word	0x12a000+PG_MOD+PG_V	; memory page
	word	0x12b000+PG_MOD+PG_V	; memory page
	word	0x12c000+PG_MOD+PG_V	; memory page
	word	0x12d000+PG_MOD+PG_V	; memory page
	word	0x12e000+PG_MOD+PG_V	; memory page
	word	0x12f000+PG_MOD+PG_V	; memory page
	word	0x130000+PG_MOD+PG_V	; memory page
	word	0x131000+PG_MOD+PG_V	; memory page
	word	0x132000+PG_MOD+PG_V	; memory page
	word	0x133000+PG_MOD+PG_V	; memory page
	word	0x134000+PG_MOD+PG_V	; memory page
	word	0x135000+PG_MOD+PG_V	; memory page
	word	0x136000+PG_MOD+PG_V	; memory page
	word	0x137000+PG_MOD+PG_V	; memory page
	word	0x138000+PG_MOD+PG_V	; memory page
	word	0x139000+PG_MOD+PG_V	; memory page
	word	0x13a000+PG_MOD+PG_V	; memory page
	word	0x13b000+PG_MOD+PG_V	; memory page
	word	0x13c000+PG_MOD+PG_V	; memory page
	word	0x13d000+PG_MOD+PG_V	; memory page
	word	0x13e000+PG_MOD+PG_V	; memory page
	word	0x13f000+PG_MOD+PG_V	; memory page
	word	0x140000+PG_MOD+PG_V	; memory page
	word	0x141000+PG_MOD+PG_V	; memory page
	word	0x142000+PG_MOD+PG_V	; memory page
	word	0x143000+PG_MOD+PG_V	; memory page
	word	0x144000+PG_MOD+PG_V	; memory page
	word	0x145000+PG_MOD+PG_V	; memory page
	word	0x146000+PG_MOD+PG_V	; memory page
	word	0x147000+PG_MOD+PG_V	; memory page
	word	0x148000+PG_MOD+PG_V	; memory page
	word	0x149000+PG_MOD+PG_V	; memory page
	word	0x14a000+PG_MOD+PG_V	; memory page
	word	0x14b000+PG_MOD+PG_V	; memory page
	word	0x14c000+PG_MOD+PG_V	; memory page
	word	0x14d000+PG_MOD+PG_V	; memory page
	word	0x14e000+PG_MOD+PG_V	; memory page
	word	0x14f000+PG_MOD+PG_V	; memory page
	word	0x150000+PG_MOD+PG_V	; memory page
	word	0x151000+PG_MOD+PG_V	; memory page
	word	0x152000+PG_MOD+PG_V	; memory page
	word	0x153000+PG_MOD+PG_V	; memory page
	word	0x154000+PG_MOD+PG_V	; memory page
	word	0x155000+PG_MOD+PG_V	; memory page
	word	0x156000+PG_MOD+PG_V	; memory page
	word	0x157000+PG_MOD+PG_V	; memory page
	word	0x158000+PG_MOD+PG_V	; memory page
	word	0x159000+PG_MOD+PG_V	; memory page
	word	0x15a000+PG_MOD+PG_V	; memory page
	word	0x15b000+PG_MOD+PG_V	; memory page
	word	0x15c000+PG_MOD+PG_V	; memory page
	word	0x15d000+PG_MOD+PG_V	; memory page
	word	0x15e000+PG_MOD+PG_V	; memory page
	word	0x15f000+PG_MOD+PG_V	; memory page
	word	0x160000+PG_MOD+PG_V	; memory page
	word	0x161000+PG_MOD+PG_V	; memory page
	word	0x162000+PG_MOD+PG_V	; memory page
	word	0x163000+PG_MOD+PG_V	; memory page
	word	0x164000+PG_MOD+PG_V	; memory page
	word	0x165000+PG_MOD+PG_V	; memory page
	word	0x166000+PG_MOD+PG_V	; memory page
	word	0x167000+PG_MOD+PG_V	; memory page
	word	0x168000+PG_MOD+PG_V	; memory page
	word	0x169000+PG_MOD+PG_V	; memory page
	word	0x16a000+PG_MOD+PG_V	; memory page
	word	0x16b000+PG_MOD+PG_V	; memory page
	word	0x16c000+PG_MOD+PG_V	; memory page
	word	0x16d000+PG_MOD+PG_V	; memory page
	word	0x16e000+PG_MOD+PG_V	; memory page
	word	0x16f000+PG_MOD+PG_V	; memory page
	word	0x170000+PG_MOD+PG_V	; memory page
	word	0x171000+PG_MOD+PG_V	; memory page
	word	0x172000+PG_MOD+PG_V	; memory page
	word	0x173000+PG_MOD+PG_V	; memory page
	word	0x174000+PG_MOD+PG_V	; memory page
	word	0x175000+PG_MOD+PG_V	; memory page
	word	0x176000+PG_MOD+PG_V	; memory page
	word	0x177000+PG_MOD+PG_V	; memory page
	word	0x178000+PG_MOD+PG_V	; memory page
	word	0x179000+PG_MOD+PG_V	; memory page
	word	0x17a000+PG_MOD+PG_V	; memory page
	word	0x17b000+PG_MOD+PG_V	; memory page
	word	0x17c000+PG_MOD+PG_V	; memory page
	word	0x17d000+PG_MOD+PG_V	; memory page
	word	0x17e000+PG_MOD+PG_V	; memory page
	word	0x17f000+PG_MOD+PG_V	; memory page
	word	0x180000+PG_MOD+PG_V	; memory page
	word	0x181000+PG_MOD+PG_V	; memory page
	word	0x182000+PG_MOD+PG_V	; memory page
	word	0x183000+PG_MOD+PG_V	; memory page
	word	0x184000+PG_MOD+PG_V	; memory page
	word	0x185000+PG_MOD+PG_V	; memory page
	word	0x186000+PG_MOD+PG_V	; memory page
	word	0x187000+PG_MOD+PG_V	; memory page
	word	0x188000+PG_MOD+PG_V	; memory page
	word	0x189000+PG_MOD+PG_V	; memory page
	word	0x18a000+PG_MOD+PG_V	; memory page
	word	0x18b000+PG_MOD+PG_V	; memory page
	word	0x18c000+PG_MOD+PG_V	; memory page
	word	0x18d000+PG_MOD+PG_V	; memory page
	word	0x18e000+PG_MOD+PG_V	; memory page
	word	0x18f000+PG_MOD+PG_V	; memory page
	word	0x190000+PG_MOD+PG_V	; memory page
	word	0x191000+PG_MOD+PG_V	; memory page
	word	0x192000+PG_MOD+PG_V	; memory page
	word	0x193000+PG_MOD+PG_V	; memory page
	word	0x194000+PG_MOD+PG_V	; memory page
	word	0x195000+PG_MOD+PG_V	; memory page
	word	0x196000+PG_MOD+PG_V	; memory page
	word	0x197000+PG_MOD+PG_V	; memory page
	word	0x198000+PG_MOD+PG_V	; memory page
	word	0x199000+PG_MOD+PG_V	; memory page
	word	0x19a000+PG_MOD+PG_V	; memory page
	word	0x19b000+PG_MOD+PG_V	; memory page
	word	0x19c000+PG_MOD+PG_V	; memory page
	word	0x19d000+PG_MOD+PG_V	; memory page
	word	0x19e000+PG_MOD+PG_V	; memory page
	word	0x19f000+PG_MOD+PG_V	; memory page
	word	0x1a0000+PG_MOD+PG_V	; memory page
	word	0x1a1000+PG_MOD+PG_V	; memory page
	word	0x1a2000+PG_MOD+PG_V	; memory page
	word	0x1a3000+PG_MOD+PG_V	; memory page
	word	0x1a4000+PG_MOD+PG_V	; memory page
	word	0x1a5000+PG_MOD+PG_V	; memory page
	word	0x1a6000+PG_MOD+PG_V	; memory page
	word	0x1a7000+PG_MOD+PG_V	; memory page
	word	0x1a8000+PG_MOD+PG_V	; memory page
	word	0x1a9000+PG_MOD+PG_V	; memory page
	word	0x1aa000+PG_MOD+PG_V	; memory page
	word	0x1ab000+PG_MOD+PG_V	; memory page
	word	0x1ac000+PG_MOD+PG_V	; memory page
	word	0x1ad000+PG_MOD+PG_V	; memory page
	word	0x1ae000+PG_MOD+PG_V	; memory page
	word	0x1af000+PG_MOD+PG_V	; memory page
	word	0x1b0000+PG_MOD+PG_V	; memory page
	word	0x1b1000+PG_MOD+PG_V	; memory page
	word	0x1b2000+PG_MOD+PG_V	; memory page
	word	0x1b3000+PG_MOD+PG_V	; memory page
	word	0x1b4000+PG_MOD+PG_V	; memory page
	word	0x1b5000+PG_MOD+PG_V	; memory page
	word	0x1b6000+PG_MOD+PG_V	; memory page
	word	0x1b7000+PG_MOD+PG_V	; memory page
	word	0x1b8000+PG_MOD+PG_V	; memory page
	word	0x1b9000+PG_MOD+PG_V	; memory page
	word	0x1ba000+PG_MOD+PG_V	; memory page
	word	0x1bb000+PG_MOD+PG_V	; memory page
	word	0x1bc000+PG_MOD+PG_V	; memory page
	word	0x1bd000+PG_MOD+PG_V	; memory page
	word	0x1be000+PG_MOD+PG_V	; memory page
	word	0x1bf000+PG_MOD+PG_V	; memory page
	word	0x1c0000+PG_MOD+PG_V	; memory page
	word	0x1c1000+PG_MOD+PG_V	; memory page
	word	0x1c2000+PG_MOD+PG_V	; memory page
	word	0x1c3000+PG_MOD+PG_V	; memory page
	word	0x1c4000+PG_MOD+PG_V	; memory page
	word	0x1c5000+PG_MOD+PG_V	; memory page
	word	0x1c6000+PG_MOD+PG_V	; memory page
	word	0x1c7000+PG_MOD+PG_V	; memory page
	word	0x1c8000+PG_MOD+PG_V	; memory page
	word	0x1c9000+PG_MOD+PG_V	; memory page
	word	0x1ca000+PG_MOD+PG_V	; memory page
	word	0x1cb000+PG_MOD+PG_V	; memory page
	word	0x1cc000+PG_MOD+PG_V	; memory page
	word	0x1cd000+PG_MOD+PG_V	; memory page
	word	0x1ce000+PG_MOD+PG_V	; memory page
	word	0x1cf000+PG_MOD+PG_V	; memory page
	word	0x1d0000+PG_MOD+PG_V	; memory page
	word	0x1d1000+PG_MOD+PG_V	; memory page
	word	0x1d2000+PG_MOD+PG_V	; memory page
	word	0x1d3000+PG_MOD+PG_V	; memory page
	word	0x1d4000+PG_MOD+PG_V	; memory page
	word	0x1d5000+PG_MOD+PG_V	; memory page
	word	0x1d6000+PG_MOD+PG_V	; memory page
	word	0x1d7000+PG_MOD+PG_V	; memory page
	word	0x1d8000+PG_MOD+PG_V	; memory page
	word	0x1d9000+PG_MOD+PG_V	; memory page
	word	0x1da000+PG_MOD+PG_V	; memory page
	word	0x1db000+PG_MOD+PG_V	; memory page
	word	0x1dc000+PG_MOD+PG_V	; memory page
	word	0x1dd000+PG_MOD+PG_V	; memory page
	word	0x1de000+PG_MOD+PG_V	; memory page
	word	0x1df000+PG_MOD+PG_V	; memory page
	word	0x1e0000+PG_MOD+PG_V	; memory page
	word	0x1e1000+PG_MOD+PG_V	; memory page
	word	0x1e2000+PG_MOD+PG_V	; memory page
	word	0x1e3000+PG_MOD+PG_V	; memory page
	word	0x1e4000+PG_MOD+PG_V	; memory page
	word	0x1e5000+PG_MOD+PG_V	; memory page
	word	0x1e6000+PG_MOD+PG_V	; memory page
	word	0x1e7000+PG_MOD+PG_V	; memory page
	word	0x1e8000+PG_MOD+PG_V	; memory page
	word	0x1e9000+PG_MOD+PG_V	; memory page
	word	0x1ea000+PG_MOD+PG_V	; memory page
	word	0x1eb000+PG_MOD+PG_V	; memory page
	word	0x1ec000+PG_MOD+PG_V	; memory page
	word	0x1ed000+PG_MOD+PG_V	; memory page
	word	0x1ee000+PG_MOD+PG_V	; memory page
	word	0x1ef000+PG_MOD+PG_V	; memory page
	word	0x1f0000+PG_MOD+PG_V	; memory page
	word	0x1f1000+PG_MOD+PG_V	; memory page
	word	0x1f2000+PG_MOD+PG_V	; memory page
	word	0x1f3000+PG_MOD+PG_V	; memory page
	word	0x1f4000+PG_MOD+PG_V	; memory page
	word	0x1f5000+PG_MOD+PG_V	; memory page
	word	0x1f6000+PG_MOD+PG_V	; memory page
	word	0x1f7000+PG_MOD+PG_V	; memory page
	word	0x1f8000+PG_MOD+PG_V	; memory page
	word	0x1f9000+PG_MOD+PG_V	; memory page
	word	0x1fa000+PG_MOD+PG_V	; memory page
	word	0x1fb000+PG_MOD+PG_V	; memory page
	word	0x1fc000+PG_MOD+PG_V	; memory page
	word	0x1fd000+PG_MOD+PG_V	; memory page
	word	0x1fe000+PG_MOD+PG_V	; memory page
	word	0x1ff000+PG_MOD+PG_V	; memory page
	word	0x200000+PG_MOD+PG_V	; memory page
	word	0x201000+PG_MOD+PG_V	; memory page
	word	0x202000+PG_MOD+PG_V	; memory page
	word	0x203000+PG_MOD+PG_V	; memory page
	word	0x204000+PG_MOD+PG_V	; memory page
	word	0x205000+PG_MOD+PG_V	; memory page
	word	0x206000+PG_MOD+PG_V	; memory page
	word	0x207000+PG_MOD+PG_V	; memory page
	word	0x208000+PG_MOD+PG_V	; memory page
	word	0x209000+PG_MOD+PG_V	; memory page
	word	0x20a000+PG_MOD+PG_V	; memory page
	word	0x20b000+PG_MOD+PG_V	; memory page
	word	0x20c000+PG_MOD+PG_V	; memory page
	word	0x20d000+PG_MOD+PG_V	; memory page
	word	0x20e000+PG_MOD+PG_V	; memory page
	word	0x20f000+PG_MOD+PG_V	; memory page
	word	0x210000+PG_MOD+PG_V	; memory page
	word	0x211000+PG_MOD+PG_V	; memory page
	word	0x212000+PG_MOD+PG_V	; memory page
	word	0x213000+PG_MOD+PG_V	; memory page
	word	0x214000+PG_MOD+PG_V	; memory page
	word	0x215000+PG_MOD+PG_V	; memory page
	word	0x216000+PG_MOD+PG_V	; memory page
	word	0x217000+PG_MOD+PG_V	; memory page
	word	0x218000+PG_MOD+PG_V	; memory page
	word	0x219000+PG_MOD+PG_V	; memory page
	word	0x21a000+PG_MOD+PG_V	; memory page
	word	0x21b000+PG_MOD+PG_V	; memory page
	word	0x21c000+PG_MOD+PG_V	; memory page
	word	0x21d000+PG_MOD+PG_V	; memory page
	word	0x21e000+PG_MOD+PG_V	; memory page
	word	0x21f000+PG_MOD+PG_V	; memory page
	word	0x220000+PG_MOD+PG_V	; memory page
	word	0x221000+PG_MOD+PG_V	; memory page
	word	0x222000+PG_MOD+PG_V	; memory page
	word	0x223000+PG_MOD+PG_V	; memory page
	word	0x224000+PG_MOD+PG_V	; memory page
	word	0x225000+PG_MOD+PG_V	; memory page
	word	0x226000+PG_MOD+PG_V	; memory page
	word	0x227000+PG_MOD+PG_V	; memory page
	word	0x228000+PG_MOD+PG_V	; memory page
	word	0x229000+PG_MOD+PG_V	; memory page
	word	0x22a000+PG_MOD+PG_V	; memory page
	word	0x22b000+PG_MOD+PG_V	; memory page
	word	0x22c000+PG_MOD+PG_V	; memory page
	word	0x22d000+PG_MOD+PG_V	; memory page
	word	0x22e000+PG_MOD+PG_V	; memory page
	word	0x22f000+PG_MOD+PG_V	; memory page
	word	0x230000+PG_MOD+PG_V	; memory page
	word	0x231000+PG_MOD+PG_V	; memory page
	word	0x232000+PG_MOD+PG_V	; memory page
	word	0x233000+PG_MOD+PG_V	; memory page
	word	0x234000+PG_MOD+PG_V	; memory page
	word	0x235000+PG_MOD+PG_V	; memory page
	word	0x236000+PG_MOD+PG_V	; memory page
	word	0x237000+PG_MOD+PG_V	; memory page
	word	0x238000+PG_MOD+PG_V	; memory page
	word	0x239000+PG_MOD+PG_V	; memory page
	word	0x23a000+PG_MOD+PG_V	; memory page
	word	0x23b000+PG_MOD+PG_V	; memory page
	word	0x23c000+PG_MOD+PG_V	; memory page
	word	0x23d000+PG_MOD+PG_V	; memory page
	word	0x23e000+PG_MOD+PG_V	; memory page
	word	0x23f000+PG_MOD+PG_V	; memory page
	word	0x240000+PG_MOD+PG_V	; memory page
	word	0x241000+PG_MOD+PG_V	; memory page
	word	0x242000+PG_MOD+PG_V	; memory page
	word	0x243000+PG_MOD+PG_V	; memory page
	word	0x244000+PG_MOD+PG_V	; memory page
	word	0x245000+PG_MOD+PG_V	; memory page
	word	0x246000+PG_MOD+PG_V	; memory page
	word	0x247000+PG_MOD+PG_V	; memory page
	word	0x248000+PG_MOD+PG_V	; memory page
	word	0x249000+PG_MOD+PG_V	; memory page
	word	0x24a000+PG_MOD+PG_V	; memory page
	word	0x24b000+PG_MOD+PG_V	; memory page
	word	0x24c000+PG_MOD+PG_V	; memory page
	word	0x24d000+PG_MOD+PG_V	; memory page
	word	0x24e000+PG_MOD+PG_V	; memory page
	word	0x24f000+PG_MOD+PG_V	; memory page
	word	0x250000+PG_MOD+PG_V	; memory page
	word	0x251000+PG_MOD+PG_V	; memory page
	word	0x252000+PG_MOD+PG_V	; memory page
	word	0x253000+PG_MOD+PG_V	; memory page
	word	0x254000+PG_MOD+PG_V	; memory page
	word	0x255000+PG_MOD+PG_V	; memory page
	word	0x256000+PG_MOD+PG_V	; memory page
	word	0x257000+PG_MOD+PG_V	; memory page
	word	0x258000+PG_MOD+PG_V	; memory page
	word	0x259000+PG_MOD+PG_V	; memory page
	word	0x25a000+PG_MOD+PG_V	; memory page
	word	0x25b000+PG_MOD+PG_V	; memory page
	word	0x25c000+PG_MOD+PG_V	; memory page
	word	0x25d000+PG_MOD+PG_V	; memory page
	word	0x25e000+PG_MOD+PG_V	; memory page
	word	0x25f000+PG_MOD+PG_V	; memory page
	word	0x260000+PG_MOD+PG_V	; memory page
	word	0x261000+PG_MOD+PG_V	; memory page
	word	0x262000+PG_MOD+PG_V	; memory page
	word	0x263000+PG_MOD+PG_V	; memory page
	word	0x264000+PG_MOD+PG_V	; memory page
	word	0x265000+PG_MOD+PG_V	; memory page
	word	0x266000+PG_MOD+PG_V	; memory page
	word	0x267000+PG_MOD+PG_V	; memory page
	word	0x268000+PG_MOD+PG_V	; memory page
	word	0x269000+PG_MOD+PG_V	; memory page
	word	0x26a000+PG_MOD+PG_V	; memory page
	word	0x26b000+PG_MOD+PG_V	; memory page
	word	0x26c000+PG_MOD+PG_V	; memory page
	word	0x26d000+PG_MOD+PG_V	; memory page
	word	0x26e000+PG_MOD+PG_V	; memory page
	word	0x26f000+PG_MOD+PG_V	; memory page
	word	0x270000+PG_MOD+PG_V	; memory page
	word	0x271000+PG_MOD+PG_V	; memory page
	word	0x272000+PG_MOD+PG_V	; memory page
	word	0x273000+PG_MOD+PG_V	; memory page
	word	0x274000+PG_MOD+PG_V	; memory page
	word	0x275000+PG_MOD+PG_V	; memory page
	word	0x276000+PG_MOD+PG_V	; memory page
	word	0x277000+PG_MOD+PG_V	; memory page
	word	0x278000+PG_MOD+PG_V	; memory page
	word	0x279000+PG_MOD+PG_V	; memory page
	word	0x27a000+PG_MOD+PG_V	; memory page
	word	0x27b000+PG_MOD+PG_V	; memory page
	word	0x27c000+PG_MOD+PG_V	; memory page
	word	0x27d000+PG_MOD+PG_V	; memory page
	word	0x27e000+PG_MOD+PG_V	; memory page
	word	0x27f000+PG_MOD+PG_V	; memory page
	word	0x280000+PG_MOD+PG_V	; memory page
	word	0x281000+PG_MOD+PG_V	; memory page
	word	0x282000+PG_MOD+PG_V	; memory page
	word	0x283000+PG_MOD+PG_V	; memory page
	word	0x284000+PG_MOD+PG_V	; memory page
	word	0x285000+PG_MOD+PG_V	; memory page
	word	0x286000+PG_MOD+PG_V	; memory page
	word	0x287000+PG_MOD+PG_V	; memory page
	word	0x288000+PG_MOD+PG_V	; memory page
	word	0x289000+PG_MOD+PG_V	; memory page
	word	0x28a000+PG_MOD+PG_V	; memory page
	word	0x28b000+PG_MOD+PG_V	; memory page
	word	0x28c000+PG_MOD+PG_V	; memory page
	word	0x28d000+PG_MOD+PG_V	; memory page
	word	0x28e000+PG_MOD+PG_V	; memory page
	word	0x28f000+PG_MOD+PG_V	; memory page
	word	0x290000+PG_MOD+PG_V	; memory page
	word	0x291000+PG_MOD+PG_V	; memory page
	word	0x292000+PG_MOD+PG_V	; memory page
	word	0x293000+PG_MOD+PG_V	; memory page
	word	0x294000+PG_MOD+PG_V	; memory page
	word	0x295000+PG_MOD+PG_V	; memory page
	word	0x296000+PG_MOD+PG_V	; memory page
	word	0x297000+PG_MOD+PG_V	; memory page
	word	0x298000+PG_MOD+PG_V	; memory page
	word	0x299000+PG_MOD+PG_V	; memory page
	word	0x29a000+PG_MOD+PG_V	; memory page
	word	0x29b000+PG_MOD+PG_V	; memory page
	word	0x29c000+PG_MOD+PG_V	; memory page
	word	0x29d000+PG_MOD+PG_V	; memory page
	word	0x29e000+PG_MOD+PG_V	; memory page
	word	0x29f000+PG_MOD+PG_V	; memory page
	word	0x2a0000+PG_MOD+PG_V	; memory page
	word	0x2a1000+PG_MOD+PG_V	; memory page
	word	0x2a2000+PG_MOD+PG_V	; memory page
	word	0x2a3000+PG_MOD+PG_V	; memory page
	word	0x2a4000+PG_MOD+PG_V	; memory page
	word	0x2a5000+PG_MOD+PG_V	; memory page
	word	0x2a6000+PG_MOD+PG_V	; memory page
	word	0x2a7000+PG_MOD+PG_V	; memory page
	word	0x2a8000+PG_MOD+PG_V	; memory page
	word	0x2a9000+PG_MOD+PG_V	; memory page
	word	0x2aa000+PG_MOD+PG_V	; memory page
	word	0x2ab000+PG_MOD+PG_V	; memory page
	word	0x2ac000+PG_MOD+PG_V	; memory page
	word	0x2ad000+PG_MOD+PG_V	; memory page
	word	0x2ae000+PG_MOD+PG_V	; memory page
	word	0x2af000+PG_MOD+PG_V	; memory page
	word	0x2b0000+PG_MOD+PG_V	; memory page
	word	0x2b1000+PG_MOD+PG_V	; memory page
	word	0x2b2000+PG_MOD+PG_V	; memory page
	word	0x2b3000+PG_MOD+PG_V	; memory page
	word	0x2b4000+PG_MOD+PG_V	; memory page
	word	0x2b5000+PG_MOD+PG_V	; memory page
	word	0x2b6000+PG_MOD+PG_V	; memory page
	word	0x2b7000+PG_MOD+PG_V	; memory page
	word	0x2b8000+PG_MOD+PG_V	; memory page
	word	0x2b9000+PG_MOD+PG_V	; memory page
	word	0x2ba000+PG_MOD+PG_V	; memory page
	word	0x2bb000+PG_MOD+PG_V	; memory page
	word	0x2bc000+PG_MOD+PG_V	; memory page
	word	0x2bd000+PG_MOD+PG_V	; memory page
	word	0x2be000+PG_MOD+PG_V	; memory page
	word	0x2bf000+PG_MOD+PG_V	; memory page
	word	0x2c0000+PG_MOD+PG_V	; memory page
	word	0x2c1000+PG_MOD+PG_V	; memory page
	word	0x2c2000+PG_MOD+PG_V	; memory page
	word	0x2c3000+PG_MOD+PG_V	; memory page
	word	0x2c4000+PG_MOD+PG_V	; memory page
	word	0x2c5000+PG_MOD+PG_V	; memory page
	word	0x2c6000+PG_MOD+PG_V	; memory page
	word	0x2c7000+PG_MOD+PG_V	; memory page
	word	0x2c8000+PG_MOD+PG_V	; memory page
	word	0x2c9000+PG_MOD+PG_V	; memory page
	word	0x2ca000+PG_MOD+PG_V	; memory page
	word	0x2cb000+PG_MOD+PG_V	; memory page
	word	0x2cc000+PG_MOD+PG_V	; memory page
	word	0x2cd000+PG_MOD+PG_V	; memory page
	word	0x2ce000+PG_MOD+PG_V	; memory page
	word	0x2cf000+PG_MOD+PG_V	; memory page
	word	0x2d0000+PG_MOD+PG_V	; memory page
	word	0x2d1000+PG_MOD+PG_V	; memory page
	word	0x2d2000+PG_MOD+PG_V	; memory page
	word	0x2d3000+PG_MOD+PG_V	; memory page
	word	0x2d4000+PG_MOD+PG_V	; memory page
	word	0x2d5000+PG_MOD+PG_V	; memory page
	word	0x2d6000+PG_MOD+PG_V	; memory page
	word	0x2d7000+PG_MOD+PG_V	; memory page
	word	0x2d8000+PG_MOD+PG_V	; memory page
	word	0x2d9000+PG_MOD+PG_V	; memory page
	word	0x2da000+PG_MOD+PG_V	; memory page
	word	0x2db000+PG_MOD+PG_V	; memory page
	word	0x2dc000+PG_MOD+PG_V	; memory page
	word	0x2dd000+PG_MOD+PG_V	; memory page
	word	0x2de000+PG_MOD+PG_V	; memory page
	word	0x2df000+PG_MOD+PG_V	; memory page
	word	0x2e0000+PG_MOD+PG_V	; memory page
	word	0x2e1000+PG_MOD+PG_V	; memory page
	word	0x2e2000+PG_MOD+PG_V	; memory page
	word	0x2e3000+PG_MOD+PG_V	; memory page
	word	0x2e4000+PG_MOD+PG_V	; memory page
	word	0x2e5000+PG_MOD+PG_V	; memory page
	word	0x2e6000+PG_MOD+PG_V	; memory page
	word	0x2e7000+PG_MOD+PG_V	; memory page
	word	0x2e8000+PG_MOD+PG_V	; memory page
	word	0x2e9000+PG_MOD+PG_V	; memory page
	word	0x2ea000+PG_MOD+PG_V	; memory page
	word	0x2eb000+PG_MOD+PG_V	; memory page
	word	0x2ec000+PG_MOD+PG_V	; memory page
	word	0x2ed000+PG_MOD+PG_V	; memory page
	word	0x2ee000+PG_MOD+PG_V	; memory page
	word	0x2ef000+PG_MOD+PG_V	; memory page
	word	0x2f0000+PG_MOD+PG_V	; memory page
	word	0x2f1000+PG_MOD+PG_V	; memory page
	word	0x2f2000+PG_MOD+PG_V	; memory page
	word	0x2f3000+PG_MOD+PG_V	; memory page
	word	0x2f4000+PG_MOD+PG_V	; memory page
	word	0x2f5000+PG_MOD+PG_V	; memory page
	word	0x2f6000+PG_MOD+PG_V	; memory page
	word	0x2f7000+PG_MOD+PG_V	; memory page
	word	0x2f8000+PG_MOD+PG_V	; memory page
	word	0x2f9000+PG_MOD+PG_V	; memory page
	word	0x2fa000+PG_MOD+PG_V	; memory page
	word	0x2fb000+PG_MOD+PG_V	; memory page
	word	0x2fc000+PG_MOD+PG_V	; memory page
	word	0x2fd000+PG_MOD+PG_V	; memory page
	word	0x2fe000+PG_MOD+PG_V	; memory page
	word	0x2ff000+PG_MOD+PG_V	; memory page
	word	0x300000+PG_MOD+PG_V	; memory page
	word	0x301000+PG_MOD+PG_V	; memory page
	word	0x302000+PG_MOD+PG_V	; memory page
	word	0x303000+PG_MOD+PG_V	; memory page
	word	0x304000+PG_MOD+PG_V	; memory page
	word	0x305000+PG_MOD+PG_V	; memory page
	word	0x306000+PG_MOD+PG_V	; memory page
	word	0x307000+PG_MOD+PG_V	; memory page
	word	0x308000+PG_MOD+PG_V	; memory page
	word	0x309000+PG_MOD+PG_V	; memory page
	word	0x30a000+PG_MOD+PG_V	; memory page
	word	0x30b000+PG_MOD+PG_V	; memory page
	word	0x30c000+PG_MOD+PG_V	; memory page
	word	0x30d000+PG_MOD+PG_V	; memory page
	word	0x30e000+PG_MOD+PG_V	; memory page
	word	0x30f000+PG_MOD+PG_V	; memory page
	word	0x310000+PG_MOD+PG_V	; memory page
	word	0x311000+PG_MOD+PG_V	; memory page
	word	0x312000+PG_MOD+PG_V	; memory page
	word	0x313000+PG_MOD+PG_V	; memory page
	word	0x314000+PG_MOD+PG_V	; memory page
	word	0x315000+PG_MOD+PG_V	; memory page
	word	0x316000+PG_MOD+PG_V	; memory page
	word	0x317000+PG_MOD+PG_V	; memory page
	word	0x318000+PG_MOD+PG_V	; memory page
	word	0x319000+PG_MOD+PG_V	; memory page
	word	0x31a000+PG_MOD+PG_V	; memory page
	word	0x31b000+PG_MOD+PG_V	; memory page
	word	0x31c000+PG_MOD+PG_V	; memory page
	word	0x31d000+PG_MOD+PG_V	; memory page
	word	0x31e000+PG_MOD+PG_V	; memory page
	word	0x31f000+PG_MOD+PG_V	; memory page
	word	0x320000+PG_MOD+PG_V	; memory page
	word	0x321000+PG_MOD+PG_V	; memory page
	word	0x322000+PG_MOD+PG_V	; memory page
	word	0x323000+PG_MOD+PG_V	; memory page
	word	0x324000+PG_MOD+PG_V	; memory page
	word	0x325000+PG_MOD+PG_V	; memory page
	word	0x326000+PG_MOD+PG_V	; memory page
	word	0x327000+PG_MOD+PG_V	; memory page
	word	0x328000+PG_MOD+PG_V	; memory page
	word	0x329000+PG_MOD+PG_V	; memory page
	word	0x32a000+PG_MOD+PG_V	; memory page
	word	0x32b000+PG_MOD+PG_V	; memory page
	word	0x32c000+PG_MOD+PG_V	; memory page
	word	0x32d000+PG_MOD+PG_V	; memory page
	word	0x32e000+PG_MOD+PG_V	; memory page
	word	0x32f000+PG_MOD+PG_V	; memory page
	word	0x330000+PG_MOD+PG_V	; memory page
	word	0x331000+PG_MOD+PG_V	; memory page
	word	0x332000+PG_MOD+PG_V	; memory page
	word	0x333000+PG_MOD+PG_V	; memory page
	word	0x334000+PG_MOD+PG_V	; memory page
	word	0x335000+PG_MOD+PG_V	; memory page
	word	0x336000+PG_MOD+PG_V	; memory page
	word	0x337000+PG_MOD+PG_V	; memory page
	word	0x338000+PG_MOD+PG_V	; memory page
	word	0x339000+PG_MOD+PG_V	; memory page
	word	0x33a000+PG_MOD+PG_V	; memory page
	word	0x33b000+PG_MOD+PG_V	; memory page
	word	0x33c000+PG_MOD+PG_V	; memory page
	word	0x33d000+PG_MOD+PG_V	; memory page
	word	0x33e000+PG_MOD+PG_V	; memory page
	word	0x33f000+PG_MOD+PG_V	; memory page
	word	0x340000+PG_MOD+PG_V	; memory page
	word	0x341000+PG_MOD+PG_V	; memory page
	word	0x342000+PG_MOD+PG_V	; memory page
	word	0x343000+PG_MOD+PG_V	; memory page
	word	0x344000+PG_MOD+PG_V	; memory page
	word	0x345000+PG_MOD+PG_V	; memory page
	word	0x346000+PG_MOD+PG_V	; memory page
	word	0x347000+PG_MOD+PG_V	; memory page
	word	0x348000+PG_MOD+PG_V	; memory page
	word	0x349000+PG_MOD+PG_V	; memory page
	word	0x34a000+PG_MOD+PG_V	; memory page
	word	0x34b000+PG_MOD+PG_V	; memory page
	word	0x34c000+PG_MOD+PG_V	; memory page
	word	0x34d000+PG_MOD+PG_V	; memory page
	word	0x34e000+PG_MOD+PG_V	; memory page
	word	0x34f000+PG_MOD+PG_V	; memory page
	word	0x350000+PG_MOD+PG_V	; memory page
	word	0x351000+PG_MOD+PG_V	; memory page
	word	0x352000+PG_MOD+PG_V	; memory page
	word	0x353000+PG_MOD+PG_V	; memory page
	word	0x354000+PG_MOD+PG_V	; memory page
	word	0x355000+PG_MOD+PG_V	; memory page
	word	0x356000+PG_MOD+PG_V	; memory page
	word	0x357000+PG_MOD+PG_V	; memory page
	word	0x358000+PG_MOD+PG_V	; memory page
	word	0x359000+PG_MOD+PG_V	; memory page
	word	0x35a000+PG_MOD+PG_V	; memory page
	word	0x35b000+PG_MOD+PG_V	; memory page
	word	0x35c000+PG_MOD+PG_V	; memory page
	word	0x35d000+PG_MOD+PG_V	; memory page
	word	0x35e000+PG_MOD+PG_V	; memory page
	word	0x35f000+PG_MOD+PG_V	; memory page
	word	0x360000+PG_MOD+PG_V	; memory page
	word	0x361000+PG_MOD+PG_V	; memory page
	word	0x362000+PG_MOD+PG_V	; memory page
	word	0x363000+PG_MOD+PG_V	; memory page
	word	0x364000+PG_MOD+PG_V	; memory page
	word	0x365000+PG_MOD+PG_V	; memory page
	word	0x366000+PG_MOD+PG_V	; memory page
	word	0x367000+PG_MOD+PG_V	; memory page
	word	0x368000+PG_MOD+PG_V	; memory page
	word	0x369000+PG_MOD+PG_V	; memory page
	word	0x36a000+PG_MOD+PG_V	; memory page
	word	0x36b000+PG_MOD+PG_V	; memory page
	word	0x36c000+PG_MOD+PG_V	; memory page
	word	0x36d000+PG_MOD+PG_V	; memory page
	word	0x36e000+PG_MOD+PG_V	; memory page
	word	0x36f000+PG_MOD+PG_V	; memory page
	word	0x370000+PG_MOD+PG_V	; memory page
	word	0x371000+PG_MOD+PG_V	; memory page
	word	0x372000+PG_MOD+PG_V	; memory page
	word	0x373000+PG_MOD+PG_V	; memory page
	word	0x374000+PG_MOD+PG_V	; memory page
	word	0x375000+PG_MOD+PG_V	; memory page
	word	0x376000+PG_MOD+PG_V	; memory page
	word	0x377000+PG_MOD+PG_V	; memory page
	word	0x378000+PG_MOD+PG_V	; memory page
	word	0x379000+PG_MOD+PG_V	; memory page
	word	0x37a000+PG_MOD+PG_V	; memory page
	word	0x37b000+PG_MOD+PG_V	; memory page
	word	0x37c000+PG_MOD+PG_V	; memory page
	word	0x37d000+PG_MOD+PG_V	; memory page
	word	0x37e000+PG_MOD+PG_V	; memory page
	word	0x37f000+PG_MOD+PG_V	; memory page
	word	0x380000+PG_MOD+PG_V	; memory page
	word	0x381000+PG_MOD+PG_V	; memory page
	word	0x382000+PG_MOD+PG_V	; memory page
	word	0x383000+PG_MOD+PG_V	; memory page
	word	0x384000+PG_MOD+PG_V	; memory page
	word	0x385000+PG_MOD+PG_V	; memory page
	word	0x386000+PG_MOD+PG_V	; memory page
	word	0x387000+PG_MOD+PG_V	; memory page
	word	0x388000+PG_MOD+PG_V	; memory page
	word	0x389000+PG_MOD+PG_V	; memory page
	word	0x38a000+PG_MOD+PG_V	; memory page
	word	0x38b000+PG_MOD+PG_V	; memory page
	word	0x38c000+PG_MOD+PG_V	; memory page
	word	0x38d000+PG_MOD+PG_V	; memory page
	word	0x38e000+PG_MOD+PG_V	; memory page
	word	0x38f000+PG_MOD+PG_V	; memory page
	word	0x390000+PG_MOD+PG_V	; memory page
	word	0x391000+PG_MOD+PG_V	; memory page
	word	0x392000+PG_MOD+PG_V	; memory page
	word	0x393000+PG_MOD+PG_V	; memory page
	word	0x394000+PG_MOD+PG_V	; memory page
	word	0x395000+PG_MOD+PG_V	; memory page
	word	0x396000+PG_MOD+PG_V	; memory page
	word	0x397000+PG_MOD+PG_V	; memory page
	word	0x398000+PG_MOD+PG_V	; memory page
	word	0x399000+PG_MOD+PG_V	; memory page
	word	0x39a000+PG_MOD+PG_V	; memory page
	word	0x39b000+PG_MOD+PG_V	; memory page
	word	0x39c000+PG_MOD+PG_V	; memory page
	word	0x39d000+PG_MOD+PG_V	; memory page
	word	0x39e000+PG_MOD+PG_V	; memory page
	word	0x39f000+PG_MOD+PG_V	; memory page
	word	0x3a0000+PG_MOD+PG_V	; memory page
	word	0x3a1000+PG_MOD+PG_V	; memory page
	word	0x3a2000+PG_MOD+PG_V	; memory page
	word	0x3a3000+PG_MOD+PG_V	; memory page
	word	0x3a4000+PG_MOD+PG_V	; memory page
	word	0x3a5000+PG_MOD+PG_V	; memory page
	word	0x3a6000+PG_MOD+PG_V	; memory page
	word	0x3a7000+PG_MOD+PG_V	; memory page
	word	0x3a8000+PG_MOD+PG_V	; memory page
	word	0x3a9000+PG_MOD+PG_V	; memory page
	word	0x3aa000+PG_MOD+PG_V	; memory page
	word	0x3ab000+PG_MOD+PG_V	; memory page
	word	0x3ac000+PG_MOD+PG_V	; memory page
	word	0x3ad000+PG_MOD+PG_V	; memory page
	word	0x3ae000+PG_MOD+PG_V	; memory page
	word	0x3af000+PG_MOD+PG_V	; memory page
	word	0x3b0000+PG_MOD+PG_V	; memory page
	word	0x3b1000+PG_MOD+PG_V	; memory page
	word	0x3b2000+PG_MOD+PG_V	; memory page
	word	0x3b3000+PG_MOD+PG_V	; memory page
	word	0x3b4000+PG_MOD+PG_V	; memory page
	word	0x3b5000+PG_MOD+PG_V	; memory page
	word	0x3b6000+PG_MOD+PG_V	; memory page
	word	0x3b7000+PG_MOD+PG_V	; memory page
	word	0x3b8000+PG_MOD+PG_V	; memory page
	word	0x3b9000+PG_MOD+PG_V	; memory page
	word	0x3ba000+PG_MOD+PG_V	; memory page
	word	0x3bb000+PG_MOD+PG_V	; memory page
	word	0x3bc000+PG_MOD+PG_V	; memory page
	word	0x3bd000+PG_MOD+PG_V	; memory page
	word	0x3be000+PG_MOD+PG_V	; memory page
	word	0x3bf000+PG_MOD+PG_V	; memory page
	word	0x3c0000+PG_MOD+PG_V	; memory page
	word	0x3c1000+PG_MOD+PG_V	; memory page
	word	0x3c2000+PG_MOD+PG_V	; memory page
	word	0x3c3000+PG_MOD+PG_V	; memory page
	word	0x3c4000+PG_MOD+PG_V	; memory page
	word	0x3c5000+PG_MOD+PG_V	; memory page
	word	0x3c6000+PG_MOD+PG_V	; memory page
	word	0x3c7000+PG_MOD+PG_V	; memory page
	word	0x3c8000+PG_MOD+PG_V	; memory page
	word	0x3c9000+PG_MOD+PG_V	; memory page
	word	0x3ca000+PG_MOD+PG_V	; memory page
	word	0x3cb000+PG_MOD+PG_V	; memory page
	word	0x3cc000+PG_MOD+PG_V	; memory page
	word	0x3cd000+PG_MOD+PG_V	; memory page
	word	0x3ce000+PG_MOD+PG_V	; memory page
	word	0x3cf000+PG_MOD+PG_V	; memory page
	word	0x3d0000+PG_MOD+PG_V	; memory page
	word	0x3d1000+PG_MOD+PG_V	; memory page
	word	0x3d2000+PG_MOD+PG_V	; memory page
	word	0x3d3000+PG_MOD+PG_V	; memory page
	word	0x3d4000+PG_MOD+PG_V	; memory page
	word	0x3d5000+PG_MOD+PG_V	; memory page
	word	0x3d6000+PG_MOD+PG_V	; memory page
	word	0x3d7000+PG_MOD+PG_V	; memory page
	word	0x3d8000+PG_MOD+PG_V	; memory page
	word	0x3d9000+PG_MOD+PG_V	; memory page
	word	0x3da000+PG_MOD+PG_V	; memory page
	word	0x3db000+PG_MOD+PG_V	; memory page
	word	0x3dc000+PG_MOD+PG_V	; memory page
	word	0x3dd000+PG_MOD+PG_V	; memory page
	word	0x3de000+PG_MOD+PG_V	; memory page
	word	0x3df000+PG_MOD+PG_V	; memory page
	word	0x3e0000+PG_MOD+PG_V	; memory page
	word	0x3e1000+PG_MOD+PG_V	; memory page
	word	0x3e2000+PG_MOD+PG_V	; memory page
	word	0x3e3000+PG_MOD+PG_V	; memory page
	word	0x3e4000+PG_MOD+PG_V	; memory page
	word	0x3e5000+PG_MOD+PG_V	; memory page
	word	0x3e6000+PG_MOD+PG_V	; memory page
	word	0x3e7000+PG_MOD+PG_V	; memory page
	word	0x3e8000+PG_MOD+PG_V	; memory page
	word	0x3e9000+PG_MOD+PG_V	; memory page
	word	0x3ea000+PG_MOD+PG_V	; memory page
	word	0x3eb000+PG_MOD+PG_V	; memory page
	word	0x3ec000+PG_MOD+PG_V	; memory page
	word	0x3ed000+PG_MOD+PG_V	; memory page
	word	0x3ee000+PG_MOD+PG_V	; memory page
	word	0x3ef000+PG_MOD+PG_V	; memory page
	word	0x3f0000+PG_MOD+PG_V	; memory page
	word	0x3f1000+PG_MOD+PG_V	; memory page
	word	0x3f2000+PG_MOD+PG_V	; memory page
	word	0x3f3000+PG_MOD+PG_V	; memory page
	word	0x3f4000+PG_MOD+PG_V	; memory page
	word	0x3f5000+PG_MOD+PG_V	; memory page
	word	0x3f6000+PG_MOD+PG_V	; memory page
	word	0x3f7000+PG_MOD+PG_V	; memory page
	word	0x3f8000+PG_MOD+PG_V	; memory page
	word	0x3f9000+PG_MOD+PG_V	; memory page
	word	0x3fa000+PG_MOD+PG_V	; memory page
	word	0x3fb000+PG_MOD+PG_V	; memory page
	word	0x3fc000+PG_MOD+PG_V	; memory page
	word	0x3fd000+PG_MOD+PG_V	; memory page
	word	0x3fe000+PG_MOD+PG_V	; memory page
	word	0x3ff000+PG_MOD+PG_V	; memory page

; PAGE 4
; comm page 2nd level page table and startup code
	word	0+PG_CI+PG_V		; commpage

; startup code
Xdivz:
	stcr	r1,cr20
	fldcr	r26,cr2			; src1
	fldcr	r27,cr4			; src2
	fldcr	r28,cr5			; dst plus operation
	ldcr	r1,cr5
	stcr	r1,cr19
	stcr	r0,cr3			; kill scoreboard
	tb1	0,r0,511
	ldcr	r1,cr1			; enable fpu and
	and	r1,r1,0xfff2		; mxm & sfd1
	stcr	r1,cr1
	bsr	__divide
	mask	r1,r28,0x1f		; dst only
	mak	r1,r1,5<21>		; dst field of or instruction
	or.u	r28,r0,hi16(fixupins)
	ld	r29,r28,lo16(fixupins)
	or	r29,r29,r1
	st	r29,r28,lo16(fixupins)
	tb1	0,r0,511
	tb1	0,r0,511
	tb1	0,r0,511
	tb1	0,r0,511
fixupins:
	or	r0,r0,r26		; modified for appropriate destination
	tb1	0,r0,511
	ldcr	r1,cr1			; enable fpu and
	or	r1,r1,1			; freeze fpu
	stcr	r1,cr1
	ldcr	r1,cr19			; NIP
	and	r1,r1,0xfffe
	stcr	r1,cr6			; set up FIP
	stcr	r0,cr5			; invlidate NIP
	ldcr	r1,cr20
	rte
	
	global	interrupt
;interrupt:
;
;   All interrupts initially come here.  From here they are vectored
; to the appropriate handler.
;
;	stcr	r1,cr18			; Actually in vector table.
;
;	ldcr	r1,cr8
;	bb1	0,r1,intdflt		; Workaround
;
;	stcr	r2,cr17			; save r2
;
;	ldcr	r1,cr1
;	and.u	r1,r1,0x9fff		; reset BO, SER
;	and	r1,r1,0xfffb		; reset MXM
;	stcr	r1,cr1
;
;	add	r2,r0,(INT_CP+CLR)
;	or.u	r1,r0,hi16(CR0)
;	st.b	r2,r1,lo16(CR0)		; clr INT_CP
;
;	or.u	r2,r0,hi16(COMMPAGE)
;	ld.bu	r1,r2,lo16(NMI_FLAG)
;	st.b	r0,r2,lo16(NMI_FLAG)
;
;	ldcr	r2,cr17			; restore r2
;
;	bb1	1,r1,gotparity		; Bit 1 Parity Error
;	bb1	0,r1,gotnmi		; Bit 0 NMI interrupt from kbd.
;	

	global	_bzero16
_bzero16:
	or	r4,r0,r0
	or	r5,r0,r0
bzero16t:
	subu	r3,r3,0x0010                    
	st.d	r4,r2,0x0000                    
	st.d	r4,r2,0x0008                    
	bcnd.n	ne0,r3,bzero16t
	addu	r2,r2,0x0010                    
	jmp	r1

gotnmi:
	br	Xtrap

interrupt:
Xcaccflt:
Xdaccflt:
Xaddr:
Xinst:
Xpriv:
Xtbnd:
Xiovf:
Xerror:
Xtrap:
	stcr	r1,cr20
	stcr	r0,cr3			; kill scoreboard
	or.u	r1,r0,hi16(__r)
	or	r1,r1,lo16(__r)
	st	r2,r1,8
	ldcr	r2,cr20
	st	r2,r1,4
	st	r3,r1,12

	st	r4,r1,16
	st	r5,r1,20
	st	r6,r1,24
	st	r7,r1,28
	st	r8,r1,32
	st	r9,r1,36
	st	r10,r1,40
	st	r11,r1,44
	st	r12,r1,48
	st	r13,r1,52
	st	r14,r1,56
	st	r15,r1,60
	st	r16,r1,64
	st	r17,r1,68
	st	r18,r1,72
	st	r19,r1,76
	st	r20,r1,80
	st	r21,r1,84
	st	r22,r1,88
	st	r23,r1,92
	st	r24,r1,96
	st	r25,r1,100
	st	r26,r1,104
	st	r27,r1,108
	st	r28,r1,112
	st	r29,r1,116
	st	r30,r1,120
	st	r31,r1,124
	
	ldcr	r2,cr0			; pid
	st	r2,r1,128
	ldcr	r2,cr1			; psr
	st	r2,r1,132
	ldcr	r2,cr2			; epsr
	st	r2,r1,136
	ldcr	r2,cr3			; ssbr
	st	r2,r1,140
	ldcr	r2,cr4			; sxip
	st	r2,r1,144
	ldcr	r2,cr5			; snip
	st	r2,r1,148
	ldcr	r2,cr6			; sfip
	st	r2,r1,152
	ldcr	r2,cr7			; vbr
	st	r2,r1,156
	ldcr	r2,cr8			; dmt0
	st	r2,r1,160
	ldcr	r2,cr9			; dmd0
	st	r2,r1,164
	ldcr	r2,cr10			; dma0
	st	r2,r1,168
	ldcr	r2,cr11			; dmt1
	st	r2,r1,172
	ldcr	r2,cr12			; dmd1
	st	r2,r1,176
	ldcr	r2,cr13			; dma1
	st	r2,r1,180
	ldcr	r2,cr14			; dmt2
	st	r2,r1,184
	ldcr	r2,cr15			; dmd2
	st	r2,r1,188
	ldcr	r2,cr16			; dma2
	st	r2,r1,192

	subu	r31,r31,128
	bsr	_trap
	bsr	_exit
	addu	r31,r31,128
	rte

start:
	ldcr	r5,cr1			; enable fpu and
	and	r5,r5,0xfff2		; misaligned access trap
	stcr	r5,cr1

	tb1	0,r0,0x80		; wait for scorboard to be clear
	stcr	r0,cr3			; clear shadow scoreboard

	addu	r2,r0,0x1000
	stcr	r2,cr7			; vbr

	fstcr	r0,cr62			; Default Inexact FP exceptions off
	fstcr	r0,cr63			; Default Inexact FP exceptions off

	or.u	r3,r0,hi16(ICMMUBASE)
	or	r3,r3,lo16(ICMMUBASE)
	or.u	r4,r0,hi16(DCMMUBASE)
	or	r4,r4,lo16(DCMMUBASE)

	addu	r2,r0,0x2000	; priority protocol
	st	r2,r3,SCTR
	tb1	0,r0,128
	addu	r0,r0,r0
	addu	r0,r0,r0
	st	r2,r4,SCTR
	tb1	0,r0,128
	addu	r0,r0,r0
	addu	r0,r0,r0

	addu	r2,r0,0x2041	; page 2 + CI + VALID
	st	r2,r3,SAPR
	tb1	0,r0,128
	addu	r0,r0,r0
	addu	r0,r0,r0
	st	r2,r4,SAPR
	tb1	0,r0,128
	addu	r0,r0,r0
	addu	r0,r0,r0
	
	or.u	r31,r0,hi16(__tmpstack)
	or	r31,r31,lo16(__tmpstack)
	subu	r31,r31,64
	st	r0,r0,0			; Zero address zero.

; size memory first
	bsr	__memsize
	
; clear bss
	or.u	r3,r0,hi16(_edata)
	or	r3,r3,lo16(_edata)
bssloop:
	st	r0,r3,0
	addu	r3,r3,4
	subu	r13,r3,r2
	bcnd	ne0,r13,bssloop

; size memory
	bsr	__memsize	; size memory
	addu	r30,r0,0
	or	r31,r2,r0	; set stack to location returned by memsize
	subu	r31,r31,64

; bring pc back to life
	bsr	_comminit

; call opus intialization routines
	bsr	__opus

; call device init routine
	bsr	__init

; Set up arguments to main()
	or.u	r4,r0,hi16(_environ)
	ld	r4,r4,lo16(_environ)
	or.u	r3,r0,hi16(__argv0)	; get **argv
	ld	r3,r3,lo16(__argv0)
	or.u	r2,r0,hi16(__argc)	; get argc
	ld	r2,r2,lo16(__argc)

	bsr	_main			; call main()

; Here at _exit, either by call, or by falling through from return
; from main(). 

_exit:
	; Put any desired cleanup here
__exit:
	subu	r2,r0,1
	bsr	__pause

	global	_initcache
_initcache:
; for each CMMU
; Initialize system control register (SCMR) and
; All cache set status ports (CSSP) are set to 0x3f0ff000 and
; ID (SCMID) registers are saved.
;
	or.u	r6,r0,0x3f0f		; Initialize Cache Set Status Port
	or	r6,r6,0xf000		; motorola 88200 bug sheet of (11/3/88)

	or.u	r7,r0,hi16(ICMMUBASE)
	or	r7,r7,lo16(ICMMUBASE)

	or.u	r8,r0,hi16(_cmmuinfo)
	or	r8,r8,lo16(_cmmuinfo)

	add	r3,r0,r0		; loop count

idcachloop:
	ld	r2,r7,SCMID
	st	r2,r8,4
	
	ld	r2,r8,0		; policy
	st	r2,r7,SCTR		; The real System Control Register
	tb1	0,r0,128		; wait for instruction to finish

	add	r4,r0,4096		; 256 Sets * 16
cloop:
	sub	r4,r4,16		; sizeof set is 16
	st	r4,r7,SADR
	tb1	0,r0,128		; wait for instruction to finish
	st	r6,r7,CSSP
	tb1	0,r0,128		; wait for instruction to finish
	bcnd	ne0,r4,cloop

	or	r7,r7,(DCMMUBASE-ICMMUBASE)
	add	r3,r3,1
	bb1.n	0,r3,idcachloop
	add	r8,r8,8
	jmp	r1

	data
	align	8
; System Command Register and the System Control Register
; Note 88200 bug sheet dated 11/3/88 says DO NOT use snooping
;
; policy:
; 0x8000 enables cmmu parity
; 0x4000 enables snooping
; 0x2000 enables priority protocol arbitration (vs fairness)
	global	_cmmuinfo
_cmmuinfo:
	word	0x2000		; I CMMU policy
	word	0		; I CMMU id
	word	0x6000		; D CMMU policy
	word	0		; D CMMU id

	text
	global	_cacheon
_cacheon:
	bcnd	ne0,r2,en1
	or.u	r3,r0,hi16(ICMMUBASE)	; load cmmu pointers
	or	r3,r3,lo16(ICMMUBASE)
	br	en2
en1:
	or.u	r3,r0,hi16(DCMMUBASE)
	or	r3,r3,lo16(DCMMUBASE)

en2:
	or	r2,r0,INVUATC		; invalidate all users ATC
	st	r2,r3,SCMR		; flush code cmmu
	tb1	0,r0,128		; wait for instruction to finish

	or	r2,r0,INVSATC		; invalidate all supv ATC
	st	r2,r3,SCMR		; flush code cmmu
	tb1	0,r0,128		; wait for instruction to finish

	or	r2,r0,0x2201		; page 2 + VALID + WT
	st	r2,r3,SAPR
	tb1	0,r0,128

	jmp	r1

	global	_cacheoff
_cacheoff:
	text

	bcnd	ne0,r2,ds1
	or.u	r3,r0,hi16(ICMMUBASE)	; load cmmu pointers
	or	r3,r3,lo16(ICMMUBASE)
	br	ds2
ds1:
	or.u	r3,r0,hi16(DCMMUBASE)
	or	r3,r3,lo16(DCMMUBASE)

ds2:

	or	r2,r0,INVUATC		; invalidate all users ATC
	st	r2,r3,SCMR		; flush cmmu
	tb1	0,r0,128		; wait for instruction to finish

	or	r2,r0,INVSATC		; invalidate all supv ATC
	st	r2,r3,SCMR		; flush cmmu
	tb1	0,r0,128		; wait for instruction to finish

	or	r2,r0,0x17		; invalidate all data cache
	st	r2,r3,SCMR		; flush code cmmu
	tb1	0,r0,128		; wait for instruction to finish

	or	r2,r0,0x2041		; page 2 + VALID + CI, root pointer
	st	r2,r3,SAPR
	tb1	0,r0,128

	jmp	r1

	global	_swapl
	global	_swaps
	global	_swaplp
	global	_swapsp
	global	_swapla
	global	_swapsa

; swap long value
_swapl:
	extu	r5,r2,8<24>		; byte 3 -> byte 0
	extu	r4,r2,8<16>		; byte 2 -> byte 0
	extu	r3,r2,8<8>		; byte 1 -> byte 0
	mak	r2,r2,8<24>		; byte 0 -> byte 3
	mak	r3,r3,8<16>		; byte 0 -> byte 2
	mak	r4,r4,8<8>		; byte 0 -> byte 1
	or	r4,r4,r5		; or them all in 
	or	r3,r3,r4
	jmp.n	r1
	or	r2,r2,r3

; swap long pointer
_swaplp:
	addu	r6,r0,r2		; save pointer
	ld	r2,r6,0			; get long
	extu	r5,r2,8<24>		; swap it
	extu	r4,r2,8<16>
	extu	r3,r2,8<8>
	mak	r2,r2,8<24>
	mak	r3,r3,8<16>
	mak	r4,r4,8<8>
	or	r4,r4,r5
	or	r3,r3,r4
	or	r2,r2,r3
	jmp.n	r1
	st	r2,r6,0			; store long

; swap long array
_swapla:
	bcnd.n	eq0,r3,laout		; return on zero count
	addu	r6,r0,r2		; r6 long pointer
	addu	r7,r0,r3		; r7 count
laloop:
	ld	r2,r6,0			; get long
	extu	r5,r2,8<24>		; swap it
	extu	r4,r2,8<16>
	extu	r3,r2,8<8>
	mak	r2,r2,8<24>
	mak	r3,r3,8<16>
	mak	r4,r4,8<8>
	or	r4,r4,r5
	or	r3,r3,r4
	or	r2,r2,r3
	st	r2,r6,0			; store long
	sub	r7,r7,1			; decrement count
	bcnd.n	ne0,r7,laloop
	addu	r6,r6,4			; bump pointer
laout:
	jmp.n	r1
	addu	r2,r6,r0		; addr of next long not modified

; swap short value
_swaps:
	extu	r3,r2,8<8>		; byte 1 -> byte 0
	mak	r2,r2,8<8>		; byte 0 -> byte 1
	jmp.n	r1
	or	r2,r2,r3		; or them in

; swap short pointer
_swapsp:
	addu	r6,r0,r2		; save pointer
	ld.hu	r2,r6,0			; get short
	extu	r3,r2,8<8>		; swap it
	mak	r2,r2,8<8>
	or	r2,r2,r3		; or them in
	jmp.n	r1
	st.h	r2,r6,0			; store short

; swap short array
_swapsa:
	bcnd.n	eq0,r3,saout		; return on zero count
	addu	r6,r0,r2		; r6 short pointer
	addu	r7,r0,r3		; r7 count
saloop:
	ld.hu	r2,r6,0			; get short
	extu	r3,r2,8<8>		; swap it
	mak	r2,r2,8<8>
	or	r2,r2,r3
	st.h	r2,r6,0			; store short
	sub	r7,r7,1			; decrement count
	bcnd.n	ne0,r7,saloop
	addu	r6,r6,2			; bump pointer
saout:
	jmp.n	r1
	addu	r2,r6,r0		; addr of next short not modified

;	divide routine that avoids kernel traps
;	r26	numerator
;	r27	denominator
;	r26	result
;	global	__divide
__divide:
	; ?/?	r26/r27
	bb1	31,r26,@L1
	; +/?
	bb1	31,r27,@L2
	; +/+
	jmp.n	r1
	div	r26,r26,r27

@L1:	; -/?
	; -/+
	bb1.n	31,r27,@L3
	subu	r26,r0,r26
	div	r26,r26,r27
	jmp.n	r1
	subu	r26,r0,r26

@L2:	; +/-
	subu	r27,r0,r27
	div	r26,r26,r27
	jmp.n	r1
	subu	r26,r0,r26

@L3:	; -/-
	subu	r27,r0,r27
	jmp.n	r1
	div	r26,r26,r27

	data
	align	8
	global cerror
cerror: word 0			; used by libc
	global	_S_CR0
_S_CR0:
	word	CR0
	global	__argc
	global	__argv0
	global	__argv00
__argc: word		0
__argv0: word	__argv00
__argv00: word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
env00:	word	0
_environ:
	word	env00

global	__MAXDOUBLE
global	__MINDOUBLE
global	__MAXFLOAT
global	__MINFLOAT
	align	8
__MAXDOUBLE:
	word	0x7fefffff
	word	0xffffffff
__MINDOUBLE:
	word	0x00100000
	word	0x00000000
__MAXFLOAT:
	word	0x7f7fffff
__MINFLOAT:
	word	0x00800000
	
	zero	0x100

	align	8
__tmpstack:

	comm	__pagetables,0x8000	; 2nd level page tables to 32MB

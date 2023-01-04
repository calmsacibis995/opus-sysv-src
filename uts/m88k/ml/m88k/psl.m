;		Copyright (c) 1985 AT&T		#
;		All Rights Reserved		#
;	@(#)psl.m	1.0	
;
;	processor status register for 78000
;
;  Bit numbers of interest in the psr.
;
	def	PS_EXP,0	; exception disable bit.
	def	PS_IPL,1	; interrupt priority level 
	def	INTMASK,1	; Mask bit for interrupts.
	def	PS_MXM,2	; Set to disable misaligned exceptions.
	def	PS_SFD1,3	; Disable floating point unit.
	def	PS_MSK,10	; Interrupt mask - software only.
	def	PS_DEXC,27	; Data exception pending.
	def	PS_C,28		; carry bit 
	def	PS_SER,29	; Set serialization mode
	def	PS_CUR,31	; current mode. Supervisor if set 
	def	PS_SUP,31	; current mode. Supervisor if set 
	def	PS_USR,31	; current mode. Supervisor if set 
;
;  Bit numbers of interest in the Data unit transaction registers.
;
	def	TRN_REG,7
	def	TRN_LD,1
	def	TRN_DOUB,13

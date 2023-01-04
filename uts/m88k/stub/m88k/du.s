;	Stub routine for distributed unix - rfs #
;	@(#)du.s	

	text
_rfubyte:	global	_rfubyte		;
_rfuword:	global	_rfuword		;
_rsubyte:	global	_rsubyte		;
_rsuword:	global	_rsuword		;
_rcopyin:	global	_rcopyin		;
_rcopyout:	global	_rcopyout	;
_advfs:		global	_advfs		;
_unadvfs:	global	_unadvfs		;
_rmount:		global	_rmount		;
_rumount:	global	_rumount		;
_rfstart:	global	_rfstart		;
_rdebug:		global	_rdebug		;
_rfstop:		global	_rfstop		;
_rfsys:		global	_rfsys		;
_duustat:	global	_duustat		;
;_dulocking:	global	_dulocking	;
_rnamei1:	global	_rnamei1		;
_rnamei2:	global	_rnamei2		;
_rnamei3:	global	_rnamei3		;
_rnamei4:	global	_rnamei4		;
_riget:		global	_riget		;
_remote_call:	global	_remote_call	;
_remio:		global	_remio		;
_rem_date:	global	_rem_date	;
_unremio:	global	_unremio		;
_remfileop:	global	_remfileop	;
	jmp	r1

;
;	Data definitions
;
	global	_nservers
	global	_idleserver
	global	_srmount
_nservers:
_idleserver:
_srmount:
	word	0			; MUST be zero


;	Stub routine for distributed unix - streams #
;	@(#)stream.s	

	text
_strgetmsg:	global	_strgetmsg	;
_strputmsg:	global	_strputmsg	;
_sealloc:	global	_sealloc		;
_sefree:		global	_sefree		;
_strpoll:	global	_strpoll		;
_stropen:	global	_stropen		;
_strread:	global	_strread		;
_strwrite:	global	_strwrite	;
_strioctl:	global	_strioctl	;
_strclean:	global	_strclean	;
_strclose:	global	_strclose	;
_runqueues:	global	_runqueues	;
	jmp	r1

	data
	global	_qrunflag
	global	_msglistcnt
_qrunflag:
_msglistcnt:
	word	0			; MUST be zero



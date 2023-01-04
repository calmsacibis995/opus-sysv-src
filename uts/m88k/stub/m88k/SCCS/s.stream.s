h53929
s 00026/00000/00000
d D 1.1 90/03/06 12:45:35 root 1 0
c date and time created 90/03/06 12:45:35 by root
e
u
U
t
T
I 1
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


E 1

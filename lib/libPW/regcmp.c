/*	Copyright (c) 1984 AT&T	*/
/*	Copyright (c) 1987 Fairchild Semiconductor Corporation	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	AND FAIRCHILD SEMICONDUCTOR CORPORATION		   	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

#ident	"@(#)sccs:lib/libPW/regcmp.c	4.5	1.3 (Fairchild) 1/22/87"
#include <varargs.h>

#define SSIZE	50
#define TGRP	48
#define A256	02
#define ZERO	01
#define	NBRA	10
#define CIRCFL	32;
#define SLOP	5
#define	NULL	0

#define	CBRA	60
#define GRP	40
#define SGRP	56
#define PGRP	68
#define EGRP	44
#define RNGE	03
#define	CCHR	20
#define	CDOT	64
#define	CCL	24
#define	NCCL	8
#define	CDOL	28
#define	CEOF	52
#define	CKET	12

#define	STAR	01
#define PLUS	02
#define MINUS	16

int	*__sp_;
char	*__stmax;
int	__i_size;
char *
regcmp(va_alist)
va_dcl 
{
	register c;
	register char *ep, *sp;

	va_list	ap;
	int i,cflg;
	char *lastep, *sep, *eptr, *cadx;
	char bracket[NBRA], *bracketp;
	int nbra,ngrp;
	int cclcnt;
	char stack[SSIZE];
#if m88k || ns32000 || clipper
	int nvargs = 0;		/* #  of va_args passed */
#endif

#if clipper && opus
	BUG IN CLIPPER PCC FOR MULTIPLE VARARGS, USE GREENHILLS CC
#endif
	va_start(ap);
	__sp_ = (int *)stack;
	*__sp_ = -1;
	__stmax = &stack[SSIZE];

	i = nbra = ngrp = 0;
#if m88k || ns32000 || clipper
	while(cadx = va_arg(ap, char *)) {
		i += __size(cadx);
		nvargs++;
	}
#else
	while(cadx = va_arg(ap, char *))  i += __size(cadx);
#endif
	va_start(ap);
	cadx = sp = va_arg(ap, char *);
#if m88k || ns32000 || clipper
	nvargs--;
#endif

	if((sep = ep = (char *)malloc(2*i+SLOP)) == (char *)0)
		return(0);
	bracketp = bracket;
	if ((c = *sp++) == NULL) goto cerror;
	if (c=='^') {
		c = *sp++;
		*ep++ = CIRCFL;
	}
	if ((c=='*') || (c=='+') || (c=='{'))
		goto cerror;
	sp--;

	for (;;) {
		if ((c = *sp++) == NULL) {
			if (cadx = va_arg(ap, char *)) {
				sp = cadx;
#if m88k || ns32000 || clipper
				nvargs--;
#endif
				continue;
			}
			*ep++ = CEOF;
			if (--nbra > NBRA || *__sp_ != -1)
				goto cerror;
			__i_size = ep - sep;
			return(sep);
		}

		if ((c!='*') && (c!='{')  && (c!='+'))
			lastep = ep;
		switch (c) {

		case '(':
			if (!__rpush(ep)) goto cerror;
			*ep++ = CBRA;
			*ep++ = -1;
			continue;
		case ')':
			if (!(eptr=(char *)__rpop())) goto cerror;
			if ((c = *sp++) == '$') {
				if ('0' > (c = *sp++) || c > '9')
					goto cerror;
				*ep++ = CKET;
				*ep++ = *++eptr = nbra++;
				*ep++ = (c-'0');
				continue;
			}
			*ep++ = EGRP;
			*ep++ = ngrp++;
			sp--;
			switch (c) {
			case '+':
				*eptr = PGRP;
				break;
			case '*':
				*eptr = SGRP;
				break;
			case '{':
				*eptr = TGRP;
				break;
			default:
				*eptr = GRP;
				continue;
			}
			i = ep - eptr - 2;
			for (cclcnt = 0; i >= 256; cclcnt++)
				i -= 256;
			if (cclcnt > 3) goto cerror;
			*eptr |= cclcnt;
			*++eptr = i;
			continue;

		case '\\':
			*ep++ = CCHR;
			if ((c = *sp++) == NULL)
				goto cerror;
			*ep++ = c;
			continue;

		case '{':
			*lastep |= RNGE;
			cflg = 0;
		nlim:
			if ((c = *sp++) == '}') goto cerror;
			i = 0;
			do {
				if ('0' <= c && c <= '9')
					i = (i*10+(c-'0'));
				else goto cerror;
			} while (((c = *sp++) != '}') && (c != ','));
			if (i>255) goto cerror;
			*ep++ = i;
			if (c==',') {
				if (cflg++) goto cerror;
				if((c = *sp++) == '}') {
					*ep++ = -1;
					continue;
				}
				else {
					sp--;
					goto nlim;
				}
			}
			if (!cflg) *ep++ = i;
			else if ((ep[-1]&0377) < (ep[-2]&0377)) goto cerror;
			continue;

		case '.':
			*ep++ = CDOT;
			continue;

		case '+':
			if (*lastep==CBRA || *lastep==CKET)
				goto cerror;
			*lastep |= PLUS;
			continue;

		case '*':
			if (*lastep==CBRA || *lastep==CKET)
			goto cerror;
			*lastep |= STAR;
			continue;

		case '$':
#if m88k || ns32000 || clipper
			if ((*sp != NULL) || nvargs > 0)
#else
			if ((*sp != NULL) || (cadx))
#endif
				goto defchar;
			*ep++ = CDOL;
			continue;

		case '[':
			*ep++ = CCL;
			*ep++ = 0;
			cclcnt = 1;
			if ((c = *sp++) == '^') {
				c = *sp++;
				ep[-2] = NCCL;
			}
			do {
				if (c==NULL)
					goto cerror;
				if ((c=='-') && (cclcnt>1) && (*sp!=']')) {
					*ep = ep[-1];
					ep++;
					ep[-2] = MINUS;
					cclcnt++;
					continue;
				}
				*ep++ = c;
				cclcnt++;
			} while ((c = *sp++) != ']');
			lastep[1] = cclcnt;
			continue;

		defchar:
		default:
			*ep++ = CCHR;
			*ep++ = c;
		}
	}
   cerror:
	free(sep);
	return(0);
}
__size(strg) char *strg;
{
	int	i;

	i = 1;
	while(*strg++) i++;
	return(i);
}

__rpop() {
	return (*__sp_ == -1)?0:*__sp_--;
}

__rpush(ptr) char *ptr;
{
	if (++__sp_ > (int *)__stmax) return(0);
	*__sp_ = (int)ptr;
	return(1);
}

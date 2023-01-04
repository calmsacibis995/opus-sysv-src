#ifndef __Ictype
#define __Ictype

#define	_U	1	/* A-Z */
#define	_L	2	/* a-z */
#define	_N	4	/* 0-9 */
#define	_S	8	/* white space */
#define	_P	16	/* !(' ' | isalnum) & isprint */
#define	_C	32	/* control character */
#define	_B	64	/* ' ' */
#define	_X	128	/* 0-9, A-F, a-f */

extern unsigned char _ctype[];

extern int tolower(int);
extern int toupper(int);
extern int isalnum(int);
extern int isalpha(int);
extern int iscntrl(int);
extern int isdigit(int);
extern int isgraph(int);
extern int islower(int);
extern int isprint(int);
extern int ispunct(int);
extern int isspace(int);
extern int isupper(int);
extern int isxdigit(int);
extern int isascii(int);
extern int toascii(int);


#define	isalnum(c)	(_ctype[(c)+1] & (_U|_L|_N))
#define	isalpha(c)	(_ctype[(c)+1] & (_U|_L))
#define	iscntrl(c)	(_ctype[(c)+1] & _C)
#define	isdigit(c)	(_ctype[(c)+1] & _N)
#define	isgraph(c)	(_ctype[(c)+1] & (_U|_L|_N|_P))
#define	islower(c)	(_ctype[(c)+1] & _L)
#define	isprint(c)	(_ctype[(c)+1] & (_U|_L|_N|_P|_B))
#define	ispunct(c)	(_ctype[(c)+1] & _P)
#define	isspace(c)	(_ctype[(c)+1] & _S)
#define	isupper(c)	(_ctype[(c)+1] & _U)
#define	isxdigit(c)	(_ctype[(c)+1] & _X)

#define	toascii(c)	((c) & 0x7f)
#define	isascii(c)	(!((c) & ~0x7f))

#ifdef m88k
#define	_toupper(c)	(_ctype[(c)+258])
#define	_tolower(c)	(_ctype[(c)+258])
#else
#define	_toupper(c)	((c) - 'a' + 'A')
#define	_tolower(c)	((c) - 'A' + 'a')
#endif

#endif

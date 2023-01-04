#ifndef __Itermios
#define __Itermios

#include <sys/ioctl.h>

#define	NCCS	19

#define	VINTR	0
#define	VQUIT	1
#define	VERASE	2
#define	VKILL	3
#define	VEOF	4
#define	VEOL	5
#define	VEOL2	6
#define	VMIN	4
#define	VTIME	5
#define	VSWTCH	7
#define	VSTART	8
#define	VSTOP	9
#define	VSUSP	10

#define	CNUL	0
#define	CDEL	0377

#define	CESC	'\\'
#define	CINTR	0177
#define	CQUIT	034
#define	CERASE	'#'
#define	CKILL	'@'
#define	CEOF	04
#define	CSTART	021
#define	CSTOP	023
#define	CSWTCH	032
#define	CNSWTCH	0
#define	CSUSP	042

#define	IGNBRK	0000001
#define	BRKINT	0000002
#define	IGNPAR	0000004
#define	PARMRK	0000010
#define	INPCK	0000020
#define	ISTRIP	0000040
#define	INLCR	0000100
#define	IGNCR	0000200
#define	ICRNL	0000400
#define	IUCLC	0001000
#define	IXON	0002000
#define	IXANY	0004000
#define	IXOFF	0010000

#define	OPOST	0000001
#define	OLCUC	0000002
#define	ONLCR	0000004
#define	OCRNL	0000010
#define	ONOCR	0000020
#define	ONLRET	0000040
#define	OFILL	0000100
#define	OFDEL	0000200
#define	NLDLY	0000400
#define	NL0	0
#define	NL1	0000400
#define	CRDLY	0003000
#define	CR0	0
#define	CR1	0001000
#define	CR2	0002000
#define	CR3	0003000
#define	TABDLY	0014000
#define	TAB0	0
#define	TAB1	0004000
#define	TAB2	0010000
#define	TAB3	0014000
#define	BSDLY	0020000
#define	BS0	0
#define	BS1	0020000
#define	VTDLY	0040000
#define	VT0	0
#define	VT1	0040000
#define	FFDLY	0100000
#define	FF0	0
#define	FF1	0100000

#define	B0	0
#define	B50	000000200000
#define	B75	000000400000
#define	B110	000000600000
#define	B134	000001000000
#define	B150	000001200000
#define	B200	000001400000
#define	B300	000001600000
#define	B600	000002000000
#define	B1200	000002200000
#define	B1800	000002400000
#define	B2400	000002600000
#define	B4800	000003000000
#define	B9600	000003200000
#define	B19200	000003400000
#define EXTA	000003400000
#define	B38400	000003600000
#define EXTB	000003600000

#define	CBAUD	000077600000
#define	CIBAUD	037700000000

#define	CSIZE	0000060
#define	CS5	0
#define	CS6	0000020
#define	CS7	0000040
#define	CS8	0000060
#define	CSTOPB	0000100
#define	CREAD	0000200
#define	PARENB	0000400
#define	PARODD	0001000
#define	HUPCL	0002000
#define	CLOCAL	0004000
#define	LOBLK	0010000

#define	ISIG	0000001
#define	ICANON	0000002
#define	XCASE	0000004
#define	ECHO	0000010
#define	ECHOE	0000020
#define	ECHOK	0000040
#define	ECHONL	0000100
#define	NOFLSH	0000200
#define	TOSTOP	0000400
#define	IEXTEN	0001000

#define	IOCTYPE	0xff00

#define TC_PX_GETATTR	_IOR_('t',0xc0, struct termios)
#define TC_PX_SETATTR	_IOW_('t',0xc1, struct termios)
#define TC_PX_SETATTRD	_IOW_('t',0xc2, struct termios)
#define TC_PX_SETATTRF	_IOW_('t',0xc3, struct termios)
#define TC_PX_DRAIN	_IO_('t',0xc4)
#define TC_PX_FLUSH	_IO_('t',0xc5)
#define TC_PX_BREAK	_IO_('t',0xc6)
#define TC_PX_GETPGRP	_IO_('t',0xc7)
#define TC_PX_SETPGRP	_IO_('t',0xc8)
#define TC_PX_FLOW	_IO_('t',0xc9)
#define TIOCSWINSZ	_IOW_('t',0x67,struct winsize)
#define TIOCGWINSZ	_IOR_('t',0x68,struct winsize)

#define TCIFLUSH	0
#define TCOFLUSH	1
#define TCIOFLUSH	2

#define TCOOFF		0
#define TCOON		1
#define TCIOFF		2
#define TCION		3

typedef	unsigned long	tcflag_t;
typedef	unsigned char	cc_t;

struct termios {
	tcflag_t	c_iflag;
	tcflag_t	c_oflag;
	tcflag_t	c_cflag;
	tcflag_t	c_lflag;
	unsigned char	c_line;
	cc_t		c_cc[NCCS];
};

struct winsize {
	unsigned short ws_row;
	unsigned short ws_col;
	unsigned short ws_xpixel;
	unsigned short ws_ypixel;
};

#define	COUTBAUD	CBAUD
#define	CINBAUD		CIBAUD

#define TCSANOW		0
#define TCSADRAIN	1
#define TCSAFLUSH	2

#define SSPEED		B9600

typedef unsigned int speed_t;

#define	LDIOC	('D'<<8)
#define	LDOPEN	(LDIOC|0)
#define	LDCLOSE	(LDIOC|1)
#define	LDCHG	(LDIOC|2)
#define	LDGETT	(LDIOC|8)
#define	LDSETT	(LDIOC|9)

#define	TERM_NONE	0	/* tty */
#define	TERM_TEC	1	/* TEC Scope */
#define	TERM_V61	2	/* DEC VT61 */
#define	TERM_V10	3	/* DEC VT100 */
#define	TERM_TEX	4	/* Tektronix 4023 */
#define	TERM_D40	5	/* TTY Mod 40/1 */
#define	TERM_H45	6	/* Hewlitt-Packard 45 */
#define	TERM_D42	7	/* TTY Mod 40/2B */

#define TM_NONE		0000	/* use default flags */
#define TM_SNL		0001	/* special newline flag */
#define TM_ANL		0002	/* auto newline on column 80 */
#define TM_LCF		0004	/* last col of last row special */
#define TM_CECHO	0010	/* echo terminal cursor control */
#define TM_CINVIS	0020	/* do not send esc seq to user */
#define TM_SET		0200	/* must be on to set/res flags */

#endif

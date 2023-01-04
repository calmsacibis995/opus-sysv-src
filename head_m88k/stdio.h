#ifndef _STDIO_H_
#define _STDIO_H_


/*	Copyright (c) 1989 OPUS Systems		*/
/*	      All Rights Reserved 		*/

/*	Copyright (c) 1984 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

#ifndef _SIZE_T_
#define	_SIZE_T_
typedef unsigned int	size_t;
#endif

typedef long		fpos_t;

#define BUFSIZ	1024

/* 
 * The FILE struct is not defined by the 88000 OCS but is required 
 * for compilation.
 * Do NOT depend on or use the elements of this structure for an OCS
 * compliant program.
 */
typedef struct {
	int	_cnt;
	unsigned char	*_ptr;
	unsigned char	*_base;
	unsigned char	_flag;
	unsigned char	_file;
	unsigned short	_fill;		/* XXX */
} FILE;

/*
 * _IOLBF means that a file's output will be buffered line by line
 * In addition to being flags, _IONBF, _IOLBF and _IOFBF are possible
 * values for "type" in setvbuf.
 */
#define _IOFBF		0000
#define _IONBF		0004
#define _IOEOF		0020
#define _IOERR		0040
#define _IOLBF		0100

#ifndef NULL
#define NULL		0
#endif /* !NULL */

#ifndef EOF
#define EOF		(-1)
#endif	/* !EOF */

#define stdin		(&__stdinb)
#define stdout		(&__stdoutb)
#define stderr		(&__stderrb)

extern FILE		__stdinb;
extern FILE		__stdoutb;
extern FILE		__stderrb;

#define SEEK_SET	0
#define SEEK_CUR	1
#define SEEK_END	2

#define L_ctermid	9
#define L_cuserid	9
#define P_tmpdir	"/usr/tmp/"
#define L_tmpnam	(sizeof(P_tmpdir) + 15)

#define getchar()	getc(stdin)
#define putchar(x)	putc((x), stdout)

#ifndef __88000_OCS_DEFINED
#define _NFILE	100

/* buffer size for multi-character output to unbuffered files */
#define _SBFSIZ 8

/*
 * _IOLBF means that a file's output will be buffered line by line
 * In addition to being flags, _IONBF, _IOLBF and _IOFBF are possible
 * values for "type" in setvbuf.
 */
#define _IOREAD		0001
#define _IOWRT		0002
#define _IOMYBUF	0010
#define _IORW		0200

#define _bufend(p)	_bufendtab[(p)->_file]
#define _bufsiz(p)	(_bufend(p) - (p)->_base)

#ifndef lint
#define getc(p)		(--(p)->_cnt < 0 ? _filbuf(p) : (int) *(p)->_ptr++)
#define putc(x, p)	(--(p)->_cnt < 0 ? \
			_flsbuf((unsigned char) (x), (p)) : \
			(int) (*(p)->_ptr++ = (unsigned char) (x)))
#define clearerr(p)	((void) ((p)->_flag &= ~(_IOERR | _IOEOF)))
#define feof(p)		((p)->_flag & _IOEOF)
#define ferror(p)	((p)->_flag & _IOERR)
#define fileno(p)	(p)->_file
#endif	/* !lint */

extern FILE	_iob[_NFILE];
extern FILE	*fopen(), *fdopen(), *freopen(), *popen(), *tmpfile();
extern long	ftell();
extern void	rewind(), setbuf();
extern char	*ctermid(), *cuserid(), *fgets(), *gets(), *tempnam(), *tmpnam();
extern int	fclose(), fflush(), fread(), fwrite(), fseek(), fgetc(),
		getw(), pclose(), printf(), fprintf(), sprintf(),
		vprintf(), vfprintf(), vsprintf(), fputc(), putw(),
		puts(), fputs(), scanf(), fscanf(), sscanf(),
		setvbuf(), system(), ungetc();
extern unsigned char *_bufendtab[];
extern int	fgetpos(), fsetpos();
#endif	/* !__88000_OCS_DEFINED */

#endif	/* ! _STDIO_H_ */

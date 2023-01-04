#ifndef __Istdio
#define __Istdio

#define _IOFBF		0000
#define _IOREAD		0001
#define _IOWRT		0002
#define _IONBF		0004
#define _IOMYBUF	0010
#define _IOEOF		0020
#define _IOERR		0040
#define _IOLBF		0100
#define _IORW		0200

#define EOF		(-1)

#define BUFSIZ		1024
#define _FBUFSIZ	16

#define P_tmpdir	"/usr/tmp/"
#define L_tmpnam	(sizeof(P_tmpdir) + 15)
#define TMP_MAX		17576

#define SEEK_SET	0
#define SEEK_CUR	1
#define SEEK_END	2

#define FOPEN_MAX	72
#define _NFILE		72
#define OPEN_MAX	72

#define FILENAME_MAX	512

#define NULL		0

typedef struct {			/* buffering control structure	*/
	int		count;		/* chars left before flush/read	*/
	char		*cur;		/* next character		*/
	char		*buf;		/* ptr to buffer base		*/
	char		flag;		/* flags, see _IOxxx		*/
	char		fd;		/* file descriptor		*/
} FILE;

typedef long fpos_t;			/* file pointer type		*/
typedef unsigned int size_t;		/* sizeof type			*/

extern FILE _iob[FOPEN_MAX];

#ifdef m88k
#define stdin		(&__stdinb)
#define stdout		(&__stdoutb)
#define stderr		(&__stderrb)
extern FILE		__stdinb;
extern FILE		__stdoutb;
extern FILE		__stderrb;
#else
#define stdin		(&_iob[0])
#define stdout		(&_iob[1])
#define stderr		(&_iob[2])
#endif

extern int getc(FILE *);
extern int putc(int, FILE *);
extern int getchar(void);
extern int putchar(int);
extern void clearerr(FILE *);
extern int feof(FILE *);
extern int ferror(FILE *);
extern int fileno(FILE *);

#define getc(stream)		(--(stream)->count < 0 ? _filbuf(stream) : \
			(int)(unsigned char)*(stream)->cur++)
#define putc(c, stream)	(--(stream)->count < 0 ? _flsbuf((c),(stream)) : \
			(int)(unsigned char)(*(stream)->cur++ = (c)))
#define getchar()	getc(stdin)
#define putchar(c)	putc((c), stdout)

#define clearerr(stream) ((void)((stream)->flag &= ~(_IOERR | _IOEOF)))
#define feof(stream)	((stream)->flag & _IOEOF)
#define ferror(stream)	((stream)->flag & _IOERR)
#define fileno(stream)	((stream)->fd)

#define L_ctermid	9
#define L_cuserid	9

extern int fclose(FILE *);
extern int fflush(FILE *);
extern int fgetc(FILE *);
extern int fgetpos(FILE *, fpos_t *);
extern char *fgets(char *, int, FILE *);
extern FILE *fopen(const char *, const char *);
extern int fprintf(FILE *, const char *, ...);
extern int fputc(int, FILE *);
extern int fputs(const char *, FILE *);
extern size_t fread(void *, size_t, size_t, FILE *);
extern FILE *freopen(const char*, const char *, FILE *);
extern int fscanf(FILE *, const char *, ...);
extern int fseek(FILE *, long, int);
extern int fsetpos(FILE *, const fpos_t *);
extern long ftell(FILE *);
extern size_t fwrite(const void *, size_t, size_t, FILE *);
extern char *gets(char *);
extern void perror(const char *);
extern int printf(const char *, ...);
extern int puts(const char *);
extern int remove(const char *);
extern int rename(const char *, const char *);
extern void rewind(FILE *);
extern int scanf(const char *, ...);
extern void setbuf(FILE *, char *);
extern int setvbuf(FILE *, char *, int, size_t);
extern int sprintf(char *, const char *, ...);
extern int sscanf(const char *, const char *, ...);
extern FILE *tmpfile(void);
extern char *tmpnam(char *s);
extern int ungetc(int, FILE *);
extern int vfprintf();
extern int vprintf();
extern int vsprintf();

extern char *ctermid(char *);
extern FILE *fdopen(int, const char *);
extern int getw(FILE *);
extern int pclose(FILE *);
extern FILE *popen(const char *, const char *);
extern int putw(int, FILE *);
extern char *tempnam(const char *, const char *);
extern int vsscanf();

extern int _flsbuf(int c, FILE *);
extern int _filbuf(FILE *);

#endif

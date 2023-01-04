#ifndef __Ifile
#define __Ifile

typedef struct file {
	char	f_flag;
	cnt_t	f_count;
	union {
		struct inode *f_uinode;
		struct file  *f_unext;
	} f_up;
	off_t f_offset;
} file_t;

#define f_inode		f_up.f_uinode
#define f_next		f_up.f_unext

extern struct file file[];
extern struct file *ffreelist;

#define	FOPEN	0xffffffff
#define	FREAD	0x01
#define	FWRITE	0x02
#define	FNDELAY	0x04
#define	FAPPEND	0x08
#define FSYNC	0x10
#define	FNONB	0x40
#define	FMASK	0x7f
#define FNET	0x80
#define	FCREAT	0x100
#define	FTRUNC	0x200
#define	FEXCL	0x400

#endif

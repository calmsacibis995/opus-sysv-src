#ifndef __Iaouthdr
#define __Iaouthdr

typedef	struct aouthdr {
	short	magic;		/* a.out magic				*/
	short	vstamp;		/* version stamp			*/
	long	tsize;		/* .text size				*/
	long	dsize;		/* .data size				*/
	long	bsize;		/* .bss size				*/
	long	entry;		/* entry point				*/
	long	text_start;	/* fileptr to .text			*/
	long	data_start;	/* fileptr to .data			*/
} AOUTHDR;

#endif

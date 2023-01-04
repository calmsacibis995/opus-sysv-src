#ident "@(#)compat.h	1.3 (Fairchild) 10/13/86"

#define TMPFILE0	"as"
#define TMPFILE1	"as"
#define TMPFILE2	"as"
#define TMPFILE3	"as"
#define TMPFILE4	"as"
#define TMPFILE5	"as"
#define TMPFILE6	"as"

#define ASDATE		/*"(1/19/82)"*/  "(as:3/14/84)"
#define M4		"/usr/bin/m4"

	/* following are moved here to 4.2 stdio.h*/
#define P_tmpdir	"/tmp/"
#define L_tmpnam	(sizeof(P_tmpdir) + 15

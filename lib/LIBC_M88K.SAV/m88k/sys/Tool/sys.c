#ident "@(#) $Header: sys.c 4.1 88/04/26 18:07:19 root Exp $ RISC Source Base"
/*
 *  make ../m88k/sys/... files
 */

#include	<stdio.h>

#define	BUFF	256

main (argc, argv)
int	argc;
char	** argv;
{
	char	buff [BUFF];

	while (fgets (buff, BUFF, stdin)) {
		makesys (buff);
	}
}

int
makesys (cmds)
char	*cmds;
{
	FILE	* f;
	char	file[BUFF], funct[BUFF];
	int	trapvalue, retflg;

	sscanf (cmds, "%s %s %d %d", file, funct, &trapvalue, &retflg);

	if(file[0] == '#')
		return(0);
	if (!(f = fopen (file, "w"))) {
		fprintf (stderr, "*** can\'t create %s!\n", cmds);
		return (0);
	}
	fprintf(stderr, "Making file \"%s\" for function \"%s\" trap value \"%d\"\n",
		file, funct, trapvalue);
	fprintf (f, ";\tM88000 %s(2) Routine\n", funct);
	fprintf (f, ";\n");
	fprintf (f, ";\t(C) Copyright 1987,1988 by Motorola, Inc.\n");
	fprintf (f, ";\n");
	fprintf (f, "\tfile\t\"%s\"\n", file);
	fprintf (f, "\ttext\n");
	fprintf (f, "\tglobal\t_%s\n", funct);
	fprintf (f, "_%s:\n", funct);
	fprintf (f, "\ttb0\t0,r0,256+%d\t\t; trap to Unix\n",trapvalue);
	fprintf (f, "\tbr\t_cerror\t\t\t; return error to caller\n");
	if(!retflg){
		fprintf (f, "\tjmp\tr1\t\t\t; return to caller\n");
	}
	else {
		fprintf (f, "\tjmp.n\tr1\t\t\t; return to caller\n");
		fprintf (f, "\tadd\tr2,r0,r0\t\t; with 0.\n");
	}
	fclose (f);
	return (0);
}

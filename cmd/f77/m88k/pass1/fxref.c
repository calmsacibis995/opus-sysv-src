/* SID @(#)cxrefprint.c	1.2 */

#include <stdio.h>

/* states */

#define NONE    0
#define F       1  /* file name changed */
#define V       2  /* variable name changed */
#define VF      3  /* variable and file name changed */

/* fields in output lines */

#define LW 80  /* total line width */
#define VS 0  /* start of variable */
#define VL 11  /* length of variable */
#define FS 12  /* start of file */
#define FL 25  /* length of file */
#define NS 38  /* line numbers start */
#define NL 42  /* line numbers length */

/* page layout */

#define PL 66  /* length of page */
#define TM 3  /* top margin */
#define BM 3  /* bottom margin */

/* parm for flushline */

#define DATA 0
#define BLANK 1

char line[LW];  /* line buffer */
char cfn[100];  /* current file name */
char cvn[100];  /* current variable name */
char fn[100];  /* new file name */
char vn[100];  /* new variable name */
char ln[100];  /* new line number */
int state;  /* state caused by current input line */
int lni;  /* next free index into line for line numbers */
int lno;  /* next line to be printed on page */

main()
{

	cfn[0] = '\0';
	cvn[0] = '\0';
	lno = 0;
	lni = NS;

	while (EOF != scanf("%s %s %s", vn, fn, ln)) {
		state = NONE;
		if (strcmp(cvn, vn) != 0) {
			state += V;
		}
		if (strcmp(cfn, fn) != 0) {
			state += F;
		}
		switch (state) {

			case VF:
			case V:
			strcpy(cvn, vn);
			strcpy(cfn, fn);
			flushline(DATA);
			flushline(BLANK);
			initline(cvn, cfn);
			addline(ln);
			break;

			case F:
			strcpy(cfn, fn);
			flushline(DATA);
			initline("", cfn);
			addline(ln);
			break;

			case NONE:
			addline(ln);
			break;
		}
	}
	flushline(DATA);
}

/*
 * Flush the current line to the output stream if there are
 * any line numbers. Clears the line buffer to blank.
 * Type is DATA to flush the current cross reference line or
 * BLANK to spit out a blank line.
 */
flushline(type)
int type;
{
	char temp[LW];
	int i;

	if (type == BLANK) {
		newline();
	} else if (type == DATA && lni > NS) {
		write(1, line, lni);
		newline();
	}
	memset(line, ' ', LW);
	lni = NS;
}

/*
 * Cause a new line to be started on the output stream and update
 * the line counter.
 */
newline()
{

	write(1, "\n", 1);
}

/*
 * Initialize the variable and file parts of the line buffer
 */
initline(v, f)
char *v;
char *f;
{

	memcpy(&line[VS], v, min(strlen(v), VL));
	memcpy(&line[FS], &f[max(0, strlen(f) - FL)], min(strlen(f), FL));
}

/*
 * Add a line number to the end of the line buffer if it will fit.
 * If not then flush the line buffer and start a new line.
 */
addline(l)
char *l;
{

	if ((lni + 1 + strlen(l)) > LW) {
		flushline(DATA);
	}
	memcpy(&line[lni], " ", 1);
	memcpy(&line[lni+1], l, strlen(l));
	lni += 1 + strlen(l);
}

min(a, b)
int a, b;
{

	return(a < b ? a : b);
}

max(a, b)
int a, b;
{

	return(a > b ? a : b);
}

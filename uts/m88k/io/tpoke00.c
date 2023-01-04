/*	@(#)tpoke00.c	6.3	*/
/* timepoke() routine for machines which do not allow simulation of
a software interrupt to defer execution of timeintr()   */

int NO_TMPK;		/* force variable */
int timeflag;

timepoke ()
{
	timeflag = 1;
}

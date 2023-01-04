h26669
s 00011/00000/00000
d D 1.1 90/03/06 12:29:05 root 1 0
c date and time created 90/03/06 12:29:05 by root
e
u
U
t
T
I 1
/*	@(#)tpoke00.c	6.3	*/
/* timepoke() routine for machines which do not allow simulation of
a software interrupt to defer execution of timeintr()   */

int NO_TMPK;		/* force variable */
int timeflag;

timepoke ()
{
	timeflag = 1;
}
E 1

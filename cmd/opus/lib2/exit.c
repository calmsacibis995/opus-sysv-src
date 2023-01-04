/*	@(#)exit.c	1.1	*/

extern _exitval;

exit (val)
{
	_exitval = val;
	_cleanup ();
	_exit ();
}

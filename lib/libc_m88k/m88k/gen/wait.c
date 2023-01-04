_wait (stat_loc)
int *stat_loc;
{
	return(_waitpid(-1,stat_loc,0));
}

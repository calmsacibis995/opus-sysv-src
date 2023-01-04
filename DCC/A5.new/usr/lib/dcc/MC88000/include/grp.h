#ifndef __Igrp
#define __Igrp

struct group {
	char	*gr_name;	/* name of the group	*/
	char	*gr_passwd;	/* group password	*/
	int	gr_gid;		/* group ID		*/
	char	**gr_mem;	/* array of members	*/
};

typedef unsigned long gid_t;

#endif

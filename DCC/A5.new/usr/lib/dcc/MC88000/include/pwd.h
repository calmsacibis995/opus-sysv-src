#ifndef __Ipwd
#define __Ipwd

struct passwd {
	char	*pw_name;
	char	*pw_passwd;
	int	pw_uid;
	int	pw_gid;
	char	*pw_age;
	char	*pw_comment;
	char	*pw_gecos;
	char	*pw_dir;
	char	*pw_shell;
};

struct comment {
	char	*c_dept;
	char	*c_name;
	char	*c_acct;
	char	*c_bin;
};

typedef unsigned long uid_t;

#endif

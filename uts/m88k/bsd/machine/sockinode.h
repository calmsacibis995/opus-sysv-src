/* struct inode extension */
extern struct xinode 
{
	struct socket *i_socket;
} *xinode;

#define inotoxino(ip)	(&xinode[(ip)-inode])

#define fptosock(fp)	(fp)->f_inode->i_fsptr

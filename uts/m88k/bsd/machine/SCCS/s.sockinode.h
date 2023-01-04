h23235
s 00009/00000/00000
d D 1.1 90/03/06 12:33:08 root 1 0
c date and time created 90/03/06 12:33:08 by root
e
u
U
t
T
I 1
/* struct inode extension */
extern struct xinode 
{
	struct socket *i_socket;
} *xinode;

#define inotoxino(ip)	(&xinode[(ip)-inode])

#define fptosock(fp)	(fp)->f_inode->i_fsptr
E 1

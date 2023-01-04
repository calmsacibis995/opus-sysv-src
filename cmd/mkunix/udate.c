/* This program prints the date that the kernel was built.
   This program must run with "root" permision.
*/

#include <stdio.h>
#include <nlist.h>
#include <fcntl.h>
#include <sys/types.h>

struct nlist nl[] = {
	{ "_verstamp" },
	{ 0 },
};

main()
{
	char c, cbuf[50];
	int ret, fd;
	long ofst, ptr;

	if(nlist("/unix",nl) != 0) {
		printf("Cannot find _verstamp in /unix.\n");
		exit(1);
	}

#ifdef DEBUG
	printf("name = %s\n",nl[0].n_name);
	printf("value = %lx\n",nl[0].n_value);
#endif

	if((fd=open("/dev/kmem",O_RDWR)) < 0) {
		printf("Can't open /dev/kmem\n");
		exit(1);
	}
	if(lseek(fd, (long)nl[0].n_value, 0) < 0) {
		printf("lseek failure\n");
		close(fd);
		exit(1);
	}
	read(fd, cbuf, 30);
	printf("Kernel was built:%s\n",cbuf);
	close(fd);
}

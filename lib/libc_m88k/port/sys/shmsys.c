/*
 * C library -- Shared Memory Operations
 *
 * shmctl(shmid, cmd, buf);
 * shmget(xkey, size, shmflg, paddr);
 * shmat(shmid, shmaddr, shmflg);
 * shmdt(shmaddr);
 *
 */

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

_shmctl(shmid, cmd, buf)
	struct shmid_ds *buf;
{
	return (_shmsys(1, shmid, cmd, buf));
}

_shmget(xkey, size, shmflg, paddr)
	key_t xkey;
{
	return (_shmsys(3, xkey, size, shmflg, paddr));
}

char *_shmat(shmid, shmaddr, shmflg)
	char *shmaddr;
{
	return ((char *)_shmsys(0, shmid, shmaddr, shmflg));
}

_shmdt(shmaddr)
	char *shmaddr;
{
	return (_shmsys(2, shmaddr, 0, 0));
}

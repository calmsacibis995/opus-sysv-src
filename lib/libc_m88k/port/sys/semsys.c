/*
 * C library -- Semaphore Operations
 *
 * semctl(semid, semnum, cmd, arg);
 * semget(key, nsems, semflg);
 * semop(semid, sops, nsops);
 *
 */

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>

_semctl(semid, semnum, cmd, arg)
{
	return (_semsys(0, semid, semnum, cmd, arg));
}

_semget(key, nsems, semflg)
	key_t key;
{
	return (_semsys(1, key, nsems, semflg, 0));
}

_semop(semid, sops, nsops)
	struct sembuf *sops;
{
	return (_semsys(2, semid, sops, nsops, 0));
}

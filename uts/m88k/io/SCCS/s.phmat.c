h52777
s 00037/00000/00000
d D 1.1 90/03/06 12:28:55 root 1 0
c date and time created 90/03/06 12:28:55 by root
e
u
U
t
T
I 1
#include "sys/errno.h"
#include "sys/types.h"
#include "sys/ipc.h"
#include "sys/shm.h"

extern errno;
static _phmid;

phmat(paddr, vaddr, size, shmflg)
register vaddr;
{
	register shmid;

	if (_phmid) {
		errno = EMFILE;
		return -1;
	}
		
	shmid = shmget(IPC_PRIVATE, size, IPC_PHYS|IPC_NOCLEAR, paddr);
	if (shmid == -1)
		return(shmid);
	vaddr = shmat(shmid, vaddr, shmflg);
	if (vaddr != -1)
		_phmid = shmid;
	return(vaddr);
}

phmdt(vaddr)
{
	register ret;

	if ((ret = shmdt(vaddr)) == 0) {
		if ((ret = shmctl( _phmid, IPC_RMID, 0)) == 0)
			_phmid = 0;
	}
	return(ret);
}
E 1

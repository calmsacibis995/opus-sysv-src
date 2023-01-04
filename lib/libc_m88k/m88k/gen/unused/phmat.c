#include "sys/errno.h"
#include "sys/types.h"
#include "sys/ipc.h"
#include "sys/shm.h"

extern errno;
static _phmaddr;

_phmat(paddr, vaddr, size, shmflg)
register vaddr;
{
	register shmid;

	if (_phmaddr) {
		errno = EMFILE;
		return -1;
	}
		
	shmid = _shmget(IPC_PRIVATE, size, IPC_PHYS|IPC_NOCLEAR, paddr);
	if (shmid == -1)
		return(shmid);

	_phmaddr = _shmat(shmid, vaddr, shmflg);

	if (_shmctl(shmid, IPC_RMID, 0) == -1)
		return -1;

	return(_phmaddr);
}

_phmdt(vaddr)
{
	if (_phmaddr == 0 || vaddr != _phmaddr) {
		errno = EINVAL;
		return -1;
	}
	_phmaddr = 0;
	return _shmdt(vaddr);
}

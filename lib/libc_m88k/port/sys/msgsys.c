/*
 * C library -- Message Operations
 *
 * msgget(key, msgflg);
 * msgctl(msqid, cmd, buf);
 * msgrcv(msqid, msgp, msgsz, msgtyp, msgflg);
 * msgsnd(msqid, msgp, msgsz, msgflg);
 *
 */

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

_msgget(key, msgflg)
	key_t key;
{
	return (_msgsys(0, key, msgflg, 0, 0, 0));
}

_msgctl(msqid, cmd, buf)
	struct msqid_ds *buf;
{
	return (_msgsys(1, msqid, cmd, buf, 0, 0));
}

_msgrcv(msqid, msgp, msgsz, msgtyp, msgflg)
	struct mymsg *msgp;
{
	return (_msgsys(2, msqid, msgp, msgsz, msgtyp, msgflg));
}

_msgsnd(msqid, msgp, msgsz, msgflg)
	struct mymsg *msgp;
{
	return (_msgsys(3, msqid, msgp, msgsz, msgflg, 0));
}

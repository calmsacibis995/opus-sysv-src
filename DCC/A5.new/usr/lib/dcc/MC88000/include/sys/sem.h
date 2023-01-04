/*
 *	Copyright 1985 Dataindustrier AB
 *
 *	Semaphore structures
 */

#define	SEM_R		0400		/* Read permission. */
#define	SEM_A		0200		/* Alter permission. */

#define SEM_UNDO	010000		/* Set up adjust on exit. */

#define GETNCNT		3		/* Get semncnt. */
#define GETPID		4		/* Get sempid. */
#define GETVAL		5		/* Get semval. */
#define GETALL		6		/* Get all semval's. */
#define GETZCNT		7		/* Get semzcnt. */
#define SETVAL		8		/* Set semval. */
#define SETALL		9		/* Set all semval's. */

/*
 *	Semaphore set data structure.
 */

#ifdef m88k
struct semid_ds {
	struct ipc_perm	sem_perm;	/* Permission. */
	struct sem	*sem_base;	/* Pointer to first semaphore in set. */
	char		sem_pad[2];
	ushort		sem_nsems;	/* # of semaphores in set. */
	time_t		sem_otime;	/* Last semop time. */
	unsigned long	sem_ousec;
	time_t		sem_ctime;	/* Last change time. */
	unsigned long	sem_cusec;
};
#else
struct semid_ds {
	struct ipc_perm	sem_perm;	/* Permission. */
	struct sem	*sem_base;	/* Pointer to first semaphore in set. */
	ushort		sem_nsems;	/* # of semaphores in set. */
	time_t		sem_otime;	/* Last semop time. */
	time_t		sem_ctime;	/* Last change time. */
};
#endif

/*
 *	Semaphore structure.
 */

struct sem {
	ushort	semval;		/* Semaphore value. */
	short	sempid;		/* Pid of last operation. */
	short	semncnt;	/* # awaiting semval > cval. */
	short	semzcnt;	/* # awaiting semval = cval. */
};

/*
 *	Semaphore structure from user.
 */

struct sembuf {
	ushort	sem_num;	/* semaphore # */
	short	sem_op;		/* semaphore operation */
	short	sem_flg;	/* operation flags */
};

/*
 *	Sem adjust array.
 */

struct sadj {
	struct sadj	*sa_nxt;	/* Pointer to next adj value. */
	ushort		sa_id;		/* Semaphore id. */
	ushort		sa_num;		/* Semaphore number. */
	short		sa_val;		/* Adjust value. */
};

/*
 *	Dnix semaphores.
 */

#define DS_SETEVNT	1
#define DS_WAITEVNT	2
#define DS_SETVAL	3
#define DS_INCVAL	4
#define DS_DECVAL	5

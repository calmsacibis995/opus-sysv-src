/*
 *	Copyright 1985 Dataindustrier AB
 *
 *	Message handling structures
 */

#define	MSG_R		0400		/* Read permission. */
#define	MSG_W		0200		/* Write permission. */
#define MSG_RWAIT	01000
#define MSG_WWAIT	02000
#define MSG_NOERROR	010000		/* No error if big message. */

#ifdef m88k

struct msqid_ds {
	struct ipc_perm	msg_perm;
	struct msg	*msg_first;
	struct msg	*msg_last;
	char		msg_pad1[2];
	ushort		msg_cbytes;
	char		msg_pad2[2];
	ushort		msg_qnum;
	char		msg_pad3[2];
	ushort		msg_qbytes;
	pid_t		msg_lspid;
	pid_t		msg_lrpid;
	time_t		msg_stime;
	unsigned long	msg_susec;
	time_t		msg_rtime;
	unsigned long	msg_rusec;
	time_t		msg_ctime;
	unsigned long	msg_cusec;
};

#define	PMSG	(PZERO + 2)

struct msg {
	struct msg	*msg_next;
	long		msg_type;
	short		msg_ts;
	short		msg_spot;
};

struct msgbuf {
	long	mtype;
	char	mtext[1];
};

struct msginfo {
	int	msgmap,
		msgmax,
		msgmnb,
		msgmni,
		msgssz,
		msgtql;
	ushort	msgseg;
};


#define	MSGLOCK(X)	&msglock[X - msgque]

#else

struct msqid_ds {
	struct ipc_perm	msg_perm;	/* Permissions. */
	unsigned long	msg_base;	/* Phys adr of msg q data. */
	unsigned long	msg_dummy;	/* Not used. */
	ushort		msg_cbytes;	/* Current # bytes on q. */
	ushort		msg_qnum;	/* # of messages on q. */
	ushort		msg_qbytes;	/* Max # of bytes on q. */
	ushort		msg_lspid;	/* Pid of last msgsnd. */
	ushort		msg_lrpid;	/* Pid of last msgrcv. */
	time_t		msg_stime;	/* Last msgsnd time. */
	time_t		msg_rtime;	/* Last msgrcv time. */
	time_t		msg_ctime;	/* Last change time. */
};

/*
 *	Message buffer in user space.
 */

struct msgbuf {
	long		mtype;		/* Message type. */
	char		mtext[1];	/* Message text. */
};

#endif

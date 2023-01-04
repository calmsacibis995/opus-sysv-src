static char SysVNFSID[] = "@(#)mount.c	2.7 System V NFS source";
/*	Copyright (c) 1984 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

/* Modified to do NFS remote mounts */

#ident	"@(#)mount:mount.c	1.42"
#include <nserve.h>
#include <sys/endian.h>
#include <sys/tiuser.h>
#include <sys/stropts.h>
#include <sys/param.h>
#include <sys/types.h>
#include <sys/sysmacros.h>
#include <sys/fs/s5param.h>
#include <sys/stat.h>
#include <sys/statfs.h>
#include <sys/fs/s5filsys.h>
#include <sys/fstyp.h>
#include <sys/fsid.h>
#include <sys/cirmgr.h>
#include <sys/message.h>
#include <sys/rfsys.h>
#include <signal.h>
#include <stdio.h>
#include <fcntl.h>
#include <mnttab.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <errno.h>
#include <nsaddr.h>
#include <pn.h>
#include <sys/hetero.h>
#include <sys/mount.h>
#include <sys/conf.h>

#define MNTTAB		"/etc/mnttab"
#define FSTAB		"/etc/fstab"
#define SEM_FILE	"/etc/.mnt.lock"
#define NETSPEC		"/usr/nserve/netspec"

#ifndef MS_CACHE
#define MS_CACHE	1
#endif
#define MAXFIELD	4
#define MINFIELD	2
#define ERROR		1
#define WARNING		2
#define	RO_BIT		1
#define	REMOTE_BIT	2

#define	LOCAL(x)	(!((x) & 2))
#define	REMOTE(x)	((x) & 2)

#define FORMAT		"%a %b %e %H:%M:%S %Y\n"   /* date time format */
				/* a  -  abbreviated month name */
				/* b  -  abbreviated weekday name */
				/* e  -  day of month */
				/* H  -  hour */
				/* M  -  minute */
				/* S  -  second */
				/* Y  -  Year */
				/* n  -  newline */

extern int   errno;
extern int   optind;
extern char *optarg;
extern char *getcwd();
extern char *strtok();
extern struct address *ns_getaddr();

static int cacheflag = MS_CACHE;
static int roflag = 0;
static int more_info = 0;
static char *fieldv[MAXFIELD];
static char *flg[] = {
		"read/write",
		"read only",
		"read/write/remote",
		"read only/remote"
	     };

static struct mnttab *mtab;
static struct mnttab *last_entry;

static int mtab_size;
static char *n_name = NULL;
static char *findaddr();
static char fsbuf[BUFSIZ]; /* array to hold entries from fstab    */
static char buf[32];	   /* array to hold the special file name *
			    * to be placed into MNTTAB	          */
static char time_buf[50];  /* array to hold data and time */

static	char  *getnetspec();

static char fs__type[FSTYPSZ];
static char fs__opts[64] = "";

main(argc,argv)
int  argc;
char **argv;
{
	char	*usage = "usage: mount [[-r] [-d] [-c] [-f fstyp] special directory]\n";
	char	*special;
	char	*directory;
	char	*fsname = NULL;

	int	 errflag = 0, fflag = 0;
	int	 rec, temprec, rtn, num_args, c;

	struct	 mnttab	curr;
	register struct mnttab *mp;

	/*
	 *	If no arguments are given, print out MNTTAB.
	 */

	if (argc == 1) {
		if ((rtn = read_mnttab(ERROR)) == 0) {
			for(mp = mtab; mp < last_entry; mp++) {
				cftime(time_buf, FORMAT, &mp->mt_time);
				printf("%.32s on %s %s",
				       mp->mt_filsys, mp->mt_dev,
				       flg[mp->mt_ro_flg]);
				if (mp->mt_mntopts[0] != '\0')
					printf(",%s", mp->mt_mntopts);
				printf(" on %s", time_buf);
			}
		}
		exit(rtn);
	}

	signal(SIGHUP,  SIG_IGN);
	signal(SIGQUIT, SIG_IGN);
	signal(SIGINT,  SIG_IGN);

	/*
	 *	check for proper arguments
	 */

	while ((c = getopt(argc, argv, "rdcf:n:")) != EOF) {
		switch (c) {
			case 'r':
				if (roflag & RO_BIT)
					errflag = 1;
				else
					roflag |= RO_BIT;
				break;
			case 'd':
				if (roflag & REMOTE_BIT)
					errflag = 1;
				else
					roflag |= REMOTE_BIT;
				break;
			case 'c':
				cacheflag = 0;
				break;
			case 'f':
				if (fflag)
					errflag = 1;
				else {
					fflag = 1;
					fsname = optarg;
				}
				break;
			case 'n':
				/*
				 *	The -n option is an undocumented
				 *	option used to override the name
				 *	server.  It takes a machine name
				 *	(in the form of "netspec:dom.name")
				 *	as an argument.
				 */
				n_name = optarg;
				break;
			case '?':
				errflag = 1;
		}
	}

	/*
	 *	There must be at least 2 more arguments, the
	 *	special file and the directory.  If the "-r"
	 *	option is the only option given, it can appear at
	 *	the end of the arg list.
	 */

	num_args = argc - optind;

	switch (num_args) {
		case 3:
			/*
			 * The following is for backward compatability.
			 */

			if (strcmp(argv[optind + 2], "-r") == 0) {
				if (roflag)
					errflag = 1;
				else
					roflag = RO_BIT;
			} else
				errflag = 1;
			/*
			 *  Fall through to set special and directory
			 */
		case 2:
			special = argv[optind];
			directory = argv[optind + 1];

			if (!fflag && !(roflag & REMOTE_BIT)) {
				if (parse(argv[optind],argv[optind + 1])) {
					fsname = fieldv[2];
					more_info = 1;
				}
			}
			break;
		case 1:
			more_info = 1;
			if (parse(argv[optind],NULL)) {
				special = fieldv[0];
				directory = fieldv[1];
				if (!fflag)
					fsname = fieldv[2];
			} else
				errflag = 1;
			break;
		default:
			errflag = 1;
	}

	if (errflag) {
		fprintf(stderr, "%s", usage);
		exit(2);
	}

	if (*directory != '/') {
		fprintf(stderr, "mount: directory argument <%s> must be a full path name\n",directory);
		exit(2);
	}

	if (geteuid() != 0) {
		fprintf(stderr, "mount: not super user\n");
		exit(2);
	}

	/*
	 *	Lock a temporary file to prevent many mounts at once.
	 *	This is done to ensure integrity of the MNTTAB.
	 */

	if ((temprec = creat(SEM_FILE, 0600)) == -1 ||
	     lockf(temprec, F_LOCK, 0L) < 0) {
		fprintf(stderr, "mount: warning: cannot lock temp file </etc/mnt.lock>\n");
	}

	/*
	 * 	Perform either the distributed mount or the local mount.
	 *	Only the low-order bit of "roflag" is used by the system
	 *	calls (to denote read-only or read-write).
	 */

	if (REMOTE(roflag))
		dist_mount(special, directory, roflag & RO_BIT);
	else
		loc_mount(special, directory, fsname, roflag & RO_BIT);

	/*
	 *	Update the MNTTAB file.  If an error occurs while
	 *	reading MNTTAB (i.e., read_mnttab returns non-zero)
	 *	simply write the information from this mount into
	 *	MNTTAB.  Otherwise, check MNTTAB for consistency
	 *	and add the information from this mount.
	 *	note that buf[] now contains the special name
	 *	to be placed into MNTTAB.
	 */

	if (read_mnttab(WARNING) != 0) {
		mtab_size = sizeof (struct mnttab);
		memset(&curr, 0, mtab_size);
		mtab = &curr;
		strncpy(mtab->mt_dev, buf, sizeof(mtab->mt_dev));
		strncpy(mtab->mt_filsys, directory, sizeof(mtab->mt_filsys));
		time(&mtab->mt_time);
		mtab->mt_ro_flg = roflag;
		strncpy(mtab->mt_fstyp, fs__type, sizeof(mtab->mt_fstyp));
		if (fs__opts[0] != '\0')
			strncpy(mtab->mt_mntopts, fs__opts, sizeof(mtab->mt_mntopts));
	} else {
		/*
	 	 *	report inconsistencies in MNTTAB.
	 	 */

		for (mp = mtab; mp < last_entry; mp++) {
			errflag = 0;
			/*
			 * If the mount points are the same, then print an error.
			 */
			if (strncmp(mp->mt_filsys,directory,sizeof(mp->mt_filsys)) == 0)
				errflag = 1;
			/*
			 * Else if the devices are the same and the file system
			 * type is not NFS then print an error.  NFS permits
			 * mounting the same file system from the server on
			 * two different mount points.
			 */
			else if (strncmp(mp->mt_dev,buf,sizeof(mp->mt_dev)) == 0)
				if (strncmp(mp->mt_fstyp,NFS,sizeof(mp->mt_fstyp)) != 0)
					errflag = 1;
			if (errflag)
				fprintf(stderr,"mount: warning: %s\t%s already in %s\n",
					mp->mt_dev, mp->mt_filsys,MNTTAB);
		}

		/*
	 	 *	put the entry into the mount table
	 	 */

		strncpy(last_entry->mt_dev, buf, sizeof(last_entry->mt_dev));
		strncpy(last_entry->mt_filsys, directory, sizeof(last_entry->mt_filsys));
		time(&last_entry->mt_time);
		last_entry->mt_ro_flg = roflag;
		strncpy(last_entry->mt_fstyp, fs__type, sizeof(last_entry->mt_fstyp));
		if (fs__opts[0] != '\0')
			strncpy(last_entry->mt_mntopts, fs__opts, sizeof(last_entry->mt_mntopts));
	}

	if ((rec = creat(MNTTAB, 0644)) < 0)
		fprintf(stderr, "mount: warning: cannot create %s\n", MNTTAB);
	else if (write(rec, mtab, mtab_size) < 0)
		fprintf(stderr, "mount: warning: write error to %s\n", MNTTAB);
	exit(0);
}

/*
 *	read_mnttab() attempts to read in MNTTAB.  Upon
 *	successful completion, the following will be set:
 *
 *	mtab :	a pointer to a buffer in memory which contains
 *		MNTTAB plus space to hold one more mount table entry.
 *
 *	mtab_size :	the size in bytes of the buffer pointed
 *			to by mtab.
 *
 *	last_entry :	a pointer to the end of MNTTAB in the
 *			buffer pointed to by mtab.
 */

static
read_mnttab(flag)
int	flag;
{
	char	*mess;
	int	rec;
	struct	stat	 sbuf;

	if (flag == WARNING)
		mess = " warning:";
	else
		mess = "";

	if ((rec = open(MNTTAB,O_RDONLY)) < 0) {
		fprintf(stderr, "mount:%s cannot open %s\n", mess, MNTTAB);
		return(2);
	}

	if (fstat(rec, &sbuf) < 0) {
		fprintf(stderr, "mount:%s cannot stat %s\n", mess, MNTTAB);
		return(2);
	}

	mtab_size = sbuf.st_size + sizeof(struct mnttab );

	if ((mtab = (struct mnttab *)malloc(mtab_size)) == 0) {
		fprintf(stderr, "mount:%s cannot allocate space for %s\n",
			mess, MNTTAB);
		return(2);
	}

	if (read(rec, mtab, sbuf.st_size) != sbuf.st_size) {
		fprintf(stderr, "mount:%s read error on %s\n", mess, MNTTAB);
		return(2);
	}

	last_entry = (struct mnttab *)((char *)mtab + sbuf.st_size);
	memset((char *)last_entry, '\0', sizeof(struct mnttab));

	close(rec);
	return(0);
}

static
loc_mount(special, directory, fsname, rflag)
char 	*special;
char 	*directory;
char	*fsname;
int	rflag;
{
	register char *ptr;
	struct statfs stbuf;
	short fstyp;
	char  fs_buf[FSTYPSZ];
	char *fs_opts = NULL;		/* "" or string of fs options */
	short nfstyp = sysfs(GETFSIND, NFS);
	char *margs = (char *)0;
	int   szmargs = 0;

	/*
	 *	If no fs type is given, make the default type that
	 *	of the root file system.
	 */

	if (fsname == NULL) {
		if (statfs("/", &stbuf, sizeof(stbuf), 0) == -1) {
			fprintf(stderr, "mount: cannot obtain the fs type of root\n");
			exit(2);
		}
		sysfs(GETFSTYP, stbuf.f_fstyp, fs_buf);
		fsname = fs_buf;
	}

	if (more_info == 1) {
		printf("mount -f %s %s%s %s\n", fsname, roflag & RO_BIT? "-r " : "", special, directory);
	}

	fs_opts = strchr(fsname, ',');	/* trim fs options from fs type */
	if (fs_opts) {
		*fs_opts = '\0';
		fs_opts++;
	}

	strncpy(fs__type, fsname, sizeof(fs__type));
	if (fs_opts)
		strncpy(fs__opts, fs_opts, sizeof(fs__opts));

	if (*special == '/' || !strcmp(fsname,NFS))
		strncpy(buf, special, sizeof(buf));
	else {
		/*
		 *	Generate the full path name.
		 */

		if (getcwd(buf,sizeof(buf)) == NULL) {
			fprintf(stderr,"mount: cannot get current directory\n");
			buf[0] = '\0';
		}
		strncat(buf,"/",sizeof(buf)-strlen(buf));
		strncat(buf,special,sizeof(buf)-strlen(buf));
	}

	if (buf[sizeof(buf) - 1] != '\0') {
		buf[sizeof(buf) - 1] = '\0';
	   	fprintf(stderr,"mount: warning: pathname will be truncated in  %s to <%s>\n",
						        MNTTAB, buf);
	}

	/*
	 *	Get the file system type from fsname and
	 *	mount the device.
	 */

	if ((fstyp = sysfs(GETFSIND,fsname)) == -1) {
		fprintf(stderr, "mount: invalid fs type <%s>\n", fsname);
		exit(2);
	}

#ifdef LAINFS
	if (fstyp == nfstyp) {
		static char *mount_nfs();	/* ready to mount NFS file */
		special = mount_nfs(special, fs_opts, &margs, &szmargs);
		if (!special) {
			exit(2);	/* quit if NFS checking fails */
		}
		rflag |= MS_DATA;
	}
#endif

	rflag |= MS_FSS;
	if (mount(special, directory, rflag, fstyp, margs, szmargs)) {
		rpterr(special, directory);
		exit(2);
	}
#ifdef LAINFS
	if (fstyp == nfstyp)	{
		return;			/* no need to check the name */
	}
#endif

	/*
 	 *	If the newly mounted directory was not on a 
	 *	block special device, then return.
	 *	Otherwise, strip any "/"s in the fsname
	 *	and check the label against the mount point.
	 */

	if (statfs(directory, &stbuf, sizeof(stbuf), 0) == -1)
		return;

	ptr = stbuf.f_fname;
	while (*ptr == '/')
		ptr ++;

	if (strncmp(strrchr(directory, '/') + 1, ptr, sizeof(stbuf.f_fname)))
		fprintf(stderr,"mount: <%.6s> mounted as <%s>\n",
			stbuf.f_fname, directory);
}

static
dist_mount(special, directory, rflag )
char	*special;
char	*directory;
int	rflag;
{
#ifndef ATTRFS
	printf("rfs mounts not supported\n");
	exit(1);
#else
	struct address *addr;
	struct token	token;
	char dname[MAXDNAME];
	extern ndata_t ndata;

	if (more_info == 1)
		printf("mount -d%s %s %s\n", roflag & RO_BIT? "r" : "", special, directory);

	/*
	 *	set id to CLIENT; later getoken() will clear it.
	 */

	token.t_id = CLIENT;
	/*
	 *	get an address of the remote resource from the
	 *	name server (or from the given name if "-n" is
	 *	specified).
	 */

	if (n_name == NULL) {
		/*
		 *	Determine if RFS is running or installed
		 *	with the following rfsys() calls.
		 *	If the first call fails, then RFS has not
		 *	been installed.  If the second call
		 *	succeeds, then RFS is not running.
		 */
		if (rfsys(RF_GETDNAME, dname, MAXDNAME) < 0) {
			perror("mount");
			exit(2);
		}
		if (rfsys(RF_SETDNAME, dname, strlen(dname)+1) >= 0) {
			fprintf(stderr, "mount: RFS not running\n");
			exit(2);
		}

		if ((addr = ns_getaddr(special, rflag, token.t_uname)) == (struct address *)NULL) {
			fprintf(stderr,"mount: %s not available\n", special);
			nserror("mount");
			exit(2);
		}
	} else {
		char *tbuf = findaddr(n_name);
		if ((tbuf == (char *)NULL)
		|| (addr = (struct address *)astoa(tbuf, NULL)) == (struct address *)NULL) {
			fprintf(stderr,"mount: invalid address specified: <%s>\n", n_name);
			exit(2);
		}
		strncpy(token.t_uname, dompart(n_name), MAXDNAME);
	}

	strncpy(buf, special, sizeof(buf));

	if (buf[sizeof(buf) - 1] != '\0') {
		buf[sizeof(buf) - 1] = '\0';
	   	fprintf(stderr,"mount: warning: resource will be truncated in  %s to <%s>\n",
						        MNTTAB, buf);
	}

	/*
	 *	tell system to mount device
	 */

	if (rmount(special, directory, &token, rflag)) {
		if (errno != ENOLINK) {
			rpterr(special, directory);
			exit(2);
		}
		if (u_getckt(addr,&token) < 0) {
			fprintf(stderr,"mount: could not connect to remote machine\n");
			exit(2);
		}
		/*
		 *  Perform user and group id mapping for the host.
		 *  NOTE: ndata.n_netname is set via negotiate() in u_getckt().
		 */
		uidmap(0, (char *)NULL, (char *)NULL, &ndata.n_netname[0], 0);
		uidmap(1, (char *)NULL, (char *)NULL, &ndata.n_netname[0], 0);

		if (rmount(special, directory, &token, rflag)) {
			rpterr(special, directory);
			exit(2);
		}
	}
	strncpy(fs__type, DUFST, sizeof(fs__type));
#endif
}

rpterr(bs, mp)
register char *bs, *mp;
{
	switch(errno) {
	case EPERM:
		fprintf(stderr,"mount: not super user\n");
		break;
	case ENXIO:
		fprintf(stderr,"mount: %s no such device\n",bs);
		break;
	case ENOTDIR:
		fprintf(stderr,"mount: %s not a directory\n",mp);
		break;
	case ENOENT:
		fprintf(stderr,"mount: %s or %s, no such file or directory\n", bs, mp);
		break;
	case EINVAL:
		if (roflag & REMOTE_BIT) {
			fprintf(stderr,"mount: %s not a valid resource\n",bs);
		} else {
			fprintf(stderr,"mount: %s not a valid file system\n",bs);
		}
		break;
	case EBUSY:
		fprintf(stderr,"mount: %s is already mounted, %s is busy,\n", bs, mp);
		fprintf(stderr,"       or allowable number of mount points exceeded\n");
		break;
	case ENOTBLK:
		fprintf(stderr,"mount: %s not a block device\n",bs);
		break;
	case EROFS:
		fprintf(stderr,"mount: %s write-protected\n",bs);
		break;
	case ENOSPC:
		if (roflag & REMOTE_BIT) {
			fprintf(stderr,"mount: remote machine cannot accept any more mounts;\n");
			fprintf(stderr,"       NSRMOUNT on remote machine exceeded\n");
		} else {
			fprintf(stderr,"mount: possibly damaged file system\n");
		}
		break;
	default:
		perror("mount");
		fprintf(stderr, "mount: cannot mount %s\n", bs);
	}
}

/*
 *	parse() gets missing information on command line from FSTAB.
 *	If arg2 is NULL, the arg1 is either a directory or special name.
 *	Otherwise, arg1 is special and arg2 is directory.
 */

static
parse(arg1,arg2)
char	*arg1, *arg2;
{

	FILE	*fd;
	int	i, found = 0;
	int	num_fields;
	char	c;

	if ((fd = fopen(FSTAB,"r")) == NULL) {
		if (arg2 == NULL)
			fprintf(stderr,"mount: cannot open <%s>\n", FSTAB);
		return(0);
	}

	while (fgets(fsbuf,sizeof(fsbuf),fd)) {

		for (i = 0; i < MAXFIELD; i++)
			fieldv[i] = NULL;

		num_fields = get_fields(fsbuf);
		if (num_fields < MINFIELD || fieldv[0][0] == '#' )
			continue;

		if (arg2 != NULL) {
			if (strcmp(arg1,fieldv[0]) == 0
			  && strcmp(arg2,fieldv[1]) == 0) {
				found = 1;
				break;
			}
		} else {
			if (strcmp(arg1,fieldv[0]) == 0
		 	  || strcmp(arg1,fieldv[1]) == 0) {
				found = 1;	
				break;
			}
		}
	}

	/*
	 *	If the third field is the "flags" field,
	 *	then set the specified flags.  Since the
	 *	third field is expected to point to the
	 *	fstype, set the third field to the fourth.
	 */

	if (found) {
		if (num_fields > 2 && fieldv[2][0] == '-') {
			i = 1;
			while (c = fieldv[2][i]) {
				if (c == 'd')
					roflag |= REMOTE_BIT;
				if (c == 'r')
					roflag |= RO_BIT;
				i ++;
			}
			fieldv[2] = fieldv[3];
		}
	}

	fclose(fd);
	return(found);
}

static
get_fields(s)
char	*s;
{
	int fieldc = 0;

	if ((fieldv[fieldc] = strtok(s, " \t\n")) == NULL)
		return(fieldc);

	fieldc ++;
	while (fieldc < MAXFIELD
	   && (fieldv[fieldc] = strtok(NULL, " \t\n")) != NULL)
		fieldc ++;

	return(fieldc);
}

#ifdef ATTRFS
static
u_getckt(addr,token)
struct address	*addr;
struct token	*token;
{
	int fd;
	char mypasswd[20];
	struct gdpmisc gdpmisc;
	int pfd;
	int num;
	char modname[FMNAMESZ+10];
	extern ndata_t ndata;

	gdpmisc.hetero = gdpmisc.version = 0;

	if (((pfd = open(PASSFILE, O_RDONLY)) < 0)
	  || ((num = read(pfd, &mypasswd[0], sizeof(mypasswd)-1)) < 0)) {
		strcpy(mypasswd, "np");
	} else { 
		mypasswd[num] = '\0';
		(void) close(pfd);
	}

	if ((fd = att_connect(addr, RFS)) == -1) {
		return(-1);
	}
	if (rf_request(fd, RF_RF) == -1) {
		t_cleanup(fd);
		return(-1);
	}
	if ((gdpmisc.version = negotiate(fd,&mypasswd[0],CLIENT))<0) {
		(void) fprintf(stderr,"mount: negotiations failed\n");
		(void) fprintf(stderr,"mount: possible cause: machine password incorrect\n");
		t_cleanup(fd);
		return(-1);
	}
	gdpmisc.hetero = ndata.n_hetero;
	if (ioctl(fd, I_LOOK, modname) >= 0) {
		if (strcmp(modname, TIMOD) == 0)
			if (ioctl(fd, I_POP) < 0)
				perror("mount: warning");
	}
	if (rfsys(RF_FWFD, fd, token, &gdpmisc) <0) {
		perror("mount");
		(void) t_close(fd);
		return(-1);
	}
	return(0);
}

static
t_cleanup(fd)
int fd;
{
	(void) t_snddis(fd, (struct t_call *)NULL);
	(void) t_unbind(fd);
	(void) t_close(fd);
}

static
char *
findaddr(mach_name)
char *mach_name;
{
	char	*file[2];
	char	*f_name, *f_cmd, *f_addr;
	char	*netspec;
	FILE	*fd;
	int	i;
	char	abuf[BUFSIZ];
	static	char	retbuf[BUFSIZ];

	file[0] = NETMASTER;
	file[1] = DOMMASTER;

	/*
	 *	Create a string of the form "netspec machaddr"
	 *	and return that string or NULL if error.
	 */

	if ((netspec = getnetspec()) == NULL) {
		fprintf(stderr, "mount: cannot obtain network specification\n");
		return(NULL);
	}

	for (i = 0; i < 2; i ++) {
		if ((fd = fopen(file[i], "r")) == NULL)
			continue;
		while (fgets(abuf, sizeof(abuf), fd) != NULL) {
			f_name = strtok(abuf, " \t");
			f_cmd  = strtok(NULL, " \t");
			if ((strcmp(f_cmd, "a") == 0 || strcmp(f_cmd, "A") == 0)
			  && (strcmp(f_name, mach_name) == 0)) {
				strncpy(retbuf, netspec, sizeof(retbuf));
				strncat(retbuf, " ", sizeof(retbuf)-strlen(retbuf));
				if ((f_addr = strtok(NULL, "\n")) != NULL)
					strncat(retbuf, f_addr, sizeof(retbuf)-strlen(retbuf));
				fclose(fd);
				return(retbuf);
			}
		}
	}
	fclose(fd);
	return(NULL);
}

static
char *
getnetspec()
{
	static char netspec[BUFSIZ];
	FILE   *fp;

	if (((fp = fopen(NETSPEC, "r")) == NULL)
	 || (fgets(netspec, BUFSIZ, fp) == NULL))
		return(NULL);
	/*
	 *	get rid of training newline if present.
	 */
	if (netspec[strlen(netspec)-1] == '\n')
		netspec[strlen(netspec)-1] = '\0';

	fclose(fp);
	return(netspec);
}
#endif	/* ATTRFS */



#ifdef LAINFS
/* extra code to support NFS file systems
 *	The header files are here to get around conflicts between
 *	SMI and ATT #defines
 */
#undef SUCCESS
#undef CLIENT
#define INKERNEL
#if !m88k
#include <sys/fs/nfs/time.h>
#endif
#undef INKERNEL
#define __SYS_TYPES__
#include <rpc/rpc.h>
#include <sys/fs/nfs/nfs.h>
#include <rpcsvc/mount.h>
#include <sys/socket.h>
/*char *process_name = "client-mount";*/
#include <rpc/netdb.h>
#undef MNTTAB
#include <rpcsvc/mntent.h>
#include <sys/fs/nfs/nfs_clnt.h>
#include <sys/fs/nfs/mount.h>



/* look for a NFS mount-option string
 */
static char *
hasmntopt(mnt,opt)
register char *mnt;			/* look here */
register char *opt;			/* for this */
{
	register int olen;

	olen = strlen(opt);
	while (mnt != NULL) {
		if (strncmp(opt, mnt, olen) == 0 &&
		    (mnt[olen] == '\0' || mnt[olen] == '=' || mnt[olen] == ','))
			return(mnt);
		if ((mnt = strchr(mnt, ',')) != NULL)
			mnt++;
	}
	return(NULL);
}



/*
 * Return the value of a numeric option of the form foo=x, if
 * option is not found or is malformed, return 0.
 */
static int
nopt(mnt, opt)
char *mnt;
char *opt;
{
	int val = 0;
	char *equal;
	char *str;

	if (str = hasmntopt(mnt, opt)) {
		if (equal = strchr(str, '=')) {
			val = atoi(&equal[1]);
		} else {
			fprintf(stderr, "mount: bad numeric option '%s'\n",
				str);
		}
	}
	return (val);
}



/* mount a NFS file system
 *	Notice that this returns a pointer to a path name for a file
 *	containing the necessary NFS arguements.
 */
static char*
mount_nfs(special, nfs_opts, margs, szmargs)
char *special;				/* remote file system path name */
char *nfs_opts;				/* options for the mount */
char **margs;
int *szmargs;
{
	char *spnam = NULL;
	int spfd;
	int printed = 0;
	struct nfs_args *args = (struct nfs_args *) malloc(sizeof(*args));
	static struct sockaddr_in sin;
	struct hostent *hp;
	static struct fhstatus fhs;
	char *cp;
	char *hostp = &args->hostname[0];
	char *path;
	int s;
	struct timeval timeout;
	CLIENT *client;
	enum clnt_stat rpc_stat;
	int rsize, wsize;
	ushort port;
#define NFS_FAIL (spnam = NULL)

	if (args == NULL)
		return NFS_FAIL;
	*margs = (caddr_t) args;
	*szmargs = sizeof(*args);

	cp = special;
	while ((*hostp = *cp) != ':') {
		if (*cp == '\0' || (cp - special) > HOSTNAMESZ) {
			fprintf(stderr,
				"mount: nfs file system; use host:path\n");
			return NFS_FAIL;
		}
		hostp++;
		cp++;
	}
	while (hostp <= args->hostname + sizeof(args->hostname))
		*hostp++ = '\0';
	path = ++cp;
	/*
	 * Get server address
	 */
	if ((hp = gethostbyname(args->hostname)) == NULL) {
		/*
		 * XXX
		 * Failure may be due to yellow pages, try again
		 */
		if ((hp = gethostbyname(args->hostname)) == NULL) {
			fprintf(stderr,
				"mount: %s not in hosts database\n",
				args->hostname);
			return NFS_FAIL;
		}
	}

	args->flags = 0;
	if (hasmntopt(nfs_opts, MNTOPT_SOFT) != NULL) {
		args->flags |= NFSMNT_SOFT;
	}

	/*
	 * get fhandle of remote path from server mountd
	 */
	memset((char *)&sin, '\0', sizeof(sin));
	memcpy((char *)&sin.sin_addr, hp->h_addr, hp->h_length);
	sin.sin_family = AF_INET;
	timeout.tv_usec = 0;
	timeout.tv_sec = 20;
	s = -1;
	if ((client = clntudp_create(&sin, MOUNTPROG, MOUNTVERS,
				     timeout, &s)) == NULL) {
		if (!printed) {
			fprintf(stderr, "mount: %s server not responding (1)",
				special);
			clnt_pcreateerror("");
			printed = 1;
		}
		return NFS_FAIL;
	}
	if (!bindresvport(s)) {
		fprintf(stderr,"Warning: umount: cannot do local bind.\n");
	}

	client->cl_auth = authunix_create_default();
	timeout.tv_usec = 0;
	timeout.tv_sec = 20;
	rpc_stat = clnt_call(client, MOUNTPROC_MNT, xdr_path, &path,
			     xdr_fhstatus, &fhs, timeout);
	if (rpc_stat != RPC_SUCCESS) {
		if (!printed) {
			fprintf(stderr, "mount: %s server not responding (2)",
				special);
			clnt_perror(client, "");
			printed = 1;
		}
		if (rpc_stat== RPC_TIMEDOUT ||
		    rpc_stat == RPC_PMAPFAILURE ||
		    rpc_stat == RPC_CANTDECODERES ||
		    rpc_stat == RPC_PROGNOTREGISTERED) {
			close(s);
			clnt_destroy(client);
			return NFS_FAIL;
		}
	}
	close(s);
	clnt_destroy(client);

	if (errno = fhs.fhs_status) {
		if (errno == EACCES) {
			fprintf(stderr, "mount: access denied for %s:%s\n",
				args->hostname, path);
		} else {
			fprintf(stderr, "mount: ");
			perror(special);
		}
		return NFS_FAIL;
	}
	if (printed) {
		fprintf(stderr, "mount: %s server ok\n", special);
		printed = 0;
	}

	/*
	 * set mount args
	 */
	args->fh = fhs.fhs_fh;
	args->flags |= NFSMNT_HOSTNAME;
	if (args->rsize = nopt(nfs_opts, "rsize")) {
		args->flags |= NFSMNT_RSIZE;
	}
	if (args->wsize = nopt(nfs_opts, "wsize")) {
		args->flags |= NFSMNT_WSIZE;
	}
	if (args->timeo = nopt(nfs_opts, "timeo")) {
		args->flags |= NFSMNT_TIMEO;
	}
	if (args->retrans = nopt(nfs_opts, "retrans")) {
		args->flags |= NFSMNT_RETRANS;
	}
	if (port = nopt(nfs_opts, "port")) {
		sin.sin_port = htons(port);
	} else {
		sin.sin_port = htons(NFS_PORT);	/* XXX should use portmapper */
	}
	memcpy(&(args->addr), (struct sockaddr *)&sin, sizeof(struct sockaddr));

	return("/dev/nfsd");
}

bindresvport(sd)
	int sd;
{
	ushort port;
	struct sockaddr_in sin;
	int err = -1;

#	define MAX_PRIV (IPPORT_RESERVED-1)
#	define MIN_PRIV (IPPORT_RESERVED/2)

	get_myaddress(&sin);
	sin.sin_family = AF_INET;
	for (port = MAX_PRIV; err && port >= MIN_PRIV; port--) {
		sin.sin_port = htons(port);
		err = bind(sd,&sin,sizeof(sin));
	}
	return(err == 0);
}
#endif /* LAINFS */

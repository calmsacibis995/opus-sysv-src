
#include <stdio.h>
#include <sys/time.h>
#include <sys/signal.h>
#include <sys/errno.h>

extern int sys_nerr, errno;
union host_short {
	short	hs_short;
	struct hs_struct {
#if vax || clipper || ns32000 || i386
		unsigned char hss_low;
		unsigned char hss_high;
#endif
#if u3b2 || mc68000 || m88k
		unsigned char hss_high;
		unsigned char hss_low;
#endif
	} hs_struct;
};

union host_long {
	long	hl_long;
	struct hl_struct {
#if vax || clipper || ns32000 || i386
		unsigned short hls_low;
		unsigned short hls_high;
#endif
#if u3b2 || mc68000 || m88k
		unsigned short hls_high;
		unsigned short hls_low;
#endif
	}hl_struct;
};

union net_short {
	short	ns_short;
	struct ns_struct {
		unsigned char nss_high;
		unsigned char nss_low;
	} ns_struct;
};

union net_long {
	long	nl_long;
	struct nl_struct {
		unsigned short nls_high;
		unsigned short nls_low;
	} nl_struct;
};

unsigned short
__htons(shx)
	unsigned short shx;
{
	union host_short hx;
	union net_short nx;

	hx.hs_short = shx;
	nx.ns_struct.nss_high = hx.hs_struct.hss_high;
	nx.ns_struct.nss_low = hx.hs_struct.hss_low;
	return (nx.ns_short);
}

unsigned short
__ntohs(nx)
	unsigned short nx;
{
	return __htons(nx);
}

long
__htonl(lhx)
	unsigned long lhx;
{
	union host_long hx;
	union net_long nx;

	hx.hl_long = lhx;
	nx.nl_struct.nls_high = __htons(hx.hl_struct.hls_high);
	nx.nl_struct.nls_low = __htons(hx.hl_struct.hls_low);
	return (nx.nl_long);
}

long
__ntohl(nx)
	long nx;
{
	return __htonl(nx);
}

#if !m88k
gettimeofday(tv, tz)
struct timeval *tv;
struct timezone *tz;
{

	tv->tv_sec = _time(0);
	tv->tv_usec = 0;
	if (tz) {
		tz->tz_minuteswest = 0;
		tz->tz_dsttime = 0;
	}
}
#endif

#if !m88k
rename(a, b)
char *a, *b;
{
	int save;

	unlink(b);
	if (link(a, b) == -1) {
		return(-1);
	}
	if (unlink(a) == -1) {
		save = errno;
		unlink(b);
		errno = save;
		return(-1);
	}
	return(0);
}
#endif

long
__random()
{

	return(_lrand48());
}

__srandom(seed)
int seed;
{

	_srand48(seed);
}

#if !m88k
setitimer(which, value, ovalue)
int which;
struct itimerval *value, *ovalue;
{
	int sec, usec;

	sec = value->it_value.tv_sec;
	usec = value->it_value.tv_usec;
	sec += (usec + 500000) / 1000000;	/* round */
	if (sec == 0 && usec != 0)
		sec = 1;
	if (alarm(sec) == -1)
		return(-1);
	else
		return(0);
}
#endif

__killpg(pgrp, sig)
int pgrp, sig;
{

	return(_kill(-pgrp, sig));
}

__utimes(file, tvp)
char *file;
struct timeval tvp[2];
{
#if m88k
	return(_utime(file, tvp));
#else
	struct utimbuf { long actime, modtime; } ut;
	
	ut.actime = tvp[0].tv_sec;
	ut.modtime  = tvp[1].tv_sec;
	return(_utime(file, &ut));
#endif
}

__vhangup()
{

	_kill(_getpgrp(), SIGHUP);
}

#if m88k
/* support 32 bit bsd signals */
#else
static int blockedsigmask;
#endif

/*
 * sigblock
 */
int
__sigblock(mask)
register int mask;
{
#if m88k
	sigset_t set, oset;
	set.s[0] = mask;
	set.s[1] = 0;
	_sigprocmask(SIG_BLOCK, &set, &oset);

	return(oset.s[0]);
#else
	int temp;
	register int oldmask;
	register int i;

	temp = oldmask = blockedsigmask;
	blockedsigmask |= mask;
	for (i = 1; i < NSIG; i++, mask >>= 1, oldmask >>= 1) {
		if ((mask & 1) && !(oldmask & 1)) {
			sighold(i);
		}
	}
	return(temp);
#endif
}

/*
 * sigsetmask
 */
int
__sigsetmask(mask)
register int mask;
{
#if m88k
	sigset_t set, oset;

	_sigprocmask(0, (sigset_t *) 0, &set);	/* get hi word from sys */
	set.s[0] = mask;			/* get lo word from user */
	_sigprocmask(SIG_SETMASK, &set, &oset);

	return(oset.s[0]);
#else
	int temp;
	register int oldmask;
	register int i;

	temp = oldmask = blockedsigmask;
	blockedsigmask = mask;
	for (i = 1; i < NSIG; i++, mask >>= 1, oldmask >>= 1) {
		if (mask & 1) {
			if (!(oldmask & 1))
				sighold(i);
		} else if (oldmask & 1) {
			sigrelse(i);
		}
	}
	return(temp);
#endif
}

/* 
 * sigpause
 */

__bsdsigpause(mask)
{
#if m88k
	sigset_t set;

	_sigprocmask(0, (sigset_t *) 0, &set);	/* get hi word from sys */
	set.s[0] = mask;			/* get lo word from user */
	return(_sigsuspend(&set));
#else
	_sigsetmask(0);			/* release everything */
	pause();			/* await interrupt */
	sigblock(mask);			/* new mask */
	errno = EINTR;			/* expected sigpause errno */
	return -1;
#endif
}

#if m88k
__sigvec(sig, vec, ovec)
register unsigned sig;
register struct sigvec *vec, *ovec;
{
	struct sigaction act, oact;
	int retval;

	act.sa_handler = vec->sv_handler;
	_sigprocmask(0, (sigset_t *) 0, &act.sa_mask);/* get hi word from sys */
	act.sa_mask.s[0] = vec->sv_mask;
	act.sa_flags = vec->sv_flags;

	retval = _sigaction (sig, &act, &oact);

	if (ovec) {
		ovec->sv_handler = oact.sa_handler ;
		ovec->sv_mask = oact.sa_mask.s[0];
		ovec->sv_flags = oact.sa_flags;
	}

	return(retval);
}
#else
static struct sigvec lsv[NSIG];

sv_handler(sig)
register sig;
{
	int omask;

	omask = blockedsigmask;
	sigblock(sigmask(sig) | lsv[sig].sv_mask);
	(*lsv[sig].sv_handler)(sig, 0, 0);
	sigsetmask(omask);
	return;
}

sigvec(sig, vec, ovec)
register unsigned sig;
register struct sigvec *vec, *ovec;
{

	if (sig == 0 || sig >= NSIG) {
		errno = EINVAL;
		return(-1);
	}
	if (ovec)
		*ovec = lsv[sig];

	lsv[sig] = *vec;
	
	if (vec->sv_handler != SIG_DFL && vec->sv_handler != SIG_IGN)
		return(sigset(sig, sv_handler));
	else
		return(sigset(sig, vec->sv_handler));
}
#endif

char *
__index(s, c)
char *s;
char c;
{
	return((char *)_strchr(s, c));
}

char *
__r_index(s, c)
char *s;
char c;
{
	return((char *)_strrchr(s, c));
}

/*
 * bcopy with special code to handle overlapped transfers.
 * Uses memcpy when possible since it may be fast.
 */
__bcopy(src, dest, len)
register char *src;
register char *dest;
register int len;
{
	if (src > dest) {
		if (src > (dest + len)) {
			_memcpy(dest, src, len);
		} else {
			while (len--) {
				*dest++ = *src++;
			}
		}
	} else {
		if (dest > (src + len)) {
			_memcpy(dest, src, len);
		} else {
			src += len;
			dest += len;
			while (len--) {
				*(--dest) = *(--src);
			}
		}
	}
}

__bzero(buf, len)
char *buf;
int len;
{

	_memset(buf, 0, len);
}

int
__bcmp(s1, s2, n)
char *s1, *s2;
int n;
{
	return(_memcmp(s1, s2, n));
}

__ffs(v)
register int v;
{
	register int i, j;

	if (v == 0) {
		return(-1);
	}
	j = 32;
	for (i = 0; i < j; v >>= 1, i++) {
		if (v & 1) {
			return(i + 1);
		}
	}
}

int
__vfork()
{

	return(_fork());
}

__initgroups()
{
	return(-1);
}

#if m88k
__wait3(status, options, rusagep)
{
	return(_waitpid(-1, status, options));
}

__getdtablesize()
{
	return(_openmax());
}
#endif

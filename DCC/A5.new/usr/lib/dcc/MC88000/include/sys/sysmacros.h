/* major number of a device */
#define	major(dev)	(int)((unsigned)(dev)>>8)
#define	bmajor(dev)	(int)(((unsigned)(dev)>>8)&037)
#define	brdev(dev)	((dev)&0x1fff)

/* minor number of a device */
#define	minor(dev)	(int)((dev)&0377)

/* make a device number */
#define	makedev(ma,mi)	(dev_t)(((ma)<<8) | (mi))


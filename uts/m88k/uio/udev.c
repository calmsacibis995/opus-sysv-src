	/* Skeleton UNIX Kernel Driver.
	 * This driver only illustrates setting up
	 * the interrupt vector.
	 * See the udevinit routine. (Also see section 4.2.1.5)
	 */

#include "sys/types.h"

udevopen(dev, flag)
dev_t dev;
{
}

udevclose(dev)
dev_t dev;
{
}

udevread(dev)
dev_t dev;
{
}

udevwrite(dev)
dev_t dev;
{
}

udevioctl(dev, cmd, arg)
dev_t dev;
caddr_t arg;
{
}

udevintr(iorb)
struct iorb *iorb;
{
}

udevinit()
{
	extern int (*intdevsw[])();

	/* initialize interrupt vector */
	intdevsw[17] = udevintr;		/* or */
	intdevsw[18] = udevintr;		/* or */
	intdevsw[19] = udevintr;		/* or */
	intdevsw[24] = udevintr;		/* or */
	intdevsw[25] = udevintr;		/* or */
	intdevsw[26] = udevintr;
}

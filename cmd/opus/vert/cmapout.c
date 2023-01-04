#include "vert.h"

main(argc, argv)
int argc;
char **argv;
{
	register int x;
	int r, g, b;

	v_open(0, 0, 0, 0);
	for (x = 0; x < 16; x++) {
		scanf("%d %d %d", &r, &g, &b);
		v_setcolor(x, r, g, b);
	}
	v_close();
}

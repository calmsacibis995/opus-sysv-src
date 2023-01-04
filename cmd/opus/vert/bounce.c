/*
 * Causes a ball to bounce in ever decreasing steps.
 */

#include "vert.h"
#include <setjmp.h>
#include <signal.h>

jmp_buf jb;
int endit();
int restart();
char xbuf[16384];

#define SQUARESIZE 10

main(argc, argv)
int argc;
char **argv;
{
	int x;
	int y;
	int vx;
	int vy;
	int z;
	short RIGHT;
	short BOTTOM;
	short X;
	short Y;

	if (argc > 4) {
		X = atoi(argv[1]);
		Y = atoi(argv[2]);
		RIGHT = atoi(argv[3]);
		BOTTOM = atoi(argv[4]);
	} else {
		X = 50;
		Y = 50;
		RIGHT = 100;
		BOTTOM = 100;
	}
	signal(SIGINT, endit);
	signal(SIGTERM, endit);
	setjmp(jb);
	v_open(xbuf, 1024, 0, 0);
	v_select(3);
	v_graphicsmode();
	v_setfillattr(4, 6, 0);
	v_setwritingmode(7, 0);
	v_screencolor(2);
	x = 0;
	y = 0;
	vx = 4;
	vy = 0;
	while (1) {
		fill(x, y, SQUARESIZE, SQUARESIZE);
		x += vx;
		if (x > (RIGHT - SQUARESIZE)) {
			x = 2 * (RIGHT - SQUARESIZE) - x;
			vx = -vx;
		} else if (x < 0) {
			x = -x;
			vx = -vx;
		}
		vy++;
		y += vy;
		if (y >= (BOTTOM - SQUARESIZE)) {
			y = BOTTOM - SQUARESIZE;
			if (vy < SQUARESIZE) {
				vy = 1 - vy;
			} else {
				vy = vy / SQUARESIZE - vy;
			}
			if (vy >= 0) {
				x = 0;
				y = 0;
				vx = 4;
				vy = 0;
			}
		}
	}
}

restart()
{

	longjmp(jb, 1);
}

endit()
{


	v_drain();
	v_ibmmode();
	v_close();
	exit(0);
}

fill(x, y, w, h)
int x, y, w, h;
{

	v_absmove(x, 768 - y);
	v_circlefill(10);
}

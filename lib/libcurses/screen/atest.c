/* test -- put the IBM special box drawing char #205 on the screen */
#include <stdio.h>
#include <curses.h>

put_char()
{
  unsigned int c;

  c=206; /* box drawing char (i.e. horizontal bars) */
  clear();
  move(0 , 0);
  addch(c);
  refresh();
}

main()
{
		initscr();
		savetty();
		noecho();
		raw();

  put_char();

		flushinp();
		resetty();
		endwin();
}


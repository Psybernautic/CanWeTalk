#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#include "../inc/winControl.h"

#define DELETE_KEY 127
#define SPACE_KEY 32
#define MAX_INPUT 80


WINDOW *create_a_window(int height, int width, int starty, int startx)
{
	WINDOW *theWindow;

	theWindow = newwin(height, width, starty, startx);
	box(theWindow, 0, 0);			// drawind a box
	wmove(theWindow, 1, 1);			// positioning the cursor at the top
	wrefresh(theWindow);
	return theWindow;
}



void input_window(WINDOW *window, char *input, char *user)
{
	int i = 0;			// For a loop counter
	int ch = 0;			// Numeric value of a char 
	int maxRow = 0;
	int maxCol = 0;
	int row = 1;
	int col = 0;

	clear_window(window);					// Make it clean
	getmaxyx(window, maxRow, maxCol);		// Getting the window size
	memset(input, 0, sizeof(input));
	wmove(window, 1, 1);

	wprintw(window, "%s", "> ");

	while ((ch = wgetch(window)) != '\n')
	{
		if (ch >= SPACE_KEY && ch <= DELETE_KEY && i < MAX_INPUT)
		{
			if (ch != DELETE_KEY && ch != KEY_BACKSPACE && ch != '\b')
			{
				input[i] = ch;								/* Storing the pressed key */
				if (col++ < maxCol - 2)						/* if within window */
				{
					wprintw(window, "%c", input[i]);
					++i;
				}
				else
				{
					col = 1;
					if (row == maxRow - 2)
					{
						scroll(window);						/* go up one line */
						row = maxRow-2;                 	/* stay at the last line */
						wmove(window, row, col);           	/* move cursor to the beginning */
						wclrtoeol(window);                 	/* clear from cursor to eol */
						box(window, 0, 0);                 	/* draw the box again */
					}
					else
					{
						row++;
						wmove(window, row, col);			/* move cursor to the beginning */
						wrefresh(window);
						wprintw(window, "%c", input[i]);	/* display the char recv'd */
					}
					++i;
				}
			}
			if (ch == DELETE_KEY && i > 0)
			{
				input[i - 1] = '\0';
				--i;
				col = 1;
				wmove(window, row, col);
				wclrtoeol(window);
				wrefresh(window);
				wprintw(window, "%s", "> ");
				wprintw(window, "%s", input);

			}
			
		}
	}
}



void display_window(WINDOW *window, char *input, int theRow, int isItBlank)
{
	if(isItBlank == 1) { clear_window(window); }		/* make it a clear window */

	wmove(window, (theRow + 1), 1);					/* position cusor at approp row */
	wprintw(window, "%s", input);
	wrefresh(window);
}



void destroy_window(WINDOW *window)
{
	delwin(window);
}



void clear_window(WINDOW *window)
{
	int i = 0;
	int maxrow, maxcol;
		
	getmaxyx(window, maxrow, maxcol);
	for (i = 1; i < maxrow-2; i++)  
	{
		wmove(window, i, 1);
		refresh();
		wclrtoeol(window);
		wrefresh(window);
	}
	box(window, 0, 0);             /* draw the box again */
	wrefresh(window);
}
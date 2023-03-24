#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#include "../inc/winControl.h"

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
	bzero(input, BUFSIZ);
	wmove(window, 1, 1);

	wprintw(window, "%s", user);
	wprintw(window, "%s", "> ");

	for (i = 0; (ch = wgetch(window)) != '\n'; i++)
	{
		input[i] = ch;                       	/* '\n' not copied */
		if (col++ < maxCol-2)               	/* if within window */
		{
			wprintw(window, "%c", input[i]);      	/* display the char recv'd */
		}
		else                                	/* last char pos reached */
		{
			col = 1;
			if (row == maxRow-2)              		/* last line in the window */
			{
				scroll(window);                    	/* go up one line */
				row = maxRow-2;                 	/* stay at the last line */
				wmove(window, row, col);           	/* move cursor to the beginning */
				wclrtoeol(window);                 	/* clear from cursor to eol */
				box(window, 0, 0);                 	/* draw the box again */
			} 
			else
			{
				row++;
				wmove(window, row, col);           /* move cursor to the beginning */
				wrefresh(window);
				wprintw(window, "%c", input[i]);    /* display the char recv'd */
			}
		}
	}
	
}



void display_window(WINDOW *window, char *input, int theRow, int isItBlank)
{
	if(isItBlank == 1) clear_window(window);		/* make it a clear window */
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
	int maxrow = 0;
	int maxcol = 0;
		
	getmaxyx(window, maxrow, maxcol);
	for (i = 1; i < maxcol-2; i++)  
	{
		wmove(window, i, 1);
		refresh();
		wclrtoeol(window);
		wrefresh(window);
	}
	box(window, 0, 0);             /* draw the box again */
	wrefresh(window);
}
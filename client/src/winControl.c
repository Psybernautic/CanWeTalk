/*
FILE                : winControl.c
PROJECT             : Can We Talk?
PROGRAMMER          : Sebastian Posada, Angel Aviles, Jonathon Gregoric
FIRST VERSION       : 2023-03-20
DESCRIPTION         : This file contains the function definitions used for the
					displays creation and management by the use of ncurses
*/

#include "../inc/winControl.h"

#define DELETE_KEY 127
#define SPACE_KEY 32
#define MAX_INPUT 80



//
// FUNCTION     : create_a_window
// DESCRIPTION  :
//  This function will create the a window according to the specified
//	inputs from the arguments
//
// PARAMETERS
//  int height	:	The height of the screen
//	int width	:	The width of the screen
//	int starty	:	The y coordinate from the starting position from cursor
//	int startx	:	The x coordinate from the starting position from cursor
//                      
//
// RETURNS      :   WINDOW
WINDOW *create_a_window(int height, int width, int starty, int startx)
{
	WINDOW *theWindow;

	theWindow = newwin(height, width, starty, startx);
	box(theWindow, 0, 0);			// drawind a box
	wmove(theWindow, 1, 1);			// positioning the cursor at the top
	wrefresh(theWindow);
	return theWindow;
}



//
// FUNCTION     : input_window
// DESCRIPTION  :
//  This function will manage the user input character per character from stdin.
//	It will receive stdin in order to receive and write a message into a buffer.
//	It will limit the max number of characters to 80 per input
//
// PARAMETERS
//  WINDOW *window	:	This is the pointer to the window that will serve as the
//						input entry of the stdin
//	char *input		:	This will be the buffer in which the stdin will be written to
//	char *user		:	This will be the name of the user running the client instance
//                      
//
// RETURNS			:	Void
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



//
// FUNCTION     : display_window
// DESCRIPTION  :
//  This function will manage the display window in which the messages sent or received
//	during the instance duration of the client. It will process and print line by line
//	as the messages are received by this function.
//
// PARAMETERS
//  WINDOW *window	:	This is the pointer to the window that will serve as the
//						prompt screen of the messages that have been received or sent.
//	char *input		:	This will be the buffer that contains the message to be prompt
//	int theRow		:	This will line number in which the message will be written
//	int isItBlank	:	This will indicate if the screen will be cleaned before printing
//						the message or not, 0 for leaving as is, 1 to clean the screen
//                      
//
// RETURNS			:	Void
void display_window(WINDOW *window, char *input, int theRow, int isItBlank)
{
	if(isItBlank == 1) { clear_window(window); }		/* make it a clear window */

	wmove(window, (theRow + 1), 1);					/* position cusor at approp row */
	wprintw(window, "%s", input);
	wrefresh(window);
}



//
// FUNCTION     : destroy_window
// DESCRIPTION  :
//  This function will terminate the window
//
// PARAMETERS
//  WINDOW *window	:	This is the pointer to the window that will be destroyed.
//                      
//
// RETURNS			:	Void
void destroy_window(WINDOW *window)
{
	delwin(window);
}



//
// FUNCTION     : clear_window
// DESCRIPTION  :
//  This function will clear the specified window line by line
//
// PARAMETERS
//  WINDOW *window	:	This is the pointer to the window that will cleaned from any content.
//                      
//
// RETURNS			:	Void
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